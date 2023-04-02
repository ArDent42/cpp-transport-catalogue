#include <unordered_map>
#include <optional>
#include <sstream>
#include "transport_catalogue.h"
#include "json_reader.h"
#include "geo.h"
#include "domain.h"
#include "request_handler.h"
using namespace std::literals;

namespace Transport::Base::Statistics {

RequestHandler::RequestHandler(const Transport::Base::TransportCatalogue &db,
		Transport::Renderer::MapRenderer &mr) :
		db_(db), mr_(mr) {
}

std::optional<Transport::Detail::Statistics::BusStat> RequestHandler::GetBusStat(
		const std::string_view &bus_name) const {
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
		out.length_geo += ComputeDistance(stops[i]->coordinates,
				stops[i + 1]->coordinates);
		std::string_view first = stops[i]->stop_name;
		std::string_view second = stops[i + 1]->stop_name;
		std::pair<std::string_view, std::string_view> pair_to_find =
				std::make_pair(std::move(first), std::move(second));
		double length = db_.GetLength(pair_to_find);
		out.length_input += length;
		out.curvativity = out.length_input / out.length_geo;
	}
	return out;
}

// Возвращает маршруты, проходящие через
std::optional<Transport::Detail::Statistics::StopStat> RequestHandler::GetBusesByStop(
		const std::string_view &stop_name) const {
	Transport::Detail::Statistics::StopStat out(stop_name);
	if (db_.FindStop(stop_name) == nullptr) {
		return {};
	} else {
		out.buses_per_stop = db_.GetBusesPerStop(stop_name);
		return out;
	}
}
void RequestHandler::AddRequests(
		const std::vector<Transport::Base::Request> &requests) {
	requests_ = requests;
}

void RequestHandler::MakeStopStatOutput(
		json::Array &stats, const Transport::Base::Request &request) const {
	std::optional<Transport::Detail::Statistics::StopStat> stop_stat =
			GetBusesByStop(request.name);
	json::Array buses;
	if (stop_stat) {
		for (const std::string_view &bus : stop_stat.value().buses_per_stop) {
			buses.emplace_back(static_cast<std::string>(bus));
		}
		stats.emplace_back(json::Dict { { "request_id"s, request.id }, {
				"buses"s, buses }, });
	} else {
		stats.emplace_back(json::Dict { { "request_id"s, request.id }, {
				"error_message"s, "not found"s }, });
	}
}

void RequestHandler::MakeBusStatOutput(
		json::Array &stats, const Transport::Base::Request &request) const {
	std::optional<Transport::Detail::Statistics::BusStat> bus_stat = GetBusStat(
			request.name);
	if (bus_stat) {
		stats.emplace_back(json::Dict { { "request_id"s, request.id }, {
				"curvature"s, bus_stat.value().curvativity }, { "route_length"s,
				bus_stat.value().length_input }, { "stop_count"s,
				bus_stat.value().stops }, { "unique_stop_count"s,
				bus_stat.value().unique_stops } });
	} else {
		stats.emplace_back(json::Dict { { "request_id"s, request.id }, {
				"error_message"s, "not found"s }, });
	}
}

void RequestHandler::MakeMapRendererOutput(
		json::Array &stats, const Transport::Base::Request &request) const {
		svg::Document doc = RenderMap();
		std::ostringstream str;
		doc.Render(str);
		stats.emplace_back(json::Dict { { "request_id"s, request.id }, {"map"s, str.str()} });
}

void RequestHandler::PrintStats(std::ostream &out) const {
	json::Array stats;
	for (const Transport::Base::Request &request : requests_) {
		if (request.type == "Stop") {
			MakeStopStatOutput(stats, request);
		} else if (request.type == "Bus") {
			MakeBusStatOutput(stats, request);
		} else {
			MakeMapRendererOutput(stats,request);
		}
	}
	json::Document doc { stats };
	json::Print(doc, out);
}

}
