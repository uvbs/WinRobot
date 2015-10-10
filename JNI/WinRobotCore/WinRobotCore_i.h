

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


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


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 475
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif // __RPCNDR_H_VERSION__

#ifndef COM_NO_WINDOWS_H
#include "windows.h"
#include "ole2.h"
#endif /*COM_NO_WINDOWS_H*/

#ifndef __WinRobotCore_i_h__
#define __WinRobotCore_i_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IWinRobotSession_FWD_DEFINED__
#define __IWinRobotSession_FWD_DEFINED__
typedef interface IWinRobotSession IWinRobotSession;
#endif 	/* __IWinRobotSession_FWD_DEFINED__ */


#ifndef __IWinRobotService_FWD_DEFINED__
#define __IWinRobotService_FWD_DEFINED__
typedef interface IWinRobotService IWinRobotService;
#endif 	/* __IWinRobotService_FWD_DEFINED__ */


#ifndef __IScreenBufferStream_FWD_DEFINED__
#define __IScreenBufferStream_FWD_DEFINED__
typedef interface IScreenBufferStream IScreenBufferStream;
#endif 	/* __IScreenBufferStream_FWD_DEFINED__ */


#ifndef __WinRobotSession_FWD_DEFINED__
#define __WinRobotSession_FWD_DEFINED__

#ifdef __cplusplus
typedef class WinRobotSession WinRobotSession;
#else
typedef struct WinRobotSession WinRobotSession;
#endif /* __cplusplus */

#endif 	/* __WinRobotSession_FWD_DEFINED__ */


#ifndef __WinRobotService_FWD_DEFINED__
#define __WinRobotService_FWD_DEFINED__

#ifdef __cplusplus
typedef class WinRobotService WinRobotService;
#else
typedef struct WinRobotService WinRobotService;
#endif /* __cplusplus */

#endif 	/* __WinRobotService_FWD_DEFINED__ */


#ifndef __ScreenBufferStream_FWD_DEFINED__
#define __ScreenBufferStream_FWD_DEFINED__

#ifdef __cplusplus
typedef class ScreenBufferStream ScreenBufferStream;
#else
typedef struct ScreenBufferStream ScreenBufferStream;
#endif /* __cplusplus */

#endif 	/* __ScreenBufferStream_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

