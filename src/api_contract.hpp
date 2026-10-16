#pragma once
#include <string_view>
namespace nexus::api {
inline bool isApiPath(std::string_view path){return path.size()>=5 && path.substr(0,5)=="/api/";}
inline bool isSupportedMethod(std::string_view method){return method=="GET"||method=="POST";}
inline bool isValidHttpVersion(std::string_view version){return version=="HTTP/1.0"||version=="HTTP/1.1";}
}
