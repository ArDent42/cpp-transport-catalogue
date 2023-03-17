#pragma once
#include <vector>
#include <iostream>
#include <string_view>
#include <unordered_map>
#include <map>
#include "geo.h"
#include "service.h"



namespace Transport::Detail::Read {
struct Input {
	struct Bus {
		std::string bus_name;
		std::vector<std::string> stops;
		bool is_circle;
	};
	std::vector<Bus> buses;
	std::vector<std::pair<std::string, bool>> requests;
	std::unordered_map<std::string, Coordinates> stops_and_coord;
	std::unordered_map<std::pair<std::string, std::string>, double, Transport::Detail::Hasher> distances;
	void ReadRawData(std::string &raw_str);
	void ReadRawRequest(std::string &raw_str);
	void ReadStop(std::string &raw_str);
	void ReadBus(std::string &raw_str);
	bool IsBus() { return true; }
	Input(std::istream& in);
};

}
