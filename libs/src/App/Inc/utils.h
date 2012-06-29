#ifndef UTILS_H

#define UTILS_H


u8  XorSum(u8 *pBuf, u16 nLen);
u32 CRC32(u8 *pBuf, u16 nSize);
u16 modrtuChecksum ( u8 *chkbuf, u8 len );
#define DEBUG_LOG(type,message)  do { if (type) printf message;} while (0)

#endif
