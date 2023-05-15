#include <iostream>
#include <string>
#include <vector>
#include "transport_catalogue.h"
#include "json_reader.h"
#include "request_handler.h"
#include "json.h"
#include "map_renderer.h"
#include "svg.h"
#include "log_duration.h"

int main() {
	Transport::Base::TransportCatalogue catalogue;
	Transport::Renderer::MapRenderer map_renderer;
	Transport::Base::TransportRouter transport_router;
	Transport::Base::Statistics::RequestHandler request_handler(catalogue, map_renderer, transport_router);
	Transport::Detail::Read::JsonReader json_reader(json::Load(std::cin));
	json_reader.AddStopsToCatalogue(catalogue);
	json_reader.AddBusesToCatalogue(catalogue);
	json_reader.AddRequestsToHandler(request_handler);
	json_reader.AddRenderSettingsToRenderer(map_renderer);
	json_reader.AddRouterSettings(transport_router);
	request_handler.PrintStats(std::cout);
}
