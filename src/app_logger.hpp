#pragma once
#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <mutex>
#include <sstream>
#include <string>
namespace nexus::logging {
inline std::mutex& lock(){static std::mutex m;return m;}
inline void write(const std::string& level,const std::string& message){
 std::lock_guard<std::mutex> guard(lock());
 auto now=std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
 std::tm utc{};
#ifdef _WIN32
 gmtime_s(&utc,&now);
#else
 gmtime_r(&now,&utc);
#endif
 std::cerr<<std::put_time(&utc,"%Y-%m-%dT%H:%M:%SZ")<<" ["<<level<<"] "<<message<<'\n';
}
inline void info(const std::string& m){write("INFO",m);}
inline void error(const std::string& m){write("ERROR",m);}
}
