#pragma once
#include <vector>
#include <iostream>
#include <string_view>
#include <unordered_map>
#include <map>
#include <deque>
#include "transport_catalogue.h"
#include "service.h"

namespace Transport::Detail::Read {

struct Input {
	std::vector<std::string> raw_stops;
	std::vector<std::string> raw_buses;
	std::vector<std::pair<std::string, bool>> requests;
	std::unordered_map<std::string, Transport::Base::Stop> stops;
	std::unordered_map<std::string, Transport::Base::Bus> buses;
	std::unordered_map<std::pair<std::string, std::string>, double, Hasher> distances;
	void ReadRawData(std::string &raw_str);
	void ReadRawRequest(std::string &raw_str);
	void ReadStop(std::string& raw_str);
	void ReadDistances(std::string &raw_str, const std::string &stop_name);
	void ReadBus(std::string& raw_str);
	bool IsBus() { return true; }
	Input(std::istream& in);
};

}
