#include "serialization.h"

void Serialization::Serial::DeserializeCatalogue(Transport::Base::TransportCatalogue &db) {
	std::vector<std::string_view> stop_ids(transport_catalogue_proto_.stops().size());
	for (const auto &stop : transport_catalogue_proto_.stops()) {
		db.AddStop( { stop.name(), { stop.latitude(), stop.longitude() } });
		stop_ids[stop.id()] = stop.name();
	}
	for (const auto &bus : transport_catalogue_proto_.buses()) {
		Transport::Base::Bus new_bus;
		new_bus.bus_name = bus.name();
		for (const auto stop_id : bus.stops()) {
			new_bus.stops.push_back(db.FindStop(stop_ids[stop_id]));
		}
		new_bus.is_circle = bus.is_circle();
		db.AddBus(new_bus);
	}
	for (const auto &distance : transport_catalogue_proto_.distances()) {
		std::string_view start = db.FindStop(stop_ids[distance.start()])->stop_name;
		std::string_view end = db.FindStop(stop_ids[distance.end()])->stop_name;
		db.AddDistance(std::make_pair(start, end), distance.distance());
	}
}

void Serialization::Serial::SerializeCatalogue(const Transport::Base::TransportCatalogue &db) {
	std::unordered_map<std::string_view, uint32_t> stop_ids;
	uint32_t id = 0;
	for (const auto& [stop_name, stop] : db.GetStops()) {
		protobuf::Stop stop_proto;
		stop_proto.set_id(id);
		stop_proto.set_name(stop->stop_name);
		stop_proto.set_latitude(stop->coordinates.lat);
		stop_proto.set_longitude(stop->coordinates.lng);
		stop_ids[stop_name] = id;
		*transport_catalogue_proto_.add_stops() = stop_proto;
		++id;
	}

	for (const auto& [bus_name, bus] : db.GetBuses()) {
		protobuf::Bus bus_proto;
		bus_proto.set_name(static_cast<std::string>(bus_name));
		for (const auto stop : bus->stops) {
			bus_proto.add_stops(stop_ids.at(stop->stop_name));
		}
		bus_proto.set_is_circle(bus->is_circle);
		*transport_catalogue_proto_.add_buses() = bus_proto;
	}

	for (const auto& [stops, dist] : db.GetDistances()) {
		protobuf::Distance distance;
		distance.set_start(stop_ids.at(stops.first));
		distance.set_end(stop_ids.at(stops.second));
		distance.set_distance(dist);
		*transport_catalogue_proto_.add_distances() = distance;
	}
}

protobuf::Color Serialization::Serial::SerializeColor(const svg::Color &color) {

	protobuf::Color color_proto;

	if (std::holds_alternative<std::monostate>(color)) {
		color_proto.set_is_none(true);
	} else if (std::holds_alternative<svg::Rgb>(color)) {
		svg::Rgb rgb = std::get<svg::Rgb>(color);
		color_proto.mutable_rgb()->set_red(rgb.red);
		color_proto.mutable_rgb()->set_green(rgb.green);
		color_proto.mutable_rgb()->set_blue(rgb.blue);
	} else if (std::holds_alternative<svg::Rgba>(color)) {
		svg::Rgba rgba = std::get<svg::Rgba>(color);
		color_proto.mutable_rgba()->set_red(rgba.red);
		color_proto.mutable_rgba()->set_green(rgba.green);
		color_proto.mutable_rgba()->set_blue(rgba.blue);
		color_proto.mutable_rgba()->set_opacity(rgba.opacity);
	} else if (std::holds_alternative<std::string>(color)) {
		color_proto.set_str_color(std::get<std::string>(color));
	}
	return color_proto;
}

svg::Color Serialization::Serial::DeserializeColor(const protobuf::Color &color_proto) {
	svg::Color color;
	if (color_proto.has_rgb()) {
		svg::Rgb rgb;
		rgb.red = color_proto.rgb().red();
		rgb.green = color_proto.rgb().green();
		rgb.blue = color_proto.rgb().blue();
		color = rgb;
	} else if (color_proto.has_rgba()) {
		svg::Rgba rgba;
		rgba.red = color_proto.rgba().red();
		rgba.green = color_proto.rgba().green();
		rgba.blue = color_proto.rgba().blue();
		rgba.opacity = color_proto.rgba().opacity();
		color = rgba;
	} else {
		color = color_proto.str_color();
	}
	return color;
}

Serialization::Serial::Serial(const Serialization::SerializationSettings &settings) :
		settings_(settings) {
}

