#include "database.hpp"
#include "app_logger.hpp"
#include "api_contract.hpp"
#include "security.hpp"
#include "http_utils.hpp"
#include <openssl/rand.h>
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <map>
#include <vector>
#include <random>
#include <iomanip>
#include <filesystem>
#include <algorithm>
#include <sqlite3.h>
#include <openssl/evp.h>
#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib,"Ws2_32.lib")
using Socket=SOCKET;
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
using Socket=int;
#define INVALID_SOCKET (-1)
#endif
namespace fs=std::filesystem;
static sqlite3* db=nullptr;
void closeSocket(Socket s){
#ifdef _WIN32
 closesocket(s);
#else
 close(s);
#endif
}
std::string escape(const std::string& s){std::string o;for(char c:s){if(c=='"'||c=='\\')o+='\\';if(c=='\n'){o+="\\n";continue;}if((unsigned char)c<32)continue;o+=c;}return o;}
std::string field(const std::string& j,const std::string& key){auto p=j.find("\""+key+"\"");if(p==std::string::npos)return "";p=j.find(':',p);if(p==std::string::npos)return "";p=j.find('"',p);if(p==std::string::npos)return "";std::string out;for(++p;p<j.size();++p){if(j[p]=='\\'&&p+1<j.size()){char c=j[++p];out+=(c=='n'?'\n':c);continue;}if(j[p]=='"')break;out+=j[p];}return out;}
std::string randomHex(int bytes=24){
 if(bytes<=0||bytes>1024)return "";
 std::vector<unsigned char> buffer(static_cast<size_t>(bytes));
 if(RAND_bytes(buffer.data(),bytes)!=1)throw std::runtime_error("Secure random generation failed");
 std::ostringstream out;for(auto c:buffer)out<<std::hex<<std::setw(2)<<std::setfill('0')<<static_cast<int>(c);
 return out.str();
}
std::string hashPassword(const std::string& pass,const std::string& salt){unsigned char out[32];if(PKCS5_PBKDF2_HMAC(pass.c_str(),(int)pass.size(),(const unsigned char*)salt.data(),(int)salt.size(),160000,EVP_sha256(),32,out)!=1)return "";std::ostringstream o;for(auto c:out)o<<std::hex<<std::setw(2)<<std::setfill('0')<<(int)c;return o.str();}
void sql(const char* q){char* err=nullptr;if(sqlite3_exec(db,q,nullptr,nullptr,&err)!=SQLITE_OK){std::cerr<<(err?err:"SQL error")<<'\n';sqlite3_free(err);}}
struct St{sqlite3_stmt* p=nullptr;St(const char* q){sqlite3_prepare_v2(db,q,-1,&p,nullptr);}~St(){if(p)sqlite3_finalize(p);}void bind(int n,const std::string& s){sqlite3_bind_text(p,n,s.c_str(),-1,SQLITE_TRANSIENT);}void bind(int n,int v){sqlite3_bind_int(p,n,v);}bool next(){return sqlite3_step(p)==SQLITE_ROW;}int num(int n){return sqlite3_column_int(p,n);}std::string str(int n){auto p=sqlite3_column_text(this->p,n);return p?(const char*)p:"";}};
std::string userFor(const std::string& token){St q("SELECT u.username FROM sessions s JOIN users u ON s.user_id=u.id WHERE s.token=? AND s.created_at>datetime('now','-7 days')");q.bind(1,token);return q.next()?q.str(0):"";}
bool validImage(const std::string& data){
 if(data.empty())return true;
 if(data.size()>350000)return false;
 size_t start=std::string::npos;
 for(const char* prefix:{"data:image/png;base64,","data:image/jpeg;base64,","data:image/webp;base64,"}){
  if(data.rfind(prefix,0)==0){start=std::string(prefix).size();break;}
 }
 if(start==std::string::npos||start==data.size())return false;
 for(size_t i=start;i<data.size();++i){char c=data[i];if(!((c>='A'&&c<='Z')||(c>='a'&&c<='z')||(c>='0'&&c<='9')||c=='+'||c=='/'||c=='='))return false;}
 return true;
}
#include "api_service.hpp"
#include "http_server.hpp"
int main(){
#ifdef _WIN32
 WSADATA w;WSAStartup(MAKEWORD(2,2),&w);
#endif
if(sqlite3_open("ashkan_social.db",&db)!=SQLITE_OK){nexus::logging::error("Database initialization failed");return 1;}
sqlite3_busy_timeout(db,5000);
try { nexus::storage::initialize(db); } catch(const std::exception& ex){nexus::logging::error(ex.what());sqlite3_close(db);return 1;}
Socket server=socket(AF_INET,SOCK_STREAM,0);if(server==INVALID_SOCKET)return 1;int yes=1;setsockopt(server,SOL_SOCKET,SO_REUSEADDR,(char*)&yes,sizeof(yes));sockaddr_in addr{};addr.sin_family=AF_INET;addr.sin_port=htons(8080);addr.sin_addr.s_addr=htonl(INADDR_LOOPBACK);if(bind(server,(sockaddr*)&addr,sizeof(addr))<0||listen(server,16)<0){nexus::logging::error("Port 8080 unavailable");return 1;}nexus::logging::info("Server listening at http://127.0.0.1:8080");while(true){Socket c=accept(server,nullptr,nullptr);if(c!=INVALID_SOCKET)handle(c);} }
