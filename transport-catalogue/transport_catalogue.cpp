#include "transport_catalogue.h"
#include <vector>
#include <algorithm>
#include <numeric>
#include <functional>
#include <set>
#include <utility>

#include "input_reader.h"
#include "stat_reader.h"

void Transport::Base::TransportCatalogue::AddStop(Transport::Detail::Read::Input &in) {
	for (auto& [name, coord] : in.stops_and_coord) {
		stops_.push_back( { std::move(name), std::move(coord) });
		p_stops_[stops_.back().stop_name] = &stops_.back();
	}
	for (auto& [names, dist] : in.distances) {
		std::string_view first = p_stops_.at(names.first)->stop_name;
		std::string_view second = p_stops_.at(names.second)->stop_name;
		length_[ { std::move(first), std::move(second) }] = dist;
	}
}

void Transport::Base::TransportCatalogue::AddBus(Transport::Detail::Read::Input &in) {
	for (Transport::Detail::Read::Input::Bus &bus : in.buses) {
		std::vector<Stop*> p_stops;
		for (const std::string &stops : bus.stops) {
			p_stops.push_back(p_stops_.at(stops));
		}
		buses_.push_back( { std::move(bus.bus_name), std::move(p_stops) });
		p_buses_[buses_.back().bus_name] = &buses_.back();
		for (const auto &stop : p_buses_.at(buses_.back().bus_name)->stops) {
			p_buses_per_stop_[stop->stop_name].insert(buses_.back().bus_name);
		}
	}
}

Transport::Detail::Statistic::Bus Transport::Base::TransportCatalogue::GetBusInfo(const std::string &bus_name) {
	Transport::Detail::Statistic::Bus out(bus_name);
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
		out.length_geo += ComputeDistance(p_buses_.at(bus_name)->stops[i]->coordinates, p_buses_.at(bus_name)->stops[i + 1]->coordinates);
		std::string_view first = p_buses_.at(bus_name)->stops[i]->stop_name;
		std::string_view second = p_buses_.at(bus_name)->stops[i + 1]->stop_name;
		std::pair<std::string_view, std::string_view> pair_to_find = std::make_pair(std::move(first), std::move(second));
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

Transport::Detail::Statistic::Stop Transport::Base::TransportCatalogue::GetStopInfo(const std::string &stop_name) {
	Transport::Detail::Statistic::Stop out(stop_name);
	if (p_stops_.count(stop_name) == 0) {
		return out;
	} else {
		out.p_buses_per_stop = &p_buses_per_stop_;
		return out;
	}
}
