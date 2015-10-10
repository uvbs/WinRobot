/*! \file $Id: GDIHook.cpp 59 2011-06-05 15:58:56Z caoym $
*  \author caoym
*  \brief CGDIHook
*/
#include "StdAfx.h"
#include "GDIHook.h"
#include <tchar.h>
#include "DDrawHook.h"
#include "ShareData.h"

const UINT DEFERRED_RECORD	= RegisterWindowMessage(_T("CE9EB872-FCAC-4569-BB08-53E2DCE2F589"));

bool UninstallGDIHook()
{

	SHARE_DATA*pData  = &g_share_data;
	//if(m_nThis == pData->owner) 
	{
		for (int i=0; i<RTL_NUMBER_OF(pData->hooks); i++)
		{
			if (pData->hooks[i])
			{
				if(!UnhookWindowsHookEx(pData->hooks[i]))
				{
					//DebugOutF(filelog::log_error,"UnhookWindowsHookEx %d,%d failed with %d",i,pData->hooks[i],GetLastError() );
				}
				pData->hooks[i] = 0;
			}
		}
	}
	return true;
}
bool InstallGDIHook(HINSTANCE inst)
{

	SHARE_DATA*pData = &g_share_data;
	/*m_nThis = */InterlockedIncrement(&pData->owner);
	// if hooked by old process,we should uninstall it

	for (int i=0;i<RTL_NUMBER_OF(pData->hooks);i++)
	{
		if (pData->hooks[i])
		{
			if(!UnhookWindowsHookEx(pData->hooks[i]))
			{
	//			DebugOutF(filelog::log_error,"UnhookWindowsHookEx %d,%d failed with %d",i,pData->hooks[i],GetLastError() );
			}
			pData->hooks[i] = 0;
		}
	}
	pData->changes_begin = 
		pData->changes_done_end = 
		pData->changes_writting_end = 0;

	if(!(pData->hooks[WH_CALLWNDPROC] = SetWindowsHookEx(WH_CALLWNDPROC,&HookProc_CallWndProc,inst,0)))
	{
	//	DebugOutF(filelog::log_error,"SetWindowsHookEx WH_CALLWNDPROC failed with %d",GetLastError() );
		UninstallGDIHook();
		return false;
	}
	else
	{
		//DebugOutF(filelog::log_info,"SetWindowsHookEx WH_CALLWNDPROC ok");
	}
	if(!(pData->hooks[WH_GETMESSAGE] = SetWindowsHookEx(WH_GETMESSAGE,&HookProc_GetMessageProc,inst,0)))
	{
		//DebugOutF(filelog::log_error,"SetWindowsHookEx WH_GETMESSAGE failed with %d",GetLastError() );
		UninstallGDIHook();
		return false;
	}
	else
	{
		//DebugOutF(filelog::log_info,"SetWindowsHookEx WH_GETMESSAGE ok");
	}
	if(!(pData->hooks[WH_SYSMSGFILTER] = SetWindowsHookEx(WH_SYSMSGFILTER,&HookProc_DialogMessageProc,inst,0)))
	{
		//DebugOutF(filelog::log_error,"SetWindowsHookEx WH_SYSMSGFILTER failed with %d",GetLastError() );
		UninstallGDIHook();
		return false;
	}
	else
	{
		//DebugOutF(filelog::log_info,"SetWindowsHookEx WH_SYSMSGFILTER ok");
	}
	return true;
}
LRESULT CALLBACK HookProc_CallWndProc(int nCode, WPARAM wParam, LPARAM lParam)
{
 	if (nCode == HC_ACTION){
 		BOOL empty = TRUE;
 		CWPSTRUCT *cwpStruct = (CWPSTRUCT *) lParam;
 
 		HookHandle(
 			cwpStruct->message, 
 			cwpStruct->hwnd, 
 			cwpStruct->wParam, 
 			cwpStruct->lParam);
 	}
	return CallNextHookEx(g_share_data.hooks[WH_CALLWNDPROC], nCode, wParam, lParam) ;
}
LRESULT CALLBACK HookProc_GetMessageProc(int nCode, WPARAM wParam, LPARAM lParam)
{
 	if (nCode == HC_ACTION){
 
 		MSG *msg = (MSG *) lParam;
 		// Only handle application messages if they're being removed:
 		if (wParam & PM_REMOVE){
 
 			HookHandle(
 				msg->message,
 				msg->hwnd, 
 				msg->wParam, 
 				msg->lParam);
 		}
 	}
	return CallNextHookEx(g_share_data.hooks[WH_GETMESSAGE], nCode, wParam, lParam) ;
}
LRESULT CALLBACK HookProc_DialogMessageProc(int nCode, WPARAM wParam, LPARAM lParam)
{
 	if (nCode == HC_ACTION){
 		MSG *msg = (MSG *) lParam;	
 		HookHandle(
 			msg->message,
 			msg->hwnd, 
 			msg->wParam, 
 			msg->lParam);
 	}
	return CallNextHookEx(g_share_data.hooks[WH_SYSMSGFILTER], nCode, wParam, lParam) ;
}
BOOL HookHandle(UINT hookmsg,HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	
	if(DEFERRED_RECORD ==hookmsg )
	{
		RECT rc = {	(SHORT)LOWORD(wParam),
			(SHORT)HIWORD(wParam),
			(SHORT)LOWORD(lParam),
			(SHORT)HIWORD(lParam)};

		RecordUpdateRect(rc);
		return TRUE;
	}
	switch (hookmsg)
	{
	case WM_NCPAINT:
	case WM_NCACTIVATE:
		return RecordDeferredBorderRect(hWnd);
	case WM_HSCROLL:
	case WM_VSCROLL:
		if (((int) LOWORD(wParam) == SB_THUMBTRACK) || ((int) LOWORD(wParam) == SB_ENDSCROLL))
			return RecordDeferredWindowRect(hWnd);
		break;
	case 485:  // HACK to handle popup menus
		{
			return RecordDeferredWindowRect(hWnd);
		}
		break;
		//full window update
	case WM_SYSCOLORCHANGE:
	case WM_PALETTECHANGED:
	case WM_SETTEXT:
	case WM_ENABLE:
	case BM_SETCHECK:
	case BM_SETSTATE:
	case EM_SETSEL:
		//case WM_MENUSELECT:
		return RecordDeferredWindowRect(hWnd);
		break;

		// find out the area that updated
	case WM_PAINT:
		if (1){
			HRGN region;
			region = CreateRectRgn(0, 0, 0, 0);

			if (GetUpdateRgn(hWnd, region, FALSE) != ERROR){
				int buffsize;
				UINT x;
				RGNDATA *buff;
				POINT TopLeft;

				// Get the top-left point of the client area
				TopLeft.x = 0;
				TopLeft.y = 0;
				if (!ClientToScreen(hWnd, &TopLeft))
					break;

				// Get the size of buffer required
				buffsize = GetRegionData(region, 0, 0);
				if (buffsize != 0){
					buff = (RGNDATA *) new BYTE [buffsize];
					if (buff == NULL)
						break;
					// Now get the region data
					if(GetRegionData(region, buffsize, buff)){
						for (x=0; x<(buff->rdh.nCount); x++){
							// Obtain the rectangles from the list
							RECT *urect = (RECT *) (((BYTE *) buff) + sizeof(RGNDATAHEADER) + (x * sizeof(RECT)));
							RecordDeferredUpdateRect(
								hWnd,
								(SHORT) (TopLeft.x + urect->left),
								(SHORT) (TopLeft.y + urect->top),
								(SHORT) (TopLeft.x + urect->right),
								(SHORT) (TopLeft.y + urect->bottom)
								);
						}
					}
					delete [] buff;
				}
			}

			if (region != NULL)
				DeleteObject(region);
		}
		else{
			RECT rcUpdate;
			if (GetUpdateRect(hWnd, &rcUpdate, FALSE) != ERROR){
				POINT TopLeft;

				// Get the top-left point of the client area
				TopLeft.x = 0;
				TopLeft.y = 0;
				if (!ClientToScreen(hWnd, &TopLeft))
					break;
				RecordDeferredUpdateRect(
					hWnd,
					(SHORT) (TopLeft.x + rcUpdate.left),
					(SHORT) (TopLeft.y + rcUpdate.top),
					(SHORT) (TopLeft.x + rcUpdate.right),
					(SHORT) (TopLeft.y + rcUpdate.bottom)
					);
			}
			return TRUE;
		}
		break;

	case WM_WINDOWPOSCHANGING:
		return RecordDeferredWindowRect(hWnd);
		break;

	case WM_WINDOWPOSCHANGED:
		return RecordDeferredWindowRect(hWnd);
		break;
	case WM_DESTROY:

		break;

	}
	return TRUE;
}
BOOL ClientRectToScreen(HWND hwnd, RECT *rect)
{
	POINT topleft;
	topleft.x = 0;
	topleft.y = 0;
	if (!GetClientRect(hwnd, rect))
		return FALSE;
	if (!ClientToScreen(hwnd, &topleft))
		return FALSE;

	rect->left += topleft.x;
	rect->top += topleft.y;
	rect->right += topleft.x;
	rect->bottom += topleft.y;
	return TRUE;
}

