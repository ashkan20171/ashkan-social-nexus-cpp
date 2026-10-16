#pragma once
#include <string>
#include <string_view>
namespace nexus::security {
inline bool validUsername(std::string_view u) {
 if(u.size()<3 || u.size()>30) return false;
 for(unsigned char c:u) if(!((c>='a'&&c<='z')||(c>='A'&&c<='Z')||(c>='0'&&c<='9')||c=='_')) return false;
 return true;
}
inline bool validPassword(std::string_view p) { return p.size()>=12 && p.size()<=128; }
inline bool constantTimeEqual(std::string_view a,std::string_view b) {
 size_t n=a.size()>b.size()?a.size():b.size();unsigned int diff=static_cast<unsigned int>(a.size()^b.size());
 for(size_t i=0;i<n;++i){unsigned char x=i<a.size()?static_cast<unsigned char>(a[i]):0;unsigned char y=i<b.size()?static_cast<unsigned char>(b[i]):0;diff|=x^y;}
 return diff==0;
}
}
