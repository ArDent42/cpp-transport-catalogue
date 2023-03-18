#include "stat_reader.h"

std::ostream& Transport::Detail::Statistics::operator<<(std::ostream &os, const Transport::Detail::Statistics::BusStat &stat) {
	if (stat.stops == 0) {
		os << "Bus " << stat.bus_name << ": not found";
		return os;
	}
	os << "Bus " << stat.bus_name << ": " << stat.stops << " stops on route, "
			<< stat.unique_stops << " unique stops, " << stat.length_input
			<< " route length, " << stat.curvativity << " curvature";
	return os;
}

std::ostream& Transport::Detail::Statistics::operator<<(std::ostream &os, const Transport::Detail::Statistics::StopStat &stat) {
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