void RecordUpdateRect(const RECT&rc)
{
	ULONG pos = InterlockedIncrement(&g_share_data.changes_writting_end);
	g_share_data.changes[(pos-1)%MAX_RECORDS_COUNT] = rc;
	InterlockedIncrement(&g_share_data.changes_done_end);
}
void RecordWindowRect(HWND hWnd)
{
	RECT wrect;

	// Get the rectangle position
	if (IsWindowVisible(hWnd) && GetWindowRect(hWnd, &wrect))
	{
		// Record the position
		RecordUpdateRect(
			wrect
			);
	}
}

BOOL RecordDeferredUpdateRect(HWND hWnd,SHORT left, SHORT top, SHORT right, SHORT bottom)
{
	WPARAM vwParam;
	LPARAM vlParam;

	vwParam = MAKELONG(left, top);
	vlParam = MAKELONG(right, bottom);

	return PostMessage(
		hWnd,
		DEFERRED_RECORD,
		vwParam,
		vlParam
		);
}
BOOL RecordDeferredWindowRect(HWND hWnd)
{
	RECT wrect;

	// Get the rectangle position
	if (IsWindowVisible(hWnd) && GetWindowRect(hWnd, &wrect))
	{
		// Record the position
		return RecordDeferredUpdateRect(hWnd,
			(SHORT) wrect.left,
			(SHORT) wrect.top,
			(SHORT) wrect.right,
			(SHORT) wrect.bottom
			);
	}
	return TRUE;
}
BOOL RecordDeferredBorderRect(HWND hWnd)
{
	RECT wrect;
	RECT crect;

	// Get the rectangle position
	BOOL b1 = TRUE;
	if (IsWindowVisible(hWnd) && GetWindowRect(hWnd, &wrect))
	{
		// Get the client rectangle position
		if (ClientRectToScreen(hWnd, &crect))
		{
			// Record the four border rectangles
			b1 = b1&&RecordDeferredUpdateRect(hWnd,(SHORT) wrect.left, (SHORT) wrect.top, (SHORT) wrect.right, (SHORT) crect.top);
			b1 = b1&&RecordDeferredUpdateRect(hWnd,(SHORT) wrect.left, (SHORT) wrect.top, (SHORT) crect.left, (SHORT) wrect.bottom);
			b1 = b1&&RecordDeferredUpdateRect(hWnd,(SHORT) wrect.left, (SHORT) crect.bottom, (SHORT) wrect.right, (SHORT) wrect.bottom);
			b1 = b1&&RecordDeferredUpdateRect(hWnd,(SHORT) crect.right, (SHORT) wrect.top, (SHORT) wrect.right, (SHORT) wrect.bottom);
		}
	}
	return b1;
}
ULONG GetGDIChangesCount()
{
	ULONG begin = g_share_data.changes_begin;
	ULONG end = g_share_data.changes_done_end;
	if (end - begin >= MAX_RECORDS_COUNT) // too many changes
	{
		return MAX_RECORDS_COUNT;
	}
	return end - begin;
}
ULONG GetGDIChanges(RECT*pDest,ULONG size)
{
	if(pDest == 0 || size == 0 ) return 0;
	ULONG begin = g_share_data.changes_begin;
	ULONG end = g_share_data.changes_done_end;

	if (end - begin >= MAX_RECORDS_COUNT) // too many changes
	{
		InterlockedExchange(&g_share_data.changes_begin,end);
		RECT rc = {0,0,0x7FFFFFFF,0x7FFFFFFF};
		pDest[0] = rc;
		return 1;
	}
	begin = begin%MAX_RECORDS_COUNT;
	end = end%MAX_RECORDS_COUNT;
	if(end < begin)
	{
		end += MAX_RECORDS_COUNT;
	}
	ULONG loop = 0;
	for ( ; (begin != end) && (size != loop); begin++,loop++)
	{
		pDest[loop] = g_share_data.changes[ begin%MAX_RECORDS_COUNT ];
	}
	InterlockedExchange(&g_share_data.changes_begin,begin);
	return loop;
}