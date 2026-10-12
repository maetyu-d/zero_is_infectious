#ifdef NDEBUG
#undef NDEBUG
#endif
#include "Engine.h"
#include <cassert>
#include <iostream>
int main(){using namespace zero;Game a;Game b;assert(a.cells.size()==81&&a.count().zeros==2);for(int i=0;i<81;i++)assert(a.cells[i].v==b.cells[i].v);Game g(1,5);for(auto& c:g.cells)c={9,0};g.cells[12]={0,0};g.cells[13]={-2,0};g.tick();assert(g.cells[11].v==8&&g.cells[13].v==-1&&g.cells[12].v==0);g=Game(1,5);for(auto& c:g.cells)c={9,0};g.cells[12]={0,3};g.tick();assert(g.cells[11].v==9&&g.cells[12].anchor==2);assert(g.perform(0,12,11));assert(g.cells[12].v==1&&g.cells[11].v==8);g=Game(1,9);g.dynamic=true;g.turn=4;g.geometry();assert(g.size==11);g.turn=8;g.geometry();assert(g.size==9);g=Game(1,5);for(auto& c:g.cells)c={9,0};g.cells[12]={0,3};g.turn=7;for(int i=0;i<3;i++)g.resolve();assert(g.phase==1&&g.streak==3);g=Game(1,5);for(auto& c:g.cells)c={0,0};g.resolve();assert(g.phase==2);for(int n:{5,7,9,11,13})for(int d=0;d<3;d++){Game s(129741,n,d,true,true);for(int t=0;t<20&&!s.phase;t++){s.resolve();assert(int(s.cells.size())==s.size*s.size);for(auto c:s.cells)assert(c.v==Void||(c.v>=-9&&c.v<=9));}}g=Game(1,5);for(auto& c:g.cells)c={9,0};g.cells[12]={0,0};assert(g.perform(3,13,12));g.tick();assert(g.cells[13].v==9&&g.walls.size()==1);g.tick();assert(g.cells[13].v==9&&g.walls.empty());g.tick();assert(g.cells[13].v==8);
g=Game(1,5);g.turn=4;g.cells[0]={1,0};assert(g.perform(2,0));assert(g.cells[0].v==0);
g=Game(1,5);g.turn=6;g.cells[0]={2,0};g.cells[1]={0,3};assert(g.perform(0,1,0));assert(g.cells[0].v==0&&g.cells[1].v==1&&g.cells[1].anchor==0);
g=Game(1,5);int ap=g.ap;assert(g.perform(4,0));assert(g.cells[0].v==Void&&g.ap==ap-2);assert(!g.perform(5,0));
std::cout<<"Engine tests passed\n";}
