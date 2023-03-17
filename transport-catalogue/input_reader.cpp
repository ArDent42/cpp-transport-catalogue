#include "input_reader.h"

Transport::Detail::Read::Input::Input(std::istream &in) {
	std::string str;
	getline(in, str);
	int i = std::stoi(str);
	while (i > 0) {
		getline(in, str);
		ReadRawData(str);
		--i;
	}
	getline(in, str);
	i = std::stoi(str);
	while (i > 0) {
		getline(in, str);
		ReadRawRequest(str);
		--i;
	}
}

void Transport::Detail::Read::Input::ReadRawData(std::string &raw_str) {
	if (raw_str[0] == 'B') {
		ReadBus(raw_str);
	} else {
		ReadStop(raw_str);
	}
}

void Transport::Detail::Read::Input::ReadBus(std::string &raw_str) {
	raw_str = raw_str.substr(4);
	auto colon_index = raw_str.find_first_of(':');
	Transport::Detail::Read::Input::Bus bus;
	bus.bus_name = raw_str.substr(0, colon_index);
	raw_str = raw_str.substr(colon_index + 2);
	auto delimiter = raw_str.find_first_of(">-");
	bus.is_circle = raw_str[delimiter] == '>' ? true : false;
	while (!raw_str.empty()) {
		std::string stop_name;
		if (delimiter != raw_str.npos) {
			stop_name = raw_str.substr(0, delimiter - 1);
			raw_str = raw_str.substr(delimiter + 2);
		} else {
			stop_name = raw_str;
			raw_str.clear();
		}
		bus.stops.push_back(std::move(stop_name));
		delimiter = raw_str.find_first_of(">-");
	}
	if (!bus.is_circle) {
		bus.stops.reserve(bus.stops.size() * 2 - 1);
		std::copy(bus.stops.rbegin() + 1, bus.stops.rend(), std::back_inserter(bus.stops));
	}
	buses.push_back(bus);
}

void Transport::Detail::Read::Input::ReadStop(std::string &raw_str) {
	raw_str = raw_str.substr(5);
	auto colon_index = raw_str.find_first_of(':');
	std::string stop_name = raw_str.substr(0, colon_index);
	raw_str = raw_str.substr(colon_index + 2);
	auto comma_index = raw_str.find_first_of(',');
	std::string lat = raw_str.substr(0, comma_index);
	std::string lng;
	raw_str = raw_str.substr(comma_index + 2);
	comma_index = raw_str.find_first_of(',');
	if (comma_index == raw_str.npos) {
		lng = raw_str;
		raw_str.clear();
	} else {
		lng = raw_str.substr(0, comma_index);
		raw_str = raw_str.substr(comma_index + 2);
	}
	stops_and_coord[stop_name].lat = std::stod(lat);
	stops_and_coord[stop_name].lng = std::stod(lng);
	while (!raw_str.empty()) {
		std::string name_dist_to;
		std::string length = raw_str.substr(0, raw_str.find_first_of(' ') - 1);
		raw_str = raw_str.substr(raw_str.find_first_of(' ') + 4);
		comma_index = raw_str.find_first_of(',');
		if (comma_index == raw_str.npos) {
			name_dist_to = raw_str;
			raw_str.clear();
		} else {
			name_dist_to = raw_str.substr(0, comma_index);
			raw_str = raw_str.substr(comma_index + 2);
		}
		distances[std::make_pair(stop_name, name_dist_to)] = std::stod(length);
	}
}

void Transport::Detail::Read::Input::ReadRawRequest(std::string &raw_str) {
	if (raw_str[0] == 'B') {
		raw_str = raw_str.substr(4);
		requests.push_back(std::make_pair(raw_str, IsBus()));
	} else {
		raw_str = raw_str.substr(5);
		requests.push_back(std::make_pair(raw_str, !IsBus()));
	}
}
