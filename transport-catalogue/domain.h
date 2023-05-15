#pragma once
#include <string>
#include <vector>
#include "geo.h"
#include <set>

namespace Transport::Base {

struct Stop {
	std::string stop_name;
	Coordinates coordinates;
};

struct Bus {
	std::string bus_name;
	std::vector<Stop*> stops;
	bool is_circle;
};

struct Request {
	int id;
	std::string type;
	std::string name;
	std::string from;
	std::string to;
};

struct RoutingSettings {
	double bus_velocity;
	int waiting_time;
};

struct RouteEdge {
	std::string_view name;
	std::string_view from;
	std::string_view to;
	double weight = 0.0;
	size_t span_count = 0;
};



}

namespace Transport::Detail::Statistics {

struct BusStat {
	std::string_view bus_name;
	int unique_stops = 0;
	int stops = 0;
	double length_geo = 0;
	double length_input = 0;
	double curvativity = 0;
	BusStat(const std::string_view &bus) :
			bus_name(bus) {
	}
	BusStat() = default;
};

struct StopStat {
	std::string_view stop_name;
	std::set<std::string_view> buses_per_stop;
	StopStat(const std::string_view &stop) :
			stop_name(stop) {
	}
	StopStat() = default;
};

struct RouteStat {
	std::vector<Transport::Base::RouteEdge> edges;
	double total_time = 0.0;
};

}

struct Hasher {
	size_t operator()(const std::pair<std::string_view, std::string_view> &lenght) const {
		return (std::hash<std::string_view> { }(lenght.first) + std::hash<std::string_view> { }(lenght.second));
	}
};
