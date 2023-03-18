#include <iostream>
#include <string>
#include "input_reader.h"
#include "stat_reader.h"
#include "transport_catalogue.h"
//#include "log_duration.h"
//#include "tests.h"
using namespace std;

int main() {
	//TestInputReader();
	//TestAddStop();
	//TestAddBus();
	//TestDistance();
	//TestStats();
	Transport::Base::TransportCatalogue catalogue;
	Transport::Detail::Read::Input input(cin);
	for (const auto& [stop_name, stop] : input.stops) {
		catalogue.AddStop(stop);
	}
	catalogue.AddDistances();
	for (const auto& [bus_name, bus] : input.buses) {
			catalogue.AddBus(bus);
		}
	for (const auto& [request, is_bus] : input.requests) {
		if (is_bus) {
			std::cout << catalogue.GetBusInfo(request) << std::endl;
		} else {
			std::cout << catalogue.GetStopInfo(request) << std::endl;
		}
	}

}
