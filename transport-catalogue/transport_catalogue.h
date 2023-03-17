#pragma once
#include <string>
#include <vector>
#include <deque>
#include <unordered_map>
#include <set>
#include <string_view>
#include "geo.h"
#include "input_reader.h"
#include "stat_reader.h"
#include "service.h"

namespace Transport::Base {

class TransportCatalogue {
private:
	struct Stop {
		std::string stop_name;
		Coordinates coordinates;
	};

	struct Bus {
		std::string bus_name;
		std::vector<Stop*> stops;
	};
	std::deque<Stop> stops_;
	std::unordered_map<std::string_view, Stop*> p_stops_;
	std::deque<Bus> buses_;
	std::unordered_map<std::string_view, Bus*> p_buses_;
	std::unordered_map<std::string_view, std::set<std::string_view>> p_buses_per_stop_;
	std::unordered_map<std::pair<std::string_view, std::string_view>, double,
			Transport::Detail::Hasher> length_;
public:
	void AddStop(Transport::Detail::Read::Input &in);
	void AddBus(Transport::Detail::Read::Input &in);
	Stop* FindStop(const std::string_view &stop_name) {
		return p_stops_.at(stop_name);
	}
	Bus* FindBus(const std::string_view &bus_name) {
		return p_buses_.at(bus_name);
	}
	Transport::Detail::Statistic::Bus GetBusInfo(const std::string &bus_name);
	Transport::Detail::Statistic::Stop GetStopInfo(const std::string &stop_name);
};

}
