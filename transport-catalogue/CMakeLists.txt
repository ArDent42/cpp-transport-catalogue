cmake_minimum_required(VERSION 3.10)

project(TRANSPORT_CATALOGUE CXX)
set(CMAKE_CXX_STANDARD 17)

find_package(Protobuf REQUIRED)
find_package(Threads REQUIRED)

protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS transport_catalogue.proto svg.proto map_renderer.proto graph.proto transport_router.proto)

set(CATALOGUE transport_catalogue.cpp
					   transport_catalogue.h
					   transport_catalogue.proto
					   domain.cpp
					   domain.h)

set(UTILITY geo.h
			geo.cpp
			ranges.h)

set(JSON json_builder.cpp
		 json_builder.h
		 json.cpp
		 json.h
		 json_reader.cpp
		 json_reader.h)

set(ROUTER transport_router.cpp
		   transport_router.h
		   router.h
		   graph.h
		   graph.proto
		   transport_router.proto)

set(MAP_RENDERER map_renderer.cpp
				 map_renderer.h
				 svg.cpp
				 svg.h
				 map_renderer.proto
				 svg.proto)

set(REQUEST_HANDLER request_handler.cpp
				   request_handler.h)

set(SERIALIZATION serialization.cpp 
				  serialization.h)

set(TRANSPORT_CATALOGUE_FILES main.cpp ${CATALOGUE} ${UTILITY} ${JSON} ${MAP_RENDERER} ${ROUTER} ${REQUEST_HANDLER} ${SERIALIZATION})

add_executable(transport_catalogue ${PROTO_SRCS} ${PROTO_HDRS} ${TRANSPORT_CATALOGUE_FILES})
target_include_directories(transport_catalogue PUBLIC ${Protobuf_INCLUDE_DIRS})
target_include_directories(transport_catalogue PUBLIC ${CMAKE_CURRENT_BINARY_DIR})

string(REPLACE "protobuf.lib" "protobufd.lib" "Protobuf_LIBRARY_DEBUG" "${Protobuf_LIBRARY_DEBUG}")
string(REPLACE "protobuf.a" "protobufd.a" "Protobuf_LIBRARY_DEBUG" "${Protobuf_LIBRARY_DEBUG}")

target_link_libraries(transport_catalogue "$<IF:$<CONFIG:Debug>,${Protobuf_LIBRARY_DEBUG},${Protobuf_LIBRARY}>" Threads::Threads)