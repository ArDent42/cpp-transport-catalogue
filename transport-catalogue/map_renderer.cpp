#include "map_renderer.h"
#include "svg.h"

namespace Transport::Renderer {
void MapRenderer::SetSettings(const RenderSettings &settings) {
	render_settings_ = settings;
}

SphereProjector MapRenderer::CreateProjector() {
	std::vector<Coordinates> coordinates;
	for (const auto& [bus_name, bus] : buses_) {
		for (const Transport::Base::Stop *stop : bus->stops) {
			coordinates.push_back(stop->coordinates);
		}
	}
	SphereProjector projector(coordinates.begin(), coordinates.end(),
			render_settings_.width, render_settings_.height,
			render_settings_.padding);
	return projector;
}

void MapRenderer::CreateLines(svg::Document &doc) const {

	std::vector<svg::Polyline> lines;
	auto it = render_settings_.color_palette.begin();
	for (const auto& [bus_name, bus] : buses_) {
		if (bus->stops.empty()) {
			continue;
		}
		lines.push_back(svg::Polyline { });
		for (const Transport::Base::Stop *stop : bus->stops) {
			lines.back().AddPoint(projector_(stop->coordinates));
		}
		svg::Color color = *it;
		if (++it == render_settings_.color_palette.end()) {
			it = render_settings_.color_palette.begin();
		}
		lines.back().SetFillColor(svg::NoneColor);
		lines.back().SetStrokeColor(color);
		lines.back().SetStrokeWidth(render_settings_.line_width);
		lines.back().SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);
		lines.back().SetStrokeLineCap(svg::StrokeLineCap::ROUND);
		doc.Add(lines.back());
	}
}

void MapRenderer::CreateBusText(svg::Document &doc, const svg::Point &position,
		const std::string_view &bus_name, const svg::Color &color) const {
	svg::Text underlayer_text;
	svg::Text text;
	text.SetPosition(position);
	text.SetOffset(
			svg::Point(render_settings_.bus_label_offset_x,
					render_settings_.bus_label_offset_y));
	text.SetFontSize(render_settings_.bus_label_font_size);
	text.SetFontFamily("Verdana"s);
	text.SetFontWeight("bold"s);
	text.SetData(static_cast<std::string>(bus_name));
	underlayer_text = text;
	text.SetFillColor(color);
	underlayer_text.SetFillColor(render_settings_.underlayer_color);
	underlayer_text.SetStrokeColor(render_settings_.underlayer_color);
	underlayer_text.SetStrokeWidth(render_settings_.underlayer_width);
	underlayer_text.SetStrokeLineCap(svg::StrokeLineCap::ROUND);
	underlayer_text.SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);
	doc.Add(underlayer_text);
	doc.Add(text);
}

void MapRenderer::CreateStopText(svg::Document &doc, const svg::Point &position,
		const std::string_view &stop_name) const {
	svg::Text underlayer_text;
	svg::Text text;
	text.SetPosition(position);
	text.SetOffset(
			svg::Point(render_settings_.stop_label_offset_x,
					render_settings_.stop_label_offset_y));
	text.SetFontSize(render_settings_.stop_label_font_size);
	text.SetFontFamily("Verdana"s);
	text.SetData(static_cast<std::string>(stop_name));
	underlayer_text = text;
	text.SetFillColor("black"s);
	underlayer_text.SetFillColor(render_settings_.underlayer_color);
	underlayer_text.SetStrokeColor(render_settings_.underlayer_color);
	underlayer_text.SetStrokeWidth(render_settings_.underlayer_width);
	underlayer_text.SetStrokeLineCap(svg::StrokeLineCap::ROUND);
	underlayer_text.SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);
	doc.Add(underlayer_text);
	doc.Add(text);
}

void MapRenderer::CreateBusNames(svg::Document &doc) const {
	auto it = render_settings_.color_palette.begin();
	for (const auto& [bus_name, bus] : buses_) {
		if (bus->stops.empty()) {
			continue;
		}
		svg::Color color = *it;
		if (++it == render_settings_.color_palette.end()) {
			it = render_settings_.color_palette.begin();
		}
		if (bus->is_circle
				|| bus->stops.front()->stop_name
						== bus->stops[bus->stops.size() / 2]->stop_name) {
			svg::Point position = projector_(bus->stops.front()->coordinates);
			CreateBusText(doc, position, bus_name, color);
		} else {
			svg::Point position_begin = projector_(
					bus->stops.front()->coordinates);
			svg::Point position_end = projector_(
					bus->stops[bus->stops.size() / 2]->coordinates);
			CreateBusText(doc, position_begin, bus_name, color);
			CreateBusText(doc, position_end, bus_name, color);
		}
	}
}

void MapRenderer::CreateStops(svg::Document &doc) const {
	std::map<std::string_view, Transport::Base::Stop*> stops;
	for (const auto& [bus_name, bus] : buses_) {
		for (Transport::Base::Stop *stop : bus->stops) {
			stops[stop->stop_name] = stop;
		}
	}
	for (const auto& [stop_name, stop] : stops) {
		svg::Point position = projector_(stop->coordinates);
		svg::Circle circle;
		circle.SetCenter(position).SetRadius(render_settings_.stop_radius).SetFillColor(
				"white"s);
		doc.Add(circle);
	}
}

void MapRenderer::CreateStopNames(svg::Document &doc) const {
	std::map<std::string_view, Transport::Base::Stop*> stops;
	for (const auto& [bus_name, bus] : buses_) {
		for (Transport::Base::Stop *stop : bus->stops) {
			stops[stop->stop_name] = stop;
		}
	}
	for (const auto& [stop_name, stop] : stops) {
		svg::Point position = projector_(stop->coordinates);
		CreateStopText(doc, position, stop_name);
	}
}

svg::Document MapRenderer::Render(
		const Transport::Base::TransportCatalogue &db_) {
	svg::Document doc;
	buses_ = db_.GetBuses();
	projector_ = CreateProjector();
	CreateLines(doc);
	CreateBusNames(doc);
	CreateStops(doc);
	CreateStopNames(doc);
	return doc;
}

}
