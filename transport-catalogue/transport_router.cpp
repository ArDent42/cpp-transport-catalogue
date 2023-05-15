#include "transport_router.h"
#include "transport_catalogue.h"
#include "router.h"

namespace Transport::Base {

void TransportRouter::InitializeRouter(const Transport::Base::TransportCatalogue &db) {
		graph_ = graph::DirectedWeightedGraph<double>(db.GetStopsCount());
		size_t id = 0;
		for (const auto& [stop_name, stop] : db.GetStops()) {
			stop_ids_[stop_name] = id++;
		}
		for (const auto& [bus_name, bus] : db.GetBuses()) {
			if (bus->is_circle) {
				CreateEdges(bus->stops.begin(), bus->stops.end(), db, bus_name);
			} else {
				CreateEdges(bus->stops.begin(), std::next(bus->stops.begin(), bus->stops.size() / 2 + 1), db, bus_name);
				CreateEdges(std::next(bus->stops.begin(), bus->stops.size() / 2), bus->stops.end(), db, bus_name);
			}
		}
	}

Transport::Detail::Statistics::RouteStat TransportRouter::BuildRoute(std::string_view from, std::string_view to, const graph::Router<double> &router) const {
		Transport::Detail::Statistics::RouteStat result;
		std::optional<graph::Router<double>::RouteInfo> route_info = router.BuildRoute(stop_ids_.at(from), stop_ids_.at(to));
		if (route_info) {
			for (const auto &edge_id : route_info.value().edges) {
				result.edges.push_back( { edges_by_id_.at(edge_id).from, { }, { }, double(routing_settings_.waiting_time), 0 });
				result.edges.push_back(edges_by_id_.at(edge_id));
				result.total_time += routing_settings_.waiting_time + edges_by_id_.at(edge_id).weight;
			}
		}
		return result;
	}

void TransportRouter::AddRouterSettings(const Transport::Base::RoutingSettings &rs) {
	routing_settings_ = rs;
}
const graph::DirectedWeightedGraph<double>& TransportRouter::GetGraph() const {
	return graph_;
}

}
