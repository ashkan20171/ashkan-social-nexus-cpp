#include "api_contract.hpp"
#include <cassert>
int main(){using namespace nexus::api;
 assert(isApiPath("/api/feed"));assert(!isApiPath("/assets/app.js"));assert(!isApiPath("/api"));
 assert(isSupportedMethod("GET"));assert(isSupportedMethod("POST"));assert(!isSupportedMethod("TRACE"));
 assert(isValidHttpVersion("HTTP/1.1"));assert(!isValidHttpVersion("HTTP/2.0"));
}
