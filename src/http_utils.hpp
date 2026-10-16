#pragma once
#include <algorithm>
#include <cctype>
#include <optional>
#include <string>
#include <string_view>
namespace nexus::http {
inline std::optional<size_t> parseContentLength(std::string_view value) {
 if(value.empty() || value.size()>10) return std::nullopt;
 size_t result=0;
 for(char c:value){if(c<'0'||c>'9')return std::nullopt;size_t digit=static_cast<size_t>(c-'0');if(result>(1048576-digit)/10)return std::nullopt;result=result*10+digit;}
 return result;
}
inline bool safeAssetPath(std::string_view path){
 if(path.empty()||path.front()!='/'||path.find('?')!=path.npos||path.find('#')!=path.npos||path.find('\\')!=path.npos)return false;
 size_t start=1;
 while(start<=path.size()) {auto end=path.find('/',start);auto part=path.substr(start,end==path.npos?path.npos:end-start);if(part==".."||part=="."||part.empty()&&start!=path.size())return false; if(end==path.npos)break;start=end+1;}
 return path.find('%')==path.npos && path.find('\0')==path.npos;
}
inline std::string statusText(int code){switch(code){case 200:return "OK";case 400:return "Bad Request";case 401:return "Unauthorized";case 403:return "Forbidden";case 404:return "Not Found";case 409:return "Conflict";case 413:return "Payload Too Large";case 431:return "Request Header Fields Too Large";default:return "Internal Server Error";}}
}
