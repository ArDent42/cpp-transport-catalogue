#pragma once
#include <transport_catalogue.pb.h>
#include <svg.pb.h>
#include <map_renderer.pb.h>
#include <transport_router.pb.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <string_view>
#include "transport_catalogue.h"
#include "domain.h"
#include "map_renderer.h"
#include "transport_router.h"

namespace Serialization {

struct SerializationSettings {
	std::string file_name;
};

class Serial {
public:
	Serial(const Serialization::SerializationSettings& settings);
	void Serialize(const Transport::Base::TransportCatalogue &db, const Transport::Renderer::MapRenderer &mr,const Transport::Base::TransportRouter& tr);
	void Deserialize(Transport::Base::TransportCatalogue &db, Transport::Renderer::MapRenderer &mr, Transport::Base::TransportRouter& tr);

private:
	protobuf::TransportCatalogue transport_catalogue_proto_;
	SerializationSettings settings_;
	void SerializeCatalogue(const Transport::Base::TransportCatalogue &db);
	void SerializeMapRender(const Transport::Renderer::MapRenderer &mr);
	void SerializeRouterSettings(const Transport::Base::TransportRouter& tr);
	void DeserializeCatalogue(Transport::Base::TransportCatalogue &db);
	void DeserializeMapRender(Transport::Renderer::MapRenderer &mr);
	void DeserializeRouterSettings(Transport::Base::TransportRouter& tr);
	protobuf::Color SerializeColor(const svg::Color &color);
	svg::Color DeserializeColor(const protobuf::Color &color_proto);
};

}
