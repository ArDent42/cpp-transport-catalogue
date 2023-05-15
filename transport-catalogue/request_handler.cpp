#include <unordered_map>
#include <optional>
#include <sstream>
#include "transport_catalogue.h"
#include "json_reader.h"
#include "geo.h"
#include "domain.h"
#include "request_handler.h"
#include "json_builder.h"
#include "transport_router.h"
#include "router.h"
using namespace std::literals;

namespace Transport::Base::Statistics {

RequestHandler::RequestHandler(const Transport::Base::TransportCatalogue &db, Transport::Renderer::MapRenderer &mr, TransportRouter &tr) :
		db_(db), mr_(mr), tr_(tr) {
}

std::optional<Transport::Detail::Statistics::BusStat> RequestHandler::GetBusStat(const std::string_view &bus_name) const {
	Transport::Detail::Statistics::BusStat out(bus_name);
	if (db_.FindBus(bus_name) == nullptr) {
		return {};
	}
	int number_of_stops = db_.FindBus(bus_name)->stops.size();
	out.stops = number_of_stops;
	std::set<Transport::Base::Stop*> unique_stops;
	std::vector<Transport::Base::Stop*> stops = db_.FindBus(bus_name)->stops;
	for (const auto &v : stops) {
		unique_stops.insert(v);
	}
	out.unique_stops = unique_stops.size();
	for (int i = 0; i < number_of_stops - 1; ++i) {
		out.length_geo += ComputeDistance(stops[i]->coordinates, stops[i + 1]->coordinates);
		std::string_view first = stops[i]->stop_name;
		std::string_view second = stops[i + 1]->stop_name;
		std::pair<std::string_view, std::string_view> pair_to_find = std::make_pair(std::move(first), std::move(second));
		double length = db_.GetLength(pair_to_find);
		out.length_input += length;
		out.curvativity = out.length_input / out.length_geo;
	}
	return out;
}

// Возвращает маршруты, проходящие через
std::optional<Transport::Detail::Statistics::StopStat> RequestHandler::GetBusesByStop(const std::string_view &stop_name) const {
	Transport::Detail::Statistics::StopStat out(stop_name);
	if (db_.FindStop(stop_name) == nullptr) {
		return {};
	} else {
		out.buses_per_stop = db_.GetBusesPerStop(stop_name);
		return out;
	}
}

std::optional<Transport::Detail::Statistics::RouteStat> RequestHandler::GetRouteStat(std::string_view from, std::string_view to, const graph::Router<double>& router) const {
	Transport::Detail::Statistics::RouteStat result = tr_.BuildRoute(from, to, router);
	if (result.edges.empty()) {
		return {};
	}
	return result;
}

void RequestHandler::AddRequests(const std::vector<Transport::Base::Request> &requests) {
	requests_ = requests;
}

void RequestHandler::MakeStopStatOutput(json::Array &stats, const Transport::Base::Request &request) const {
	std::optional<Transport::Detail::Statistics::StopStat> stop_stat = GetBusesByStop(request.name);
	json::Array buses;
	if (stop_stat) {
		for (const std::string_view &bus : stop_stat.value().buses_per_stop) {
			buses.emplace_back(static_cast<std::string>(bus));
		}
		stats.emplace_back(json::Builder { }
		.StartDict()
				.Key("request_id"s).Value(request.id)
				.Key("buses"s).Value(buses)
				.EndDict().Build());
	} else {
		stats.emplace_back(json::Builder { }.StartDict().Key("request_id"s).Value(request.id).Key("error_message"s).Value("not found"s).EndDict().Build());
	}
}

void RequestHandler::MakeBusStatOutput(json::Array &stats, const Transport::Base::Request &request) const {
	std::optional<Transport::Detail::Statistics::BusStat> bus_stat = GetBusStat(request.name);
	if (bus_stat) {
		stats.emplace_back(json::Builder { }
		.StartDict()
				.Key("request_id"s).Value(request.id)
				.Key("curvature"s).Value(bus_stat.value().curvativity)
				.Key("route_length"s).Value(bus_stat.value().length_input)
				.Key("stop_count"s).Value(bus_stat.value().stops)
				.Key("unique_stop_count"s).Value(bus_stat.value().unique_stops)
				.EndDict().Build());
	} else {
		stats.emplace_back(json::Builder { }.StartDict().Key("request_id"s).Value(request.id).Key("error_message"s).Value("not found"s).EndDict().Build());
	}
}

void RequestHandler::MakeMapRendererOutput(json::Array &stats, const Transport::Base::Request &request) const {
	svg::Document doc = RenderMap();
	std::ostringstream str;
	doc.Render(str);
	stats.emplace_back(json::Builder { }.StartDict().Key("request_id"s).Value(request.id).Key("map"s).Value(str.str()).EndDict().Build());
}

void RequestHandler::MakeRouteStatOutput(json::Array &stats, const Transport::Base::Request &request, const
		graph::Router<double>& router) const {
	std::optional<Transport::Detail::Statistics::RouteStat> route_stat = GetRouteStat(request.from, request.to, router);
	if (request.from == request.to) {
						json::Array items;
						stats.emplace_back(json::Builder{}
								.StartDict()
										.Key("items"s).Value(json::Array{})
										.Key("request_id"s).Value(request.id)
										.Key("total_time"s).Value(0.0)
										.EndDict().Build());
						return;
					}
	if (route_stat) {
		json::Array items;
		for (const Transport::Base::RouteEdge& edge : route_stat.value().edges) {
			if (edge.span_count == 0) {
				items.emplace_back(json::Builder{}
				.StartDict()
						.Key("stop_name"s).Value(static_cast<std::string>(edge.name))
						.Key("time"s).Value(edge.weight)
						.Key("type").Value("Wait"s)
						.EndDict().Build());
			} else {
				items.emplace_back(json::Builder{}
				.StartDict()
						.Key("bus"s).Value(static_cast<std::string>(edge.name))
						.Key("span_count"s).Value(static_cast<int>(edge.span_count))
						.Key("time"s).Value(edge.weight)
						.Key("type").Value("Bus"s)
						.EndDict().Build());
			}
		}
		stats.emplace_back(json::Builder{}
		.StartDict()
				.Key("items"s).Value(items)
				.Key("request_id"s).Value(request.id)
				.Key("total_time"s).Value(route_stat.value().total_time)
				.EndDict().Build());
	} else {
		stats.emplace_back(json::Builder { }
		.StartDict()
				.Key("error_message"s).Value("not found"s)
				.Key("request_id"s).Value(request.id)
				.EndDict().Build());
	}

}

void RequestHandler::PrintStats(std::ostream &out) const {
	json::Array results = json::Array { };
	tr_.InitializeRouter(db_);
	graph::Router<double> router(tr_.GetGraph());
	for (const Transport::Base::Request &request : requests_) {
		if (request.type == "Stop") {
			MakeStopStatOutput(results, request);
		} else if (request.type == "Bus") {
			MakeBusStatOutput(results, request);
		} else if (request.type == "Map") {
			MakeMapRendererOutput(results, request);
		} else if (request.type == "Route") {
			MakeRouteStatOutput(results, request, router);
		}
	}
	json::Document doc { results };
	json::Print(doc, out);
}

}
