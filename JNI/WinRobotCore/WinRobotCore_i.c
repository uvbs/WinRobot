

/* this ALWAYS GENERATED file contains the IIDs and CLSIDs */

/* link this file in with the server and any clients */


 /* File created by MIDL compiler version 7.00.0500 */
/* at Mon Mar 26 23:40:21 2012
 */
/* Compiler settings for .\WinRobotCore.idl:
    Oicf, W1, Zp8, env=Win64 (32b run)
    protocol : dce , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
//@@MIDL_FILE_HEADING(  )

#pragma warning( disable: 4049 )  /* more than 64k source lines */


#ifdef __cplusplus
extern "C"{
#endif 


#include <rpc.h>
#include <rpcndr.h>

#ifdef _MIDL_USE_GUIDDEF_

#ifndef INITGUID
#define INITGUID
#include <guiddef.h>
#undef INITGUID
#else
#include <guiddef.h>
#endif

#define MIDL_DEFINE_GUID(type,name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8) \
        DEFINE_GUID(name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8)

#else // !_MIDL_USE_GUIDDEF_

#ifndef __IID_DEFINED__
#define __IID_DEFINED__

typedef struct _IID
{
    unsigned long x;
    unsigned short s1;
    unsigned short s2;
    unsigned char  c[8];
} IID;

#endif // __IID_DEFINED__

#ifndef CLSID_DEFINED
#define CLSID_DEFINED
typedef IID CLSID;
#endif // CLSID_DEFINED

#define MIDL_DEFINE_GUID(type,name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8) \
        const type name = {l,w1,w2,{b1,b2,b3,b4,b5,b6,b7,b8}}

#endif !_MIDL_USE_GUIDDEF_

MIDL_DEFINE_GUID(IID, IID_IWinRobotSession,0x9AB9A753,0xB8A3,0x4E01,0xAC,0x4B,0x99,0x5B,0x4B,0x34,0xEF,0x13);


MIDL_DEFINE_GUID(IID, IID_IWinRobotService,0x43CF023A,0x99C4,0x4867,0xAA,0x3F,0xEA,0x49,0x0C,0xAC,0xE6,0x93);


MIDL_DEFINE_GUID(IID, IID_IScreenBufferStream,0xE4CC963A,0x2692,0x4A7A,0x8C,0x73,0xD9,0xDF,0x7C,0x47,0x01,0x3A);


MIDL_DEFINE_GUID(IID, LIBID_WinRobotCoreLib,0x2833AC19,0x456E,0x46CF,0xA8,0xE8,0xA5,0x99,0xFF,0xC5,0xDE,0x1A);


MIDL_DEFINE_GUID(CLSID, CLSID_WinRobotSession,0x80AD2243,0xBBC8,0x442B,0xAA,0x55,0x14,0x63,0x3E,0x6F,0xE1,0x7B);


MIDL_DEFINE_GUID(CLSID, CLSID_WinRobotService,0xD08018BD,0x6958,0x4A2E,0x95,0xEA,0xFE,0xC1,0x32,0x11,0xDA,0x0F);


MIDL_DEFINE_GUID(CLSID, CLSID_ScreenBufferStream,0xC24870C2,0x8829,0x4BEF,0xBC,0xBE,0x90,0x64,0xAB,0x81,0x59,0x9E);

#undef MIDL_DEFINE_GUID

#ifdef __cplusplus
}
#endif



