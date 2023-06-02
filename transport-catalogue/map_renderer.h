#pragma once
#include "geo.h"
#include "svg.h"
#include "domain.h"
#include "transport_catalogue.h"
//#include "request_handler.h"
#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <optional>
#include <vector>
#include <map>

namespace Transport::Renderer {
using namespace std::literals;

inline const double EPSILON = 1e-6;

class SphereProjector {
public:
	SphereProjector() = default;

	template<typename PointInputIt>
	SphereProjector(PointInputIt points_begin, PointInputIt points_end,
			double max_width, double max_height, double padding) :
			padding_(padding) {
		MakeSphereProjector(points_begin, points_end, max_width, max_height,
				padding);
	}
	template<typename PointInputIt>
	void MakeSphereProjector(PointInputIt points_begin, PointInputIt points_end,
			double max_width, double max_height, double padding) {
		if (points_begin == points_end) {
			return;
		}
		const auto [left_it, right_it] = std::minmax_element(points_begin,
				points_end, [](auto lhs, auto rhs) {
					return lhs.lng < rhs.lng;
				});
		min_lon_ = left_it->lng;
		const double max_lon = right_it->lng;
		const auto [bottom_it, top_it] = std::minmax_element(points_begin,
				points_end, [](auto lhs, auto rhs) {
					return lhs.lat < rhs.lat;
				});
		const double min_lat = bottom_it->lat;
		max_lat_ = top_it->lat;
		std::optional<double> width_zoom;
		if (!IsZero(max_lon - min_lon_)) {
			width_zoom = (max_width - 2 * padding) / (max_lon - min_lon_);
		}
		std::optional<double> height_zoom;
		if (!IsZero(max_lat_ - min_lat)) {
			height_zoom = (max_height - 2 * padding) / (max_lat_ - min_lat);
		}
		if (width_zoom && height_zoom) {
			zoom_coeff_ = std::min(*width_zoom, *height_zoom);
		} else if (width_zoom) {
			zoom_coeff_ = *width_zoom;
		} else if (height_zoom) {
			zoom_coeff_ = *height_zoom;
		}
	}
	svg::Point operator()(Coordinates coords) const {
		return {
			(coords.lng - min_lon_) * zoom_coeff_ + padding_,
			(max_lat_ - coords.lat) * zoom_coeff_ + padding_
		};
	}

private:
	bool IsZero(double value) {
		return std::abs(value) < EPSILON;
	}
	double padding_ = 0;
	double min_lon_ = 0;
	double max_lat_ = 0;
	double zoom_coeff_ = 0;
};

class MapRenderer {
public:
	struct RenderSettings {
		double width;
		double height;
		double padding;
		double line_width;
		double stop_radius;
		int bus_label_font_size;
		double bus_label_offset_x;
		double bus_label_offset_y;
		int stop_label_font_size;
		double stop_label_offset_x;
		double stop_label_offset_y;
		svg::Color underlayer_color;
		double underlayer_width;
		std::vector<svg::Color> color_palette;
	};
private:
	SphereProjector projector_;
	RenderSettings render_settings_;
	std::map<std::string_view, Transport::Base::Bus*> buses_;
public:
	void SetSettings(const RenderSettings &settings);
	SphereProjector CreateProjector();
	RenderSettings GetSettings() const;
	void CreateLines(svg::Document& doc) const;
	void CreateBusNames(svg::Document& doc) const;
	void CreateBusText(svg::Document& doc, const svg::Point &position,
			const std::string_view &bus_name, const svg::Color &color) const;
	void CreateStops(svg::Document& doc) const;
	void CreateStopNames(svg::Document& doc) const;
	void CreateStopText(svg::Document& doc, const svg::Point &position,
				const std::string_view &bus_name) const;
	svg::Document Render(const Transport::Base::TransportCatalogue &db_);
};

}
