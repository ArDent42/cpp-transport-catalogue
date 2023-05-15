#pragma once
#include <unordered_map>
#include <optional>
#include "transport_catalogue.h"
#include "json.h"
#include "map_renderer.h"
#include "graph.h"
#include "transport_router.h"

namespace Transport::Base::Statistics {

using namespace std::literals;

class RequestHandler {
public:
	RequestHandler(const Transport::Base::TransportCatalogue &db, Transport::Renderer::MapRenderer &mr, TransportRouter &tr);
	std::optional<Transport::Detail::Statistics::BusStat> GetBusStat(const std::string_view &bus_name) const;
	std::optional<Transport::Detail::Statistics::StopStat> GetBusesByStop(const std::string_view &stop_name) const;
	std::optional<Transport::Detail::Statistics::RouteStat> GetRouteStat(std::string_view from, std::string_view to, const graph::Router<double>& router) const;
	void AddRequests(const std::vector<Transport::Base::Request> &requests);
	void MakeStopStatOutput(json::Array &stats, const Transport::Base::Request &request) const;
	void MakeBusStatOutput(json::Array &stats, const Transport::Base::Request &request) const;
	void MakeMapRendererOutput(json::Array &stats, const Transport::Base::Request &request) const;
	void MakeRouteStatOutput(json::Array &stats, const Transport::Base::Request &request, const graph::Router<double>& router) const;
	void PrintStats(std::ostream &out) const;
	svg::Document RenderMap() const {
		return mr_.Render(db_);
	}

private:
	const Transport::Base::TransportCatalogue &db_;
	Transport::Renderer::MapRenderer &mr_;
	TransportRouter &tr_;
	std::vector<Transport::Base::Request> requests_;
	graph::DirectedWeightedGraph<double> graph_;
};

}
