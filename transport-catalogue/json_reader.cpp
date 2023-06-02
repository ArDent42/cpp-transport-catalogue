#include <algorithm>
#include "json_reader.h"

using namespace std::literals;

namespace Transport::Detail::Read {

void JsonReader::AddStopsToCatalogue(Transport::Base::TransportCatalogue &db) {
	json::Array base_requests = document_.GetRoot().AsDict().at("base_requests").AsArray();
	for (const auto &request : base_requests) {
		std::string str = request.AsDict().at("type").AsString();
		if (str == "Stop") {
			std::string stop_name = request.AsDict().at("name").AsString();
			double lng = request.AsDict().at("longitude").AsDouble();
			double lat = request.AsDict().at("latitude").AsDouble();
			db.AddStop( { stop_name, { lat, lng } });
			json::Dict stop_distances = request.AsDict().at("road_distances").AsDict();
		}
	}
	for (const auto &request : base_requests) {
		std::string str = request.AsDict().at("type").AsString();
		if (str == "Stop") {
			std::string stop_name = request.AsDict().at("name").AsString();
			json::Dict stop_distances = request.AsDict().at("road_distances").AsDict();
			if (!stop_distances.empty()) {
				for (const auto &stop_distance : stop_distances) {
					std::string name_dist_to = stop_distance.first;
					double dist = stop_distance.second.AsDouble();
					std::string_view first = db.FindStop(stop_name)->stop_name;
					std::string_view second = db.FindStop(name_dist_to)->stop_name;
					std::pair<std::string_view, std::string_view> pair_to_add = std::make_pair(first, second);
					db.AddDistance(pair_to_add, dist);
				}
			}
		}
	}
}

void JsonReader::AddBusesToCatalogue(Transport::Base::TransportCatalogue &db) {
	json::Array base_requests = document_.GetRoot().AsDict().at("base_requests").AsArray();
	for (const auto &request : base_requests) {
		std::string str = request.AsDict().at("type").AsString();
		if (str == "Bus") {
			Transport::Base::Bus bus;
			bus.bus_name = request.AsDict().at("name").AsString();
			bus.is_circle = request.AsDict().at("is_roundtrip").AsBool();
			json::Array stops = request.AsDict().at("stops").AsArray();
			for (const json::Node &stop_name : stops) {
				bus.stops.push_back(db.FindStop(stop_name.AsString()));
			}
			if (!bus.is_circle) {
				bus.stops.reserve(bus.stops.size() * 2 - 1);
				std::copy(bus.stops.rbegin() + 1, bus.stops.rend(), std::back_inserter(bus.stops));
			}
			db.AddBus(bus);
		}
	}
}

void JsonReader::AddRequestsToHandler(Transport::Base::Statistics::RequestHandler &request_handler) {
	std::vector<Transport::Base::Request> requests;
	json::Array stat_requests = document_.GetRoot().AsDict().at("stat_requests").AsArray();
	for (const auto &request : stat_requests) {
		int id = request.AsDict().at("id").AsInt();
		std::string type = request.AsDict().at("type").AsString();
		if (type == "Map"s) {
			requests.push_back( { id, type, ""s, ""s, ""s });
		} else if (type == "Route"s) {
			std::string from = request.AsDict().at("from").AsString();
			std::string to = request.AsDict().at("to").AsString();
			requests.push_back( { id, type, ""s, from, to });
		} else {
			std::string name = request.AsDict().at("name").AsString();
			requests.push_back( { id, type, name, ""s, ""s });
		}
	}
	request_handler.AddRequests(requests);
}

void JsonReader::AddRouterSettings(Transport::Base::TransportRouter &tr) {
	json::Dict settings = document_.GetRoot().AsDict().at("routing_settings").AsDict();
	tr.AddRouterSettings( { settings.at("bus_velocity"s).AsDouble(), settings.at("bus_wait_time"s).AsInt() });
}

svg::Color MakeColor(const json::Node &node) {
	if (node.IsArray()) {
		json::Array color = node.AsArray();
		int red = color.at(0).AsInt();
		int green = color.at(1).AsInt();
		int blue = color.at(2).AsInt();
		if (color.size() == 3) {
			return svg::Color(svg::Rgb(red, green, blue));
		} else {
			return svg::Color(svg::Rgba(red, green, blue, color.at(3).AsDouble()));
		}
	} else {
		return svg::Color(svg::Color(node.AsString()));
	}
}

void JsonReader::AddRenderSettingsToRenderer(Transport::Renderer::MapRenderer &map_renderer) {
	json::Dict render_settings = document_.GetRoot().AsDict().at("render_settings").AsDict();
	Transport::Renderer::MapRenderer::RenderSettings settings;
	settings.width = render_settings.at("width"s).AsDouble();
	settings.height = render_settings.at("height"s).AsDouble();
	settings.padding = render_settings.at("padding"s).AsDouble();
	settings.stop_radius = render_settings.at("stop_radius"s).AsDouble();
	settings.line_width = render_settings.at("line_width").AsDouble();
	settings.bus_label_font_size = render_settings.at("bus_label_font_size").AsInt();
	settings.bus_label_offset_x = render_settings.at("bus_label_offset").AsArray().at(0).AsDouble();
	settings.bus_label_offset_y = render_settings.at("bus_label_offset").AsArray().at(1).AsDouble();
	settings.stop_label_font_size = render_settings.at("stop_label_font_size").AsInt();
	settings.stop_label_offset_x = render_settings.at("stop_label_offset").AsArray().at(0).AsDouble();
	settings.stop_label_offset_y = render_settings.at("stop_label_offset").AsArray().at(1).AsDouble();
	settings.underlayer_color = MakeColor(render_settings.at("underlayer_color"s));
	settings.underlayer_width = render_settings.at("underlayer_width"s).AsDouble();
	json::Array color_palette = render_settings.at("color_palette"s).AsArray();
	for (const json::Node &node : color_palette) {
		svg::Color color = MakeColor(node);
		settings.color_palette.push_back(color);
	}
	map_renderer.SetSettings(settings);
}

Serialization::SerializationSettings JsonReader::GetSerializationSettings() const {
	json::Dict serialization_settings = document_.GetRoot().AsDict().at("serialization_settings").AsDict();
	return {serialization_settings.at("file").AsString()};
}

}
