#include "stat_reader.h"

using namespace Transport::Detail::Statistic;
std::ostream& operator<<(std::ostream &os, const Bus &stat) {
	if (stat.stops == 0) {
		os << "Bus " << stat.bus_name << ": not found";
		return os;
	}
	os << "Bus " << stat.bus_name << ": " << stat.stops << " stops on route, "
			<< stat.unique_stops << " unique stops, " << stat.length_input
			<< " route length, " << stat.curvativity << " curvature";
	return os;
}

std::ostream& operator<<(std::ostream &os, const Stop &stat) {
	if (stat.p_buses_per_stop == nullptr) {
		os << "Stop " << stat.stop_name << ": not found";
		return os;
	}
	if (stat.p_buses_per_stop->count(stat.stop_name) == 0) {
		os << "Stop " << stat.stop_name << ": no buses";
		return os;
	}
	bool is_first = true;
	os << "Stop " << stat.stop_name << ": buses ";
	for (const auto &stop : stat.p_buses_per_stop->at(stat.stop_name)) {
		if (is_first) {
			os << stop;
			is_first = false;
		} else {
			os << " " << stop;
		}
	}
	return os;
}
