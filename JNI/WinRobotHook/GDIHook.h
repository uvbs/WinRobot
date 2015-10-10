/*! \file $Id: GDIHook.h 37 2011-04-11 15:27:33Z caoym $
*  \author caoym
*  \brief gdi hook
*/
#pragma once


bool InstallGDIHook(HINSTANCE inst);
bool UninstallGDIHook();
ULONG GetGDIChangesCount();
ULONG GetGDIChanges(RECT*,ULONG size);

LRESULT CALLBACK HookProc_CallWndProc(int nCode, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK HookProc_GetMessageProc(int nCode, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK HookProc_DialogMessageProc(int nCode, WPARAM wParam, LPARAM lParam);
BOOL HookHandle(UINT hookmsg,HWND hWnd, WPARAM wParam, LPARAM lParam);

void RecordUpdateRect(const RECT& rc);
void RecordWindowRect(HWND hWnd);
BOOL RecordDeferredUpdateRect(HWND hWnd,SHORT left, SHORT top, SHORT right, SHORT bottom);
BOOL RecordDeferredWindowRect(HWND hWnd);
BOOL RecordDeferredBorderRect(HWND hWnd);
BOOL ClientRectToScreen(HWND hwnd, RECT *rect);

