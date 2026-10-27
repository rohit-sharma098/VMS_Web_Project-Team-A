#include <algorithm>
#include <ctime>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <direct.h>
using namespace std;

struct Visitor { int id{}; string name,phone,email,idProof,purpose,host,status="Pending",pass,checkIn,checkOut; bool pre=false; };
struct Entry { string name,idProof,reason; };
string root="data";
string stamp(){ time_t t=time(nullptr); tm *lt=localtime(&t); char b[32]; strftime(b,sizeof b,"%Y-%m-%d %H:%M:%S",lt); return b; }
string clean(string s){ for(char &c:s) if(c=='|'||c=='\n'||c=='\r') c=' '; return s; }
string esc(const string&s){ string o; for(unsigned char c:s){ switch(c){case '"':o+="\\\"";break;case '\\':o+="\\\\";break;case '\n':o+="\\n";break;case '\r':o+="\\r";break;case '\t':o+="\\t";break;default: if(c<32){}else o+=(char)c;} } return o; }
string js(const string&s){return "\""+esc(s)+"\"";}
vector<string> split(const string&s,char d='|'){ vector<string> v; string x; stringstream ss(s); while(getline(ss,x,d))v.push_back(x); return v; }
vector<Visitor> loadV(){ vector<Visitor> a; ifstream f(root+"/visitors.txt"); string l; while(getline(f,l)){auto p=split(l); if(p.size()<12)continue; Visitor v; try{v.id=stoi(p[0]);}catch(...){continue;} v.name=p[1];v.phone=p[2];v.email=p[3];v.idProof=p[4];v.purpose=p[5];v.host=p[6];v.status=p[7];v.pass=p[8];v.checkIn=p[9];v.checkOut=p[10];v.pre=p[11]=="1";a.push_back(v);} return a; }
void saveV(const vector<Visitor>&a){ofstream f(root+"/visitors.txt",ios::trunc);for(auto&v:a)f<<v.id<<'|'<<v.name<<'|'<<v.phone<<'|'<<v.email<<'|'<<v.idProof<<'|'<<v.purpose<<'|'<<v.host<<'|'<<v.status<<'|'<<v.pass<<'|'<<v.checkIn<<'|'<<v.checkOut<<'|'<<(v.pre?1:0)<<'\n';}
vector<Entry> loadB(){vector<Entry>a;ifstream f(root+"/blacklist.txt");string l;while(getline(f,l)){auto p=split(l);if(p.size()>=3)a.push_back({p[0],p[1],p[2]});}return a;}
void saveB(const vector<Entry>&a){ofstream f(root+"/blacklist.txt",ios::trunc);for(auto&e:a)f<<e.name<<'|'<<e.idProof<<'|'<<e.reason<<'\n';}
void visitorJson(const Visitor&v){cout<<"{\"id\":"<<v.id<<",\"name\":"<<js(v.name)<<",\"phone\":"<<js(v.phone)<<",\"email\":"<<js(v.email)<<",\"idProof\":"<<js(v.idProof)<<",\"purpose\":"<<js(v.purpose)<<",\"host\":"<<js(v.host)<<",\"status\":"<<js(v.status)<<",\"pass\":"<<js(v.pass)<<",\"checkIn\":"<<js(v.checkIn)<<",\"checkOut\":"<<js(v.checkOut)<<",\"pre\":"<<(v.pre?"true":"false")<<"}";}
void listJson(const vector<Visitor>&a){cout<<"[";for(size_t i=0;i<a.size();i++){if(i)cout<<",";visitorJson(a[i]);}cout<<"]";}
int main(int argc, char** argv){_mkdir(root.c_str());string action=argc>1?argv[1]:"list";vector<Visitor> vs=loadV();vector<Entry> bs=loadB();auto arg=[&](int i)->string{return argc>i?clean(argv[i]):"";};
 if(action=="list"||action=="dashboard"){if(action=="dashboard")vs.erase(remove_if(vs.begin(),vs.end(),[](auto&v){return v.status!="CheckedIn";}),vs.end());listJson(vs);}
 else if(action=="register"){if(argc<9){cout<<"{\"ok\":false,\"message\":\"Missing visitor fields\"}";return 0;} Visitor v;v.id=1;for(auto&x:vs)v.id=max(v.id,x.id+1);v.name=arg(2);v.phone=arg(3);v.email=arg(4);v.idProof=arg(5);v.purpose=arg(6);v.host=arg(7);v.pre=arg(8)=="1";for(auto&e:bs)if(e.idProof==v.idProof||e.name==v.name){v.status="Denied";break;}vs.push_back(v);saveV(vs);cout<<"{\"ok\":true,\"id\":"<<v.id<<",\"status\":"<<js(v.status)<<"}";}
 else if(action=="decision"||action=="checkin"||action=="checkout"){int id=0;try{id=stoi(arg(2));}catch(...){} bool ok=false;string msg="Visit not found or status does not allow this action";for(auto&v:vs)if(v.id==id){if(action=="decision"&&v.status=="Pending"){if(arg(3)=="approve"){v.status="Approved";v.pass="VMS-PASS-"+to_string(v.id)+"-"+stamp().substr(0,10);}else v.status="Rejected";ok=true;}else if(action=="checkin"&&v.status=="Approved"){v.status="CheckedIn";v.checkIn=stamp();ok=true;}else if(action=="checkout"&&v.status=="CheckedIn"){v.status="CheckedOut";v.checkOut=stamp();ok=true;}break;}if(ok)saveV(vs);cout<<"{\"ok\":"<<(ok?"true":"false")<<",\"message\":"<<js(ok?"Action completed successfully":msg)<<"}";}
 else if(action=="blacklist"){cout<<"[";for(size_t i=0;i<bs.size();i++){if(i)cout<<",";cout<<"{\"name\":"<<js(bs[i].name)<<",\"idProof\":"<<js(bs[i].idProof)<<",\"reason\":"<<js(bs[i].reason)<<"}";}cout<<"]";}
 else if(action=="blacklist-add"){if(argc<5){cout<<"{\"ok\":false}";return 0;}bs.push_back({arg(2),arg(3),arg(4)});saveB(bs);cout<<"{\"ok\":true}";}
 else if(action=="blacklist-remove"){string id=arg(2);auto n=bs.size();bs.erase(remove_if(bs.begin(),bs.end(),[&](auto&e){return e.idProof==id;}),bs.end());saveB(bs);cout<<"{\"ok\":"<<(bs.size()<n?"true":"false")<<"}";}
 else if(action=="export"){ofstream f(root+"/visitor_report.csv");f<<"ID,Name,Phone,Email,ID Proof,Purpose,Host,Status,Check In,Check Out\n";for(auto&v:vs){auto q=[](string s){string o="\"";for(char c:s){if(c=='\"')o+='\"';o+=c;}return o+"\"";};f<<v.id<<","<<q(v.name)<<","<<q(v.phone)<<","<<q(v.email)<<","<<q(v.idProof)<<","<<q(v.purpose)<<","<<q(v.host)<<","<<q(v.status)<<","<<q(v.checkIn)<<","<<q(v.checkOut)<<"\n";}cout<<"{\"ok\":true,\"file\":\"data/visitor_report.csv\"}";}
 else cout<<"{\"ok\":false,\"message\":\"Unknown action\"}";
}
