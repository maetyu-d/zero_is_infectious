#pragma once
#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <map>
#include <string>
#include <vector>
namespace zero {
constexpr int Void=99;
struct Random { uint32_t seed; double operator()(){uint32_t t=(seed+=0x6D2B79F5u);t=(t^(t>>15))*(t|1);t^=t+(t^(t>>7))*(t|61);return double(t^(t>>14))/4294967296.0;} };
struct Profile {int turns,actions,zeros;double ratio,cap;std::array<int,4> weather;std::array<int,3> mutation;std::array<int,2> geometry;std::vector<int> flux;int fluxCount;};
inline Profile profile(int n){switch(n){case 5:return {9,2,1,.52,.28,{2,3,5,6},{4,6,8},{3,6},{},0};case 7:return {11,3,1,.54,.26,{3,5,7,8},{5,8,10},{4,8},{},0};case 11:return {14,4,3,.52,.24,{4,7,9,10},{6,10,13},{5,10},{5,9},1};case 13:return {16,5,4,.50,.23,{4,8,11,12},{7,11,15},{5,10},{5,9,12},2};default:return {12,3,2,.52,.25,{3,6,8,9},{5,8,11},{4,8},{},0};}}
struct Cell {int v=0,anchor=0;};
struct Counts {int living=0,zeros=0,voids=0;};
struct Game {
 int size=9,initial=9,difficulty=1,turn=1,ap=3,apMax=3,streak=0,stable=-1,quiet=0,pressure=0,cooldown=0,births=0,phase=0;uint32_t seed=129741;bool dynamic=false,worldSize=false;Profile p=profile(9);std::vector<Cell> cells;std::map<std::pair<int,int>,int> walls;std::vector<std::string> log;std::string reason;
 int actions()const{return std::clamp(profile(size).actions+(difficulty==0?1:difficulty==2&&size>=7?-1:0),2,6);}
 int minLiving()const{return std::max(6,int(std::ceil(size*size*(profile(size).ratio+(difficulty==0?-.065:difficulty==2?.07:0)))));}
 int maxZeros()const{return std::max(5,int(std::ceil(size*size*(profile(size).cap+(difficulty==0?.045:difficulty==2?-.04:0)))));}
 Counts count()const{Counts c;for(auto x:cells){if(x.v==Void)c.voids++;else if(x.v==0)c.zeros++;else c.living++;}return c;}
 int distance(int a,int b)const{return std::abs(a%size-b%size)+std::abs(a/size-b/size);}
 static std::pair<int,int> edge(int a,int b){return {std::min(a,b),std::max(a,b)};}
 Cell randomCell(Random& r,int n)const{int low=n>=11?3:n>=7?2:1;int m=std::clamp(low+int(std::pow(r(),.77)*(10-low)),1,9);int sign=r()<.51?1:-1;return {sign*(difficulty==2?std::max(1,m-1):m),0};}
 Game(uint32_t s=129741,int n=9,int d=1,bool dyn=false,bool ws=false):size(n),initial(n),difficulty(d),seed(s),dynamic(dyn),worldSize(ws),p(profile(n)){
 Random r{s};for(int i=0;i<n*n;i++)cells.push_back(randomCell(r,n));int c=n/2,core=c+c*n;cells[core].v=0;int k=d==0?4:d==2?2:3;cells[core-n].v=k;cells[core+1].v=-k;cells[core+n].v=k+1;cells[core-1].v=-k;
 std::vector<std::pair<double,int>> candidates;for(int i=0;i<n*n;i++)if(i!=core)candidates.push_back({r(),i});std::sort(candidates.begin(),candidates.end());std::vector<int> used{core};int total=p.zeros+(d==2?1:0)-(d==0&&n>=9?1:0);
 for(auto q:candidates){if(int(used.size())>=total)break;int i=q.second,x=i%n,y=i/n;if(x==0||y==0||x==n-1||y==n-1)continue;bool near=false;for(int j:used)if(distance(i,j)<std::max(3,int(n*.38)))near=true;if(near)continue;used.push_back(i);cells[i].v=0;for(int j:{i-1,i+1,i-n,i+n})if(cells[j].v!=0&&std::abs(cells[j].v)<3)cells[j].v=cells[j].v>0?3:-3;}
 ap=apMax=actions();log.push_back("World "+std::to_string(seed)+" / "+std::to_string(n)+" x "+std::to_string(n));}
 bool mutated(int op)const{return (op==2&&turn>=p.mutation[0])||(op==0&&turn>=p.mutation[1])||(op==3&&turn>=p.mutation[2]);}
 int weather()const{for(int i=0;i<4;i++)if(p.weather[i]==turn)return i;return -1;}
 bool affected(int w,int i)const{return w==0||w==2||(w==1&&std::abs(i/size-size/2)<=1)||(w==3&&i%size==int(seed%size));}
 std::vector<int> satellites()const{int k=std::max(0,p.fluxCount+(difficulty==2?1:difficulty==0?-1:0));if(!k||std::find(p.flux.begin(),p.flux.end(),turn)==p.flux.end())return {};Random r{seed^(uint32_t(turn)*98791u)^(uint32_t(initial)*4327u)};std::vector<std::pair<double,int>> list;for(int y=1;y<size-1;y++)for(int x=1;x<size-1;x++){if((dynamic||worldSize)&&(x<2||y<2||x>=size-2||y>=size-2))continue;list.push_back({r(),x+y*size});}std::sort(list.begin(),list.end());std::vector<int> out;for(auto q:list){bool ok=true;for(int i:out)if(distance(i,q.second)<std::max(3,int(size*.32)))ok=false;if(ok)out.push_back(q.second);if(int(out.size())>=k)break;}return out;}
 bool valid(int op,int src,int to)const{if(to<0||to>=size*size||cells[to].v==Void||src<0)return false;if(op==0)return src!=to&&distance(src,to)<=2&&std::abs(cells[src].v)>=2&&std::abs(cells[to].v)<9;if(op==1)return distance(src,to)==1;if(op==3)return distance(src,to)==1&&!walls.count(edge(src,to));return false;}
 bool perform(int op,int to,int src=-1){int cost=op==4?2:1;if(phase||ap<cost||to<0||to>=size*size||cells[to].v==Void)return false;if((op==0||op==1||op==3)&&!valid(op,src,to))return false;auto& c=cells[to];if(op==2&&c.v==0)return false;if(op==5&&c.v!=0)return false;
 if(op==0){auto& a=cells[src];int sign=a.v>0?1:-1;a.v=sign*(std::abs(a.v)-(mutated(op)?2:1));c.v=c.v==0?sign:(c.v>0?1:-1)*(std::abs(c.v)+1);c.anchor=0;if(a.v==0)a.anchor=0;}if(op==1)std::swap(c,cells[src]);if(op==2){c.v=mutated(op)&&std::abs(c.v)==1?0:-c.v;c.anchor=0;}if(op==3)walls[edge(src,to)]=mutated(op)?1:2;if(op==4)c={Void,0};if(op==5)c.anchor=difficulty==2?2:3;ap-=cost;return true;}
 void tick(){auto before=cells;std::vector<int> attacks(size*size);auto active=[&](int x,int y){return x>=0&&y>=0&&x<size&&y<size&&before[x+y*size].v==0&&before[x+y*size].anchor==0;};auto hit=[&](int a,int b){if(b>=0&&b<size*size&&cells[b].v!=Void&&cells[b].v!=0&&!walls.count(edge(a,b)))attacks[b]++;};
 for(int y=0;y<size;y++)for(int x=0;x<size;x++)if(active(x,y)){int i=x+y*size;for(auto d:std::array<std::pair<int,int>,4>{{{1,0},{-1,0},{0,1},{0,-1}}}){int nx=x+d.first,ny=y+d.second;if(nx>=0&&ny>=0&&nx<size&&ny<size)hit(i,nx+ny*size);}}
 for(int y=0;y<size;y++)for(int x=0;x<size;x++)for(auto d:std::array<std::pair<int,int>,2>{{{1,0},{0,1}}}){int dx=d.first,dy=d.second;if(!active(x,y)||!active(x+dx,y+dy)||!active(x+2*dx,y+2*dy))continue;for(int end:{0,2}){int sign=end==0?-1:1,xx=x+end*dx,yy=y+end*dy,mx=xx+sign*dx,my=yy+sign*dy,tx=xx+2*sign*dx,ty=yy+2*sign*dy;if(tx<0||ty<0||tx>=size||ty>=size)continue;int a=xx+yy*size,b=mx+my*size,c=tx+ty*size;if(cells[b].v!=Void&&!walls.count(edge(a,b))&&!walls.count(edge(b,c)))hit(b,c);}}
 for(int y=0;y<size-1;y++)for(int x=0;x<size-1;x++)if(active(x,y)&&active(x+1,y)&&active(x,y+1)&&active(x+1,y+1))for(auto d:std::array<std::pair<int,int>,4>{{{-1,-1},{2,-1},{-1,2},{2,2}}}){int tx=x+d.first,ty=y+d.second;if(tx>=0&&ty>=0&&tx<size&&ty<size){int i=tx+ty*size;if(cells[i].v!=0&&cells[i].v!=Void)attacks[i]++;}}
 for(int i=0;i<size*size;i++){auto& c=cells[i];if(attacks[i]&&c.v!=0&&c.v!=Void)c.v=(c.v>0?1:-1)*std::max(0,std::abs(c.v)-(c.v>0?attacks[i]:std::max(1,attacks[i]-1)));if(c.anchor)c.anchor--;}
 for(auto it=walls.begin();it!=walls.end();)if(--it->second<=0)it=walls.erase(it);else ++it;
 int w=weather();for(int i=0;i<size*size;i++){auto& c=cells[i];if(w<0||!affected(w,i)||c.v==Void||c.v==0)continue;if(w==0)c.v=-c.v;if(w==1)c.v=std::clamp(c.v-1,-9,9);if(w==2)c.v=(c.v>0?1:-1)*((std::abs(c.v)+1)/2);if(w==3)c.v=(c.v>0?1:-1)*(std::abs(c.v)%5);}
 for(int i:satellites())if(cells[i].v!=Void&&cells[i].v!=0)cells[i]={0,0};births=0;for(int i=0;i<size*size;i++)if(cells[i].v==0&&before[i].v!=0)births++;}
 void resize(int n){int old=size,off=(n-old)/2;Random r{seed+uint32_t(turn*991+n*37+old*13)};std::vector<Cell> fresh;for(int i=0;i<n*n;i++)fresh.push_back(randomCell(r,n));auto remap=[&](int i){int x=i%old+off,y=i/old+off;return x<0||y<0||x>=n||y>=n?-1:x+y*n;};for(int i=0;i<old*old;i++)if(remap(i)>=0)fresh[remap(i)]=cells[i];std::map<std::pair<int,int>,int> ws;for(auto w:walls){int a=remap(w.first.first),b=remap(w.first.second);if(a>=0&&b>=0)ws[edge(a,b)]=w.second;}if(stable>=0)stable=remap(stable);cells=fresh;walls=ws;size=n;quiet=pressure=0;cooldown=2;apMax=actions();}
 void geometry(){bool cooling=cooldown>0;if(cooling)cooldown--;if(dynamic){for(int i=0;i<2;i++)if(turn==p.geometry[i]){bool grow=(i==0)==(initial<13);int n=std::clamp(size+(grow?2:-2),5,13);if(n!=size){resize(n);return;}}}if(!worldSize)return;auto c=count();pressure=c.zeros>=std::max(3,int(std::ceil(size*.36)))?pressure+1:0;quiet=c.zeros==1&&c.living>=std::ceil(size*size*.58)?quiet+1:0;if(cooling)return;if(pressure>=2&&size>5)resize(size-2);else if(quiet>=3&&size<13)resize(size+2);}
 void resolve(){if(phase)return;tick();geometry();auto c=count();if(turn>=p.turns-2){int ix=-1;for(int i=0;i<size*size;i++)if(cells[i].v==0){ix=i;break;}if(c.zeros==1&&(stable<0||stable==ix)){streak++;stable=ix;}else streak=0;}if(c.living<minLiving()){phase=2;reason="Too few survivors.";}else if(c.zeros>=maxZeros()){phase=2;reason="The zero colony exceeded its limit.";}else if(turn>=p.turns-2&&streak!=turn-(p.turns-2)+1){phase=2;reason="Preserve the same single zero for the final three turns.";}else if(turn>=p.turns){phase=streak==3&&(!worldSize||size>=std::max(5,initial-2))?1:2;reason=phase==1?"One zero. The world survived.":"The world became too small.";}log.insert(log.begin(),"T"+std::to_string(turn)+" / "+std::to_string(births)+" new zeros");if(log.size()>6)log.resize(6);if(!phase){turn++;ap=apMax=actions();}}
};
}
