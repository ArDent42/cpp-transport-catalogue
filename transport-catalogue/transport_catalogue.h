#pragma once
#include <string>
#include <vector>
#include <deque>
#include <unordered_map>
#include <set>
#include <string_view>
#include "geo.h"
#include "service.h"
#include "stat_reader.h"

namespace Transport::Base {

struct Stop {
	std::string stop_name;
	Coordinates coordinates;
	std::unordered_map<std::string, double> distance_to_another;
};

struct Bus {
	std::string bus_name;
	std::vector<Stop*> stops;
};

class TransportCatalogue {
private:
	std::deque<Stop> stops_;
	std::unordered_map<std::string_view, Stop*> p_stops_;
	std::deque<Bus> buses_;
	std::unordered_map<std::string_view, Bus*> p_buses_;
	std::unordered_map<std::string_view, std::set<std::string_view>> p_buses_per_stop_;
	std::unordered_map<std::pair<std::string_view, std::string_view>, double,
			Hasher> length_;
public:
	void AddStop(const Stop& stop);
	void AddBus(const Bus& bus);
	void AddDistances(); //с добавлением дистанций ничего более толкого не придумал, мне здесь не нравится наличие этого метода и бесполезное поле в структуре Stop
	Stop* FindStop(const std::string_view &stop_name) {
		return p_stops_.at(stop_name);
	}
	Bus* FindBus(const std::string_view &bus_name) {
		return p_buses_.at(bus_name);
	}
	Transport::Detail::Statistics::BusStat GetBusInfo(const std::string &bus_name);
	Transport::Detail::Statistics::StopStat GetStopInfo(const std::string &stop_name);
};

}
