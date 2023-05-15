#include "transport_catalogue.h"
#include <vector>
#include <algorithm>
#include <numeric>
#include <functional>
#include <set>
#include <utility>

namespace Transport::Base {

void TransportCatalogue::AddStop(const Transport::Base::Stop &stop) {
	stops_.push_back(stop);
	p_stops_[stops_.back().stop_name] = &stops_.back();
}
size_t TransportCatalogue::GetStopsCount() const {
	return stops_.size();
}

void TransportCatalogue::AddBus(const Transport::Base::Bus &bus) {
	Transport::Base::Bus new_bus { bus.bus_name, { }, { } };
	std::vector<Stop*> stops_temp = bus.stops;
	for (const Stop *stop : bus.stops) {
		new_bus.stops.push_back(p_stops_.at(stop->stop_name));
	}
	new_bus.is_circle = bus.is_circle;
	buses_.push_back(new_bus);
	p_buses_[buses_.back().bus_name] = &buses_.back();
	for (const auto &stop : p_buses_.at(buses_.back().bus_name)->stops) {
		p_buses_per_stop_[stop->stop_name].insert(buses_.back().bus_name);
	}
}

void TransportCatalogue::AddDistance(std::pair<std::string_view, std::string_view> stops_pair, double distance) {
	distances_[stops_pair] = distance;
}

Stop* TransportCatalogue::FindStop(const std::string_view &stop_name) const {
	if (p_stops_.count(stop_name)) {
		return p_stops_.at(stop_name);
	}
	return nullptr;
}
Bus* TransportCatalogue::FindBus(const std::string_view &bus_name) const {
	if (p_buses_.count(bus_name)) {
		return p_buses_.at(bus_name);
	}
	return nullptr;
}

std::map<std::string_view, Bus*> TransportCatalogue::GetBuses() const {
	std::map<std::string_view, Bus*> buses;
		for (const auto& [bus_name, bus] : p_buses_) {
			buses[bus_name] = bus;
		}
		return buses;
}

std::unordered_map<std::string_view, Stop*> TransportCatalogue::GetStops() const {
	return p_stops_;
}

double TransportCatalogue::GetLength(std::pair<std::string_view, std::string_view> pair_of_stops) const {
	auto it = distances_.find(pair_of_stops);
	if (it != distances_.end()) {
		return it->second;
	}
	std::swap(pair_of_stops.first, pair_of_stops.second);
	it = distances_.find(pair_of_stops);
	if (it != distances_.end()) {
		return it->second;
	}
	return 0.0;
}
std::set<std::string_view> TransportCatalogue::GetBusesPerStop(const std::string_view &stop_name) const {
	if (p_buses_per_stop_.count(stop_name) == 0) {
		return {};
	}
	return p_buses_per_stop_.at(stop_name);
}

}
