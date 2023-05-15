#pragma once
#include <vector>
#include <iostream>
#include <string_view>
#include <unordered_map>
#include <map>
#include <deque>
#include "transport_catalogue.h"
#include "json.h"
#include "request_handler.h"
#include "map_renderer.h"

namespace Transport::Detail::Read {

class JsonReader {
public:
	JsonReader(const json::Document &document) :
			document_(document) {
	}
	void AddStopsToCatalogue(Transport::Base::TransportCatalogue &db);
	void AddBusesToCatalogue(Transport::Base::TransportCatalogue &db);
	void AddRequestsToHandler(
			Transport::Base::Statistics::RequestHandler &request_handler);
	void AddRenderSettingsToRenderer(
			Transport::Renderer::MapRenderer &map_renderer);
	void AddRouterSettings(Transport::Base::TransportRouter& tr);

private:
	const json::Document document_;
};

}
