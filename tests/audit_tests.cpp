#include "database.hpp"
#include <stdexcept>
#define assert(expr) do { if(!(expr)) throw std::runtime_error("assertion failed: " #expr); } while(false)
#include <sqlite3.h>
#include <iostream>
int scalar(sqlite3* db,const char* sql){sqlite3_stmt* st=nullptr;assert(sqlite3_prepare_v2(db,sql,-1,&st,nullptr)==SQLITE_OK);assert(sqlite3_step(st)==SQLITE_ROW);int n=sqlite3_column_int(st,0);sqlite3_finalize(st);return n;}
int main(){sqlite3* db=nullptr;assert(sqlite3_open(":memory:",&db)==SQLITE_OK);nexus::storage::initialize(db);nexus::storage::initialize(db);
assert(sqlite3_exec(db,"INSERT INTO users(username,salt,passhash) VALUES('test','salt','hash');",nullptr,nullptr,nullptr)==SQLITE_OK);
assert(sqlite3_exec(db,"UPDATE users SET role='moderator' WHERE username='test';",nullptr,nullptr,nullptr)==SQLITE_OK);
assert(scalar(db,"SELECT COUNT(*) FROM audit_events WHERE event_type='user.role'")==1);
assert(sqlite3_exec(db,"UPDATE users SET role='moderator' WHERE username='test';",nullptr,nullptr,nullptr)==SQLITE_OK);
assert(scalar(db,"SELECT COUNT(*) FROM audit_events")==1);
sqlite3_close(db);std::cout<<"audit tests passed\n";}
