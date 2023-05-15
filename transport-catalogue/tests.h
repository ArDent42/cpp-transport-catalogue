//#pragma once
//#include <vector>
//#include <string>
//#include <cassert>
//#include <iostream>
//#include "input_reader.h"
//
//void TestInputReader() {
//	InputReader input;
//	std::vector<std::string> raw_strings =
//			{
//					"Stop Tolstopaltsevo: 55.611087, 37.208290",
//					"Stop Marushkino: 55.595884, 37.209755",
//					"Bus 256: Biryulyovo Zapadnoye > Biryusinka > Universam > Biryulyovo Tovarnaya > Biryulyovo Passazhirskaya > Biryulyovo Zapadnoye",
//					"Bus 750: Tolstopaltsevo - Marushkino - Rasskazovka",
//					"Stop Rasskazovka: 55.632761, 37.333324",
//					"Stop Biryulyovo Zapadnoye: 55.574371, 37.651700",
//					"Stop Biryusinka: 55.581065, 37.648390",
//					"Stop Universam: 55.587655, 37.645687",
//					"Stop Biryulyovo Tovarnaya: 55.592028, 37.653656",
//					"Stop Biryulyovo Passazhirskaya: 55.580999, 37.659164"
//			};
//	for (auto &str : raw_strings)
//	{
//		input.ReadRawString(str);
//	}
//	assert(input.raw_stops[0] == "Tolstopaltsevo: 55.611087, 37.208290");
//	assert(input.raw_stops[3] == "Biryulyovo Zapadnoye: 55.574371, 37.651700");
//	assert(
//			input.raw_buses[0]
//					== "256: Biryulyovo Zapadnoye > Biryusinka > Universam > Biryulyovo Tovarnaya > Biryulyovo Passazhirskaya > Biryulyovo Zapadnoye");
//	assert(input.raw_buses[1] == "750: Tolstopaltsevo - Marushkino - Rasskazovka");
//	std::cout << "InputReader test OK" << std::endl;
//}
//
//void TestAddStop() {
//	InputReader input;
//	TransportCatalogue catalogue;
//	std::vector<std::string> raw_strings =
//			{
//					"Stop Tolstopaltsevo: 55.611087, 37.208290",
//					"Stop Marushkino: 55.595884, 37.209755",
//					"Bus 256: Biryulyovo Zapadnoye > Marushkino > Rasskazovka",
//					"Bus 750: Tolstopaltsevo - Marushkino - Rasskazovka",
//					"Stop Rasskazovka: 55.632761, 37.333324",
//					"Stop Biryulyovo Zapadnoye: 55.574371, 37.651700"
//			};
//	for (auto &str : raw_strings)
//	{
//		input.ReadRawString(str);
//	}
//
//	for (auto &str : input.raw_stops) {
//		catalogue.AddStop(str);
//	}
//	auto stop1 = catalogue.FindStop("Tolstopaltsevo");
//	auto stop2 = catalogue.FindStop("Marushkino");
//	auto stop3 = catalogue.FindStop("Biryulyovo Zapadnoye");
//	assert(stop1->stop_name_ == "Tolstopaltsevo");
//	assert(stop2->stop_name_ == "Marushkino");
//	assert(stop3->stop_name_ == "Biryulyovo Zapadnoye");
//	double lng = stop1->coordinates_.lng;
//	double lat = stop1->coordinates_.lat;
//	assert(std::abs(lat - 55.611087) < 0.0001);
//	assert(std::abs(lng - 37.208290) < 0.0001);
//	std::cout << "AddStop test OK" << std::endl;
//}
//
//void TestAddBus() {
//	InputReader input;
//	TransportCatalogue catalogue;
//	std::vector<std::string> raw_strings =
//			{
//					"Stop Tolstopaltsevo: 55.611087, 37.208290",
//					"Stop Marushkino: 55.595884, 37.209755",
//					"Bus 256: Biryulyovo Zapadnoye > Biryusinka > Universam > Biryulyovo Tovarnaya > Biryulyovo Passazhirskaya > Biryulyovo Zapadnoye",
//					"Bus 750: Tolstopaltsevo - Marushkino - Rasskazovka",
//					"Stop Rasskazovka: 55.632761, 37.333324",
//					"Stop Biryulyovo Zapadnoye: 55.574371, 37.651700",
//					"Stop Biryusinka: 55.581065, 37.648390",
//					"Stop Universam: 55.587655, 37.645687",
//					"Stop Biryulyovo Tovarnaya: 55.592028, 37.653656",
//					"Stop Biryulyovo Passazhirskaya: 55.580999, 37.659164"
//			};
//	for (auto &str : raw_strings)
//	{
//		input.ReadRawString(str);
//	}
//
//	for (auto &str : input.raw_stops) {
//		catalogue.AddStop(str);
//	}
//	for (auto &str : input.raw_buses) {
//		catalogue.AddBus(str);
//	}
//	auto bus1 = catalogue.FindBus("256");
//	auto bus2 = catalogue.FindBus("750");
//	assert(bus1->bus_name_ == "256");
//	assert(bus2->bus_name_ == "750");
//	std::vector<std::string> bus1_stops;
//	for (const auto stop : bus1->stops_) {
//		bus1_stops.push_back(stop->stop_name_);
//	}
//	std::vector<std::string> bus2_stops;
//	for (const auto stop : bus2->stops_) {
//		bus2_stops.push_back(stop->stop_name_);
//	}
//	std::vector<std::string> bus1_stops_ = {"Biryulyovo Zapadnoye", "Biryusinka", "Universam", "Biryulyovo Tovarnaya", "Biryulyovo Passazhirskaya", "Biryulyovo Zapadnoye",};
//	std::vector<std::string> bus2_stops_ = {"Tolstopaltsevo", "Marushkino", "Rasskazovka", "Marushkino", "Tolstopaltsevo"};
//	assert(bus1_stops == bus1_stops_);
//	assert(bus2_stops == bus2_stops_);
//	std::cout << "AddBus test OK" << std::endl;
//	BusStat stat = catalogue.GetBusInfo("750");
//	std::cout << stat;
//}
