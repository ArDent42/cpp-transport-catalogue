#pragma once
#include <string>
#include <vector>
#include <deque>
#include <unordered_map>
#include <set>
#include <map>
#include <string_view>
#include "domain.h"

namespace Transport::Base {

class TransportCatalogue {
private:
	std::deque<Stop> stops_;
	std::unordered_map<std::string_view, Stop*> p_stops_;
	std::deque<Bus> buses_;
	std::unordered_map<std::string_view, Bus*> p_buses_;
	std::unordered_map<std::string_view, std::set<std::string_view>> p_buses_per_stop_;
	std::unordered_map<std::pair<std::string_view, std::string_view>, double, Hasher> distances_;
public:
	void AddStop(const Stop &stop);
	size_t GetStopsCount() const;
	void AddBus(const Bus &bus);
	void AddDistance(std::pair<std::string_view, std::string_view> stops_pair, double distance);
	Stop* FindStop(const std::string_view &stop_name) const;
	Bus* FindBus(const std::string_view &bus_name) const;
	std::map<std::string_view, Bus*> GetBuses() const;
	std::unordered_map<std::string_view, Stop*> GetStops() const;
	std::unordered_map<std::pair<std::string_view, std::string_view>, double, Hasher> GetDistances() const {
		return distances_;
	}
	double GetLength(std::pair<std::string_view, std::string_view> pair_of_stops) const;
	std::set<std::string_view> GetBusesPerStop(const std::string_view &stop_name) const;
};

}
