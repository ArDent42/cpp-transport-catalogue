#pragma once
#include "domain.h"
#include "router.h"
#include "transport_catalogue.h"
#include <vector>
#include <string_view>
#include <unordered_map>
#include <optional>

namespace Transport::Base {

constexpr double KM_PER_HOUR_TO_M_PER_MIN = 1000.0 / 60.0;

class TransportRouter {
public:
	void InitializeRouter(const Transport::Base::TransportCatalogue &db);

	template<typename It>
	void CreateEdges(It begin, It end, const Transport::Base::TransportCatalogue &db, std::string_view bus_name);

	Transport::Detail::Statistics::RouteStat BuildRoute(std::string_view from, std::string_view to, const graph::Router<double> &router) const;

	void AddRouterSettings(const Transport::Base::RoutingSettings &rs);
	const graph::DirectedWeightedGraph<double>& GetGraph() const;
	Transport::Base::RoutingSettings GetSettings() const {
		return routing_settings_;
	}
private:
	graph::DirectedWeightedGraph<double> graph_;
	Transport::Base::RoutingSettings routing_settings_;
	std::unordered_map<std::string_view, size_t> stop_ids_;
	std::vector<Transport::Base::RouteEdge> edges_by_id_;
};

template<typename It>
void TransportRouter::CreateEdges(It begin, It end, const Transport::Base::TransportCatalogue &db, std::string_view bus_name) {
	for (auto it1 = begin; it1 < end; ++it1) {
		double distance = 0;
		size_t span_count = 0;
		for (auto it2 = std::next(it1); it2 != end; ++it2) {
			distance += db.GetLength(std::make_pair((*std::prev(it2))->stop_name, (*it2)->stop_name));
			++span_count;
			graph::Edge<double> edge;
			edge.from = stop_ids_[(*it1)->stop_name];
			edge.to = stop_ids_[(*it2)->stop_name];
			edge.weight = routing_settings_.waiting_time + distance / (routing_settings_.bus_velocity * KM_PER_HOUR_TO_M_PER_MIN);
			graph_.AddEdge(edge);
			edges_by_id_.push_back({ bus_name, (*it1)->stop_name, (*it2)->stop_name, edge.weight - routing_settings_.waiting_time, span_count });

		}
	}
}

}
