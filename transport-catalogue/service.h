#pragma once
#include <string>
namespace Transport::Detail {
struct Hasher {
    size_t operator()(const std::pair<std::string_view, std::string_view>& lenght) const{
        return (std::hash<std::string_view>{}(lenght.first) + std::hash<std::string_view>{}(lenght.second));
    }
};

}
