#include "database.hpp"
#include <sqlite3.h>
#ifdef NDEBUG
#undef NDEBUG
#endif
#include <cassert>
#include <iostream>
int main(){
 sqlite3* db=nullptr;
 if(sqlite3_open(":memory:",&db)!=SQLITE_OK)return 1;
 nexus::storage::initialize(db);
 nexus::storage::initialize(db); // idempotent migration
 sqlite3_stmt* st=nullptr;
 assert(sqlite3_prepare_v2(db,"SELECT COUNT(*) FROM sqlite_master WHERE type='table' AND name='posts'",-1,&st,nullptr)==SQLITE_OK);
 assert(sqlite3_step(st)==SQLITE_ROW && sqlite3_column_int(st,0)==1);
 sqlite3_finalize(st);
 assert(sqlite3_prepare_v2(db,"PRAGMA table_info(users)",-1,&st,nullptr)==SQLITE_OK);
 bool role=false,bio=false;while(sqlite3_step(st)==SQLITE_ROW){auto col=reinterpret_cast<const char*>(sqlite3_column_text(st,1));if(std::string(col)=="role")role=true;if(std::string(col)=="bio")bio=true;}
 sqlite3_finalize(st);assert(role&&bio);
 sqlite3_close(db);std::cout<<"Database migrations passed\n";
}
