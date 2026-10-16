#include "security.hpp"
#include <cassert>
#include <iostream>
int main(){
 using namespace nexus::security;
 assert(validUsername("ashkan_2026"));assert(!validUsername("ab"));assert(!validUsername("bad user"));assert(!validUsername("bad/route"));
 assert(validPassword("a_secure_password"));assert(!validPassword("short"));assert(!validPassword(std::string(129,'a')));
 assert(constantTimeEqual("abc","abc"));assert(!constantTimeEqual("abc","abd"));assert(!constantTimeEqual("abc","abcd"));
 std::cout<<"Security unit tests passed\n";
}
