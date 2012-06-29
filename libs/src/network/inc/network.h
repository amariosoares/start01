#ifndef NETWORK_H
#define NETWORK_H
#include "debugcomm.h"
void network_srv(void);
u32 network_send_data(TDataChan chan,u8* data, u32 len);
u32 udp_send_data(u8* data, u32 len);
void network_set_ok(u8 ok);
#endif
