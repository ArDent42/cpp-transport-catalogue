#include "transport_catalogue.h"
#include <vector>
#include <algorithm>
#include <numeric>
#include <functional>
#include <set>
#include <utility>
#include "input_reader.h"
#include "stat_reader.h"

void Transport::Base::TransportCatalogue::AddStop(const Transport::Base::Stop &stop) {
	stops_.push_back(stop);
	p_stops_[stops_.back().stop_name] = &stops_.back();
}

void Transport::Base::TransportCatalogue::AddBus(const Transport::Base::Bus &bus) {
	Transport::Base::Bus new_bus { bus.bus_name, { } };
	std::vector<Stop*> stops_temp = bus.stops;
	for (const Stop *stop : bus.stops) {
		new_bus.stops.push_back(p_stops_.at(stop->stop_name));
	}
	buses_.push_back(new_bus);
	p_buses_[buses_.back().bus_name] = &buses_.back();
	for (const auto &stop : p_buses_.at(buses_.back().bus_name)->stops) {
		p_buses_per_stop_[stop->stop_name].insert(buses_.back().bus_name);
	}
}

Transport::Detail::Statistics::BusStat Transport::Base::TransportCatalogue::GetBusInfo(
		const std::string &bus_name) {
	Transport::Detail::Statistics::BusStat out(bus_name);
	if (p_buses_.count(bus_name) == 0) {
		return out;
	}
	out.stops = p_buses_.at(bus_name)->stops.size();
	std::set<Stop*> unique_stops;
	for (auto &v : p_buses_.at(bus_name)->stops) {
		unique_stops.insert(v);
	}
	out.unique_stops = unique_stops.size();
	for (size_t i = 0; i < p_buses_.at(bus_name)->stops.size() - 1; ++i) {
		out.length_geo += ComputeDistance(
				p_buses_.at(bus_name)->stops[i]->coordinates,
				p_buses_.at(bus_name)->stops[i + 1]->coordinates);
		std::string_view first = p_buses_.at(bus_name)->stops[i]->stop_name;
		std::string_view second = p_buses_.at(bus_name)->stops[i + 1]->stop_name;
		std::pair<std::string_view, std::string_view> pair_to_find =
				std::make_pair(std::move(first), std::move(second));
		auto it = length_.find(pair_to_find);
		if (it != length_.end()) {
			out.length_input += it->second;
		} else {
			std::swap(pair_to_find.first, pair_to_find.second);
			it = length_.find(pair_to_find);
			out.length_input += it->second;
		}
		out.curvativity = out.length_input / out.length_geo;
	}
	return out;
}

Transport::Detail::Statistics::StopStat Transport::Base::TransportCatalogue::GetStopInfo(
		const std::string &stop_name) {
	Transport::Detail::Statistics::StopStat out(stop_name);
	if (p_stops_.count(stop_name) == 0) {
		return out;
	} else {
		out.p_buses_per_stop = &p_buses_per_stop_;
		return out;
	}
}

void Transport::Base::TransportCatalogue::AddDistances() {
	for (auto& [stop_name, stop] : p_stops_) {
		for (auto& [stop_name_another, distance] : stop->distance_to_another) {
			std::string_view first = stop->stop_name;
			std::string_view second = p_stops_.at(stop_name_another)->stop_name;
			std::pair<std::string_view, std::string_view> pair_to_add =
					std::make_pair(first, second);
			length_[pair_to_add] = distance;
		}
		stop->distance_to_another.clear();
	}
}
