#include "http_utils.hpp"
#include <cassert>
int main(){using namespace nexus::http;
 assert(parseContentLength("0")==0);assert(parseContentLength("524288")==524288);
 assert(!parseContentLength("-1"));assert(!parseContentLength("12x"));assert(!parseContentLength("999999999999999999"));
 assert(safeAssetPath("/"));assert(safeAssetPath("/index.html"));assert(safeAssetPath("/assets/app.js"));
 assert(!safeAssetPath("/../secret"));assert(!safeAssetPath("/a/../b"));assert(!safeAssetPath("/%2e%2e/x"));assert(!safeAssetPath("/a\\b"));
 assert(statusText(413)=="Payload Too Large");return 0;
}
