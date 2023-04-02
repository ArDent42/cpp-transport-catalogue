#pragma once
#include <unordered_map>
#include <optional>
#include "transport_catalogue.h"
#include "json.h"
#include "map_renderer.h"

namespace Transport::Base::Statistics {

using namespace std::literals;

class RequestHandler {
public:
	RequestHandler(const Transport::Base::TransportCatalogue &db,
			Transport::Renderer::MapRenderer &mr);
	std::optional<Transport::Detail::Statistics::BusStat> GetBusStat(
			const std::string_view &bus_name) const;
	std::optional<Transport::Detail::Statistics::StopStat> GetBusesByStop(
			const std::string_view &stop_name) const;
	void AddRequests(const std::vector<Transport::Base::Request> &requests);
	void MakeStopStatOutput(json::Array &stats,
			const Transport::Base::Request &request) const;
	void MakeBusStatOutput(json::Array &stats,
			const Transport::Base::Request &request) const;
	void MakeMapRendererOutput(json::Array &stats,
				const Transport::Base::Request &request) const;
	void PrintStats(std::ostream &out) const;
	svg::Document RenderMap() const {
		return mr_.Render(db_);
	}
private:
	const Transport::Base::TransportCatalogue &db_;
	Transport::Renderer::MapRenderer &mr_;
	std::vector<Transport::Base::Request> requests_;
};

}