#ifdef __cplusplus
extern "C"{
#endif 


#ifndef __IWinRobotSession_INTERFACE_DEFINED__
#define __IWinRobotSession_INTERFACE_DEFINED__

/* interface IWinRobotSession */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IWinRobotSession;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("9AB9A753-B8A3-4E01-AC4B-995B4B34EF13")
    IWinRobotSession : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE CreateScreenCapture( 
            /* [in] */ LONG nX,
            /* [in] */ LONG nY,
            /* [in] */ LONG nWidth,
            /* [in] */ LONG nHeight,
            /* [retval][out] */ IUnknown **ppBufferedImage) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Delay( 
            /* [in] */ ULONG ms) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_AutoDelay( 
            /* [retval][out] */ ULONG *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_AutoDelay( 
            /* [in] */ ULONG newVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetPixelColor( 
            /* [in] */ LONG x,
            /* [in] */ LONG y,
            /* [retval][out] */ OLE_COLOR *color) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_AutoWaitForIdle( 
            /* [retval][out] */ VARIANT_BOOL *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_AutoWaitForIdle( 
            /* [in] */ VARIANT_BOOL newVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE WaitForIdle( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE KeyPress( 
            /* [in] */ ULONG keycode) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE KeyRelease( 
            /* [in] */ ULONG keycode) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE MouseMove( 
            /* [in] */ LONG x,
            /* [in] */ LONG y) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE MousePress( 
            /* [in] */ ULONG buttons) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE MouseRelease( 
            /* [in] */ ULONG buttons) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE MouseWheel( 
            /* [in] */ LONG wheelAmt) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE CaptureMouseCursor( 
            /* [out] */ LONG *x,
            /* [out] */ LONG *y,
            /* [out] */ LONG *hotx,
            /* [out] */ LONG *hoty,
            /* [out] */ LONG *type,
            /* [retval][out] */ IUnknown **pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE InputChar( 
            /* [in] */ USHORT uchar) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetKeyboardLayout( 
            /* [in] */ BSTR KLID) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IWinRobotSessionVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IWinRobotSession * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IWinRobotSession * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IWinRobotSession * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IWinRobotSession * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IWinRobotSession * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IWinRobotSession * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IWinRobotSession * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *CreateScreenCapture )( 
            IWinRobotSession * This,
            /* [in] */ LONG nX,
            /* [in] */ LONG nY,
            /* [in] */ LONG nWidth,
            /* [in] */ LONG nHeight,
            /* [retval][out] */ IUnknown **ppBufferedImage);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Delay )( 
            IWinRobotSession * This,
            /* [in] */ ULONG ms);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_AutoDelay )( 
            IWinRobotSession * This,
            /* [retval][out] */ ULONG *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_AutoDelay )( 
            IWinRobotSession * This,
            /* [in] */ ULONG newVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetPixelColor )( 
            IWinRobotSession * This,
            /* [in] */ LONG x,
            /* [in] */ LONG y,
            /* [retval][out] */ OLE_COLOR *color);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_AutoWaitForIdle )( 
            IWinRobotSession * This,
            /* [retval][out] */ VARIANT_BOOL *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_AutoWaitForIdle )( 
            IWinRobotSession * This,
            /* [in] */ VARIANT_BOOL newVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *WaitForIdle )( 
            IWinRobotSession * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *KeyPress )( 
            IWinRobotSession * This,
            /* [in] */ ULONG keycode);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *KeyRelease )( 
            IWinRobotSession * This,
            /* [in] */ ULONG keycode);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *MouseMove )( 
            IWinRobotSession * This,
            /* [in] */ LONG x,
            /* [in] */ LONG y);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *MousePress )( 
            IWinRobotSession * This,
            /* [in] */ ULONG buttons);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *MouseRelease )( 
            IWinRobotSession * This,
            /* [in] */ ULONG buttons);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *MouseWheel )( 
            IWinRobotSession * This,
            /* [in] */ LONG wheelAmt);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *CaptureMouseCursor )( 
            IWinRobotSession * This,
            /* [out] */ LONG *x,
            /* [out] */ LONG *y,
            /* [out] */ LONG *hotx,
            /* [out] */ LONG *hoty,
            /* [out] */ LONG *type,
            /* [retval][out] */ IUnknown **pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *InputChar )( 
            IWinRobotSession * This,
            /* [in] */ USHORT uchar);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetKeyboardLayout )( 
            IWinRobotSession * This,
            /* [in] */ BSTR KLID);
        
        END_INTERFACE
    } IWinRobotSessionVtbl;

    interface IWinRobotSession
    {
        CONST_VTBL struct IWinRobotSessionVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IWinRobotSession_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IWinRobotSession_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IWinRobotSession_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IWinRobotSession_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IWinRobotSession_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IWinRobotSession_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IWinRobotSession_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IWinRobotSession_CreateScreenCapture(This,nX,nY,nWidth,nHeight,ppBufferedImage)	\
    ( (This)->lpVtbl -> CreateScreenCapture(This,nX,nY,nWidth,nHeight,ppBufferedImage) ) 

#define IWinRobotSession_Delay(This,ms)	\
    ( (This)->lpVtbl -> Delay(This,ms) ) 

#define IWinRobotSession_get_AutoDelay(This,pVal)	\
    ( (This)->lpVtbl -> get_AutoDelay(This,pVal) ) 

#define IWinRobotSession_put_AutoDelay(This,newVal)	\
    ( (This)->lpVtbl -> put_AutoDelay(This,newVal) ) 

#define IWinRobotSession_GetPixelColor(This,x,y,color)	\
    ( (This)->lpVtbl -> GetPixelColor(This,x,y,color) ) 

#define IWinRobotSession_get_AutoWaitForIdle(This,pVal)	\
    ( (This)->lpVtbl -> get_AutoWaitForIdle(This,pVal) ) 

#define IWinRobotSession_put_AutoWaitForIdle(This,newVal)	\
    ( (This)->lpVtbl -> put_AutoWaitForIdle(This,newVal) ) 

#define IWinRobotSession_WaitForIdle(This)	\
    ( (This)->lpVtbl -> WaitForIdle(This) ) 

#define IWinRobotSession_KeyPress(This,keycode)	\
    ( (This)->lpVtbl -> KeyPress(This,keycode) ) 

#define IWinRobotSession_KeyRelease(This,keycode)	\
    ( (This)->lpVtbl -> KeyRelease(This,keycode) ) 

#define IWinRobotSession_MouseMove(This,x,y)	\
    ( (This)->lpVtbl -> MouseMove(This,x,y) ) 

#define IWinRobotSession_MousePress(This,buttons)	\
    ( (This)->lpVtbl -> MousePress(This,buttons) ) 

#define IWinRobotSession_MouseRelease(This,buttons)	\
    ( (This)->lpVtbl -> MouseRelease(This,buttons) ) 

#define IWinRobotSession_MouseWheel(This,wheelAmt)	\
    ( (This)->lpVtbl -> MouseWheel(This,wheelAmt) ) 

#define IWinRobotSession_CaptureMouseCursor(This,x,y,hotx,hoty,type,pVal)	\
    ( (This)->lpVtbl -> CaptureMouseCursor(This,x,y,hotx,hoty,type,pVal) ) 

#define IWinRobotSession_InputChar(This,uchar)	\
    ( (This)->lpVtbl -> InputChar(This,uchar) ) 

#define IWinRobotSession_SetKeyboardLayout(This,KLID)	\
    ( (This)->lpVtbl -> SetKeyboardLayout(This,KLID) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IWinRobotSession_INTERFACE_DEFINED__ */


#ifndef __IWinRobotService_INTERFACE_DEFINED__
#define __IWinRobotService_INTERFACE_DEFINED__

/* interface IWinRobotService */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IWinRobotService;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("43CF023A-99C4-4867-AA3F-EA490CACE693")
    IWinRobotService : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetActiveConsoleSession( 
            /* [retval][out] */ IUnknown **ppSession) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE RegSession( 
            /* [in] */ ULONG sid,
            /* [in] */ ULONG processid,
            /* [in] */ IUnknown *pSession) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IWinRobotServiceVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IWinRobotService * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IWinRobotService * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IWinRobotService * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IWinRobotService * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IWinRobotService * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IWinRobotService * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IWinRobotService * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetActiveConsoleSession )( 
            IWinRobotService * This,
            /* [retval][out] */ IUnknown **ppSession);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *RegSession )( 
            IWinRobotService * This,
            /* [in] */ ULONG sid,
            /* [in] */ ULONG processid,
            /* [in] */ IUnknown *pSession);
        
        END_INTERFACE
    } IWinRobotServiceVtbl;

    interface IWinRobotService
    {
        CONST_VTBL struct IWinRobotServiceVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IWinRobotService_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IWinRobotService_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IWinRobotService_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IWinRobotService_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IWinRobotService_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IWinRobotService_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IWinRobotService_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IWinRobotService_GetActiveConsoleSession(This,ppSession)	\
    ( (This)->lpVtbl -> GetActiveConsoleSession(This,ppSession) ) 

#define IWinRobotService_RegSession(This,sid,processid,pSession)	\
    ( (This)->lpVtbl -> RegSession(This,sid,processid,pSession) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IWinRobotService_INTERFACE_DEFINED__ */


#ifndef __IScreenBufferStream_INTERFACE_DEFINED__
#define __IScreenBufferStream_INTERFACE_DEFINED__

/* interface IScreenBufferStream */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IScreenBufferStream;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("E4CC963A-2692-4A7A-8C73-D9DF7C47013A")
    IScreenBufferStream : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Size( 
            /* [retval][out] */ ULONG *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_FileMappingName( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IScreenBufferStreamVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IScreenBufferStream * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IScreenBufferStream * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IScreenBufferStream * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IScreenBufferStream * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IScreenBufferStream * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IScreenBufferStream * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IScreenBufferStream * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Size )( 
            IScreenBufferStream * This,
            /* [retval][out] */ ULONG *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_FileMappingName )( 
            IScreenBufferStream * This,
            /* [retval][out] */ BSTR *pVal);
        
        END_INTERFACE
    } IScreenBufferStreamVtbl;

    interface IScreenBufferStream
    {
        CONST_VTBL struct IScreenBufferStreamVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IScreenBufferStream_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IScreenBufferStream_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IScreenBufferStream_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IScreenBufferStream_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IScreenBufferStream_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IScreenBufferStream_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IScreenBufferStream_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IScreenBufferStream_get_Size(This,pVal)	\
    ( (This)->lpVtbl -> get_Size(This,pVal) ) 

#define IScreenBufferStream_get_FileMappingName(This,pVal)	\
    ( (This)->lpVtbl -> get_FileMappingName(This,pVal) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IScreenBufferStream_INTERFACE_DEFINED__ */



#ifndef __WinRobotCoreLib_LIBRARY_DEFINED__
#define __WinRobotCoreLib_LIBRARY_DEFINED__

/* library WinRobotCoreLib */
/* [helpstring][version][uuid] */ 

typedef /* [v1_enum] */ 
enum _InputEvent
    {	BUTTON1_MASK	= 16,
	BUTTON2_MASK	= 8,
	BUTTON3_MASK	= 4
    } 	InputEvent;

typedef /* [v1_enum] */ 
enum _CursorType
    {	CURSOR_APPSTARTING	= 0,
	CURSOR_ARROW	= ( CURSOR_APPSTARTING + 1 ) ,
	CURSOR_CROSS	= ( CURSOR_ARROW + 1 ) ,
	CURSOR_HAND	= ( CURSOR_CROSS + 1 ) ,
	CURSOR_HELP	= ( CURSOR_HAND + 1 ) ,
	CURSOR_IBEAM	= ( CURSOR_HELP + 1 ) ,
	CURSOR_ICON	= ( CURSOR_IBEAM + 1 ) ,
	CURSOR_NO	= ( CURSOR_ICON + 1 ) ,
	CURSOR_SIZE	= ( CURSOR_NO + 1 ) ,
	CURSOR_SIZEALL	= ( CURSOR_SIZE + 1 ) ,
	CURSOR_SIZENESW	= ( CURSOR_SIZEALL + 1 ) ,
	CURSOR_SIZENS	= ( CURSOR_SIZENESW + 1 ) ,
	CURSOR_SIZENWSE	= ( CURSOR_SIZENS + 1 ) ,
	CURSOR_SIZEWE	= ( CURSOR_SIZENWSE + 1 ) ,
	CURSOR_UPARROW	= ( CURSOR_SIZEWE + 1 ) ,
	CURSOR_WAIT	= ( CURSOR_UPARROW + 1 ) ,
	CURSOR_UNKNOWN	= -1
    } 	CursorType;


EXTERN_C const IID LIBID_WinRobotCoreLib;

EXTERN_C const CLSID CLSID_WinRobotSession;

#ifdef __cplusplus

class DECLSPEC_UUID("80AD2243-BBC8-442B-AA55-14633E6FE17B")
WinRobotSession;
#endif

EXTERN_C const CLSID CLSID_WinRobotService;

#ifdef __cplusplus

class DECLSPEC_UUID("D08018BD-6958-4A2E-95EA-FEC13211DA0F")
WinRobotService;
#endif

EXTERN_C const CLSID CLSID_ScreenBufferStream;

#ifdef __cplusplus

class DECLSPEC_UUID("C24870C2-8829-4BEF-BCBE-9064AB81599E")
ScreenBufferStream;
#endif
#endif /* __WinRobotCoreLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

unsigned long             __RPC_USER  BSTR_UserSize(     unsigned long *, unsigned long            , BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserMarshal(  unsigned long *, unsigned char *, BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserUnmarshal(unsigned long *, unsigned char *, BSTR * ); 
void                      __RPC_USER  BSTR_UserFree(     unsigned long *, BSTR * ); 

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