void Serialization::Serial::SerializeMapRender(const Transport::Renderer::MapRenderer &mr) {
	auto render_settings = mr.GetSettings();
	protobuf::RenderSettings render_settings_proto;
	render_settings_proto.set_width(render_settings.width);
	render_settings_proto.set_height(render_settings.height);
	render_settings_proto.set_padding(render_settings.padding);
	render_settings_proto.set_line_width(render_settings.line_width);
	render_settings_proto.set_stop_radius(render_settings.stop_radius);
	render_settings_proto.set_bus_label_font_size(render_settings.bus_label_font_size);
	render_settings_proto.set_bus_label_offset_x(render_settings.bus_label_offset_x);
	render_settings_proto.set_bus_label_offset_y(render_settings.bus_label_offset_y);
	render_settings_proto.set_stop_label_font_size(render_settings.stop_label_font_size);
	render_settings_proto.set_stop_label_offset_x(render_settings.stop_label_offset_x);
	render_settings_proto.set_stop_label_offset_y(render_settings.stop_label_offset_y);
	*render_settings_proto.mutable_underlayer_color() = SerializeColor(render_settings.underlayer_color);
	render_settings_proto.set_underlayer_width(render_settings.underlayer_width);
	const auto &colors = render_settings.color_palette;
	for (const auto &color : render_settings.color_palette) {
		*render_settings_proto.add_color_palette() = SerializeColor(color);
	}
	*transport_catalogue_proto_.mutable_render_settings() = render_settings_proto;
}

void Serialization::Serial::DeserializeMapRender(Transport::Renderer::MapRenderer &mr) {
	Transport::Renderer::MapRenderer::RenderSettings render_settings;
	protobuf::RenderSettings render_setting_proto = transport_catalogue_proto_.render_settings();
	render_settings.width = render_setting_proto.width();
	render_settings.height = render_setting_proto.height();
	render_settings.padding = render_setting_proto.padding();
	render_settings.line_width = render_setting_proto.line_width();
	render_settings.stop_radius = render_setting_proto.stop_radius();
	render_settings.bus_label_font_size = render_setting_proto.bus_label_font_size();
	render_settings.bus_label_offset_x = render_setting_proto.bus_label_offset_x();
	render_settings.bus_label_offset_y = render_setting_proto.bus_label_offset_y();
	render_settings.stop_label_font_size = render_setting_proto.stop_label_font_size();
	render_settings.stop_label_offset_x = render_setting_proto.stop_label_offset_x();
	render_settings.stop_label_offset_y = render_setting_proto.stop_label_offset_y();
	render_settings.underlayer_color = DeserializeColor(transport_catalogue_proto_.render_settings().underlayer_color());
	render_settings.underlayer_width = render_setting_proto.underlayer_width();
	for (const auto& color_proto : transport_catalogue_proto_.render_settings().color_palette()) {
	        render_settings.color_palette.push_back(DeserializeColor(color_proto));
	    }
	mr.SetSettings(render_settings);
}

void Serialization::Serial::SerializeRouterSettings(const Transport::Base::TransportRouter& tr) {
	auto router_settings = tr.GetSettings();
	auto router_settings_proto = transport_catalogue_proto_.mutable_routing_settings();
	router_settings_proto->set_bus_velocity(router_settings.bus_velocity);
	router_settings_proto->set_bus_wait_time(router_settings.waiting_time);
}

void Serialization::Serial::DeserializeRouterSettings(Transport::Base::TransportRouter& tr) {
	auto router_settings_proto = *transport_catalogue_proto_.mutable_routing_settings();
	Transport::Base::RoutingSettings routing_settings;
	routing_settings.bus_velocity = router_settings_proto.bus_velocity();
	routing_settings.waiting_time = router_settings_proto.bus_wait_time();
	tr.AddRouterSettings(routing_settings);
}

void Serialization::Serial::Serialize(const Transport::Base::TransportCatalogue &db, const Transport::Renderer::MapRenderer &mr, const Transport::Base::TransportRouter& tr) {
	std::ofstream out(settings_.file_name, std::ios::binary);
	SerializeCatalogue(db);
	SerializeMapRender(mr);
	SerializeRouterSettings(tr);
	transport_catalogue_proto_.SerializeToOstream(&out);
}

void Serialization::Serial::Deserialize(Transport::Base::TransportCatalogue &db, Transport::Renderer::MapRenderer &mr, Transport::Base::TransportRouter& tr) {
	std::ifstream in(settings_.file_name, std::ios::binary);
	const auto is_serialized = transport_catalogue_proto_.ParseFromIstream(&in);
	DeserializeCatalogue(db);
	DeserializeMapRender(mr);
	DeserializeRouterSettings(tr);
}
