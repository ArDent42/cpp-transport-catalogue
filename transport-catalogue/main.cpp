#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include "transport_catalogue.h"
#include "json_reader.h"
#include "request_handler.h"
#include "json.h"
#include "map_renderer.h"
#include "svg.h"
#include "serialization.h"

using namespace std::literals;

void PrintUsage(std::ostream &stream = std::cerr) {
	stream << "Usage: transport_catalogue [make_base|process_requests]\n"sv;
}

//int main() {
//	Transport::Base::TransportCatalogue catalogue;
//	Transport::Renderer::MapRenderer map_renderer;
//	Transport::Base::TransportRouter transport_router;
//	Transport::Base::Statistics::RequestHandler request_handler(catalogue, map_renderer, transport_router);
//	Transport::Detail::Read::JsonReader json_reader(json::Load(std::cin));
//	json_reader.AddStopsToCatalogue(catalogue);
//	json_reader.AddBusesToCatalogue(catalogue);
//	json_reader.AddRequestsToHandler(request_handler);
//	json_reader.AddRenderSettingsToRenderer(map_renderer);
//	json_reader.AddRouterSettings(transport_router);
//	request_handler.PrintStats(std::cout);
//}

int main(int argc, char *argv[]) {
	if (argc != 2) {
		PrintUsage();
		return 1;
	}
	const std::string_view mode(argv[1]);
	Transport::Base::TransportCatalogue catalogue;
	Transport::Renderer::MapRenderer map_renderer;
	Transport::Base::TransportRouter transport_router;
	Transport::Detail::Read::JsonReader json_reader(json::Load(std::cin));
	Serialization::Serial serial(json_reader.GetSerializationSettings());
	if (mode == "make_base"sv) {
		json_reader.AddStopsToCatalogue(catalogue);
		json_reader.AddBusesToCatalogue(catalogue);
		json_reader.AddRenderSettingsToRenderer(map_renderer);
		json_reader.AddRouterSettings(transport_router);
		serial.Serialize(catalogue, map_renderer, transport_router);
	} else if (mode == "process_requests"sv) {
		Transport::Base::TransportRouter transport_router;
		Transport::Base::Statistics::RequestHandler request_handler(catalogue, map_renderer, transport_router);
		serial.Deserialize(catalogue, map_renderer, transport_router);
		json_reader.AddRequestsToHandler(request_handler);
		request_handler.PrintStats(std::cout);
	} else {
		PrintUsage();
		return 1;
	}

}
