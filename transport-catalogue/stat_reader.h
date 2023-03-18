#pragma once
#include <string_view>
#include <iostream>
#include <unordered_map>
#include <set>

namespace Transport::Detail::Statistics {

struct BusStat {
	std::string bus_name;
	int unique_stops = 0;
	int stops = 0;
	double length_geo = 0;
	double length_input = 0;
	double curvativity = 0;
	BusStat(const std::string& bus) : bus_name(bus) {}
};

struct StopStat {
	std::string stop_name;
	std::unordered_map<std::string_view, std::set<std::string_view>> *p_buses_per_stop;
	StopStat(const std::string& stop) : stop_name(stop), p_buses_per_stop(nullptr){}
};


std::ostream& operator<<(std::ostream& os, const BusStat& stat);
std::ostream& operator<<(std::ostream& os, const StopStat& stat);

}
