#include <iostream>
#include <string>
#include "transport_catalogue.h"
#include "json_reader.h"
#include "request_handler.h"
#include "json.h"
#include "map_renderer.h"
#include "svg.h"

int main() {
	Transport::Base::TransportCatalogue catalogue;
	Transport::Renderer::MapRenderer map_renderer;
	Transport::Base::Statistics::RequestHandler request_handler(catalogue, map_renderer);
	Transport::Detail::Read::JsonReader json_reader(json::Load(std::cin));
	json_reader.AddStopsToCatalogue(catalogue);
	json_reader.AddBusesToCatalogue(catalogue);
	json_reader.AddRequestsToHandler(request_handler);
	json_reader.AddRenderSettingsToRenderer(map_renderer);
	request_handler.PrintStats(std::cout);
	//svg::Document doc = request_handler.RenderMap();
	//doc.Render(std::cout);
}
