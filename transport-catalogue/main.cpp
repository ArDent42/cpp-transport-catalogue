#include <iostream>
#include <string>
#include "input_reader.h"
#include "stat_reader.h"
#include "transport_catalogue.h"
#include "log_duration.h"
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
	catalogue.AddStop(input);
	catalogue.AddBus(input);
	for (const auto& [request, is_bus] : input.requests) {
		if (is_bus) {
			std::cout << catalogue.GetBusInfo(request) << std::endl;
		} else {
			std::cout << catalogue.GetStopInfo(request) << std::endl;
		}
	}

}
