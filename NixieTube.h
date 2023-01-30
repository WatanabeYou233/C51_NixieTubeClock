#ifndef __Nixie__
#define __Nixie__
void NixieTube_FlowTenHour(unsigned char e,unsigned char f,unsigned char g);
void NixieTube_FlowHour(unsigned char e,unsigned char f,unsigned char g);
void NixieTube_FlowTenMin(unsigned char e,unsigned char f,unsigned char g);
void NixieTube_FlowMin(unsigned char e,unsigned char f,unsigned char g);
void NixieTube_Flag();

extern unsigned char xdata Nixie_Flag;

#endif