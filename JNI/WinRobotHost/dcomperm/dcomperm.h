/*++

DCOM Permission Configuration Sample
Copyright (c) 1996, Microsoft Corporation. All rights reserved.

Module Name:

    dcomperm.h

Abstract:

    Include file for DCOM Permission Configuration sample

Author:

    Michael Nelson

Environment:

    Windows NT

--*/

// added for definitions and headers.
#include <windows.h>

#define GUIDSTR_MAX 38

#ifndef STR2UNI

#define STR2UNI(unistr, regstr) \
        mbstowcs (unistr, regstr, strlen (regstr)+1);

#define UNI2STR(regstr, unistr) \
        wcstombs (regstr, unistr, wcslen (unistr)+1);

#endif


//
// Wrappers
//

DWORD
ListDefaultAccessACL();

DWORD
ListDefaultLaunchACL();

DWORD
ListAppIDAccessACL (
    LPTSTR AppID
    );

DWORD
ListAppIDLaunchACL (
    LPTSTR AppID
    );

DWORD ZapAppIDLaunchACL(LPTSTR AppID);

DWORD
ChangeDefaultAccessACL (
    LPTSTR Principal,
    BOOL SetPrincipal,
    BOOL Permit,
	DWORD PermissionMask
    );

DWORD
ChangeDefaultLaunchACL (
    LPTSTR Principal,
    BOOL SetPrincipal,
    BOOL Permit,
	DWORD PermissionMask
    );

DWORD
ChangeAppIDAccessACL (
    LPTSTR AppID,
    LPTSTR Principal,
    BOOL SetPrincipal,
    BOOL Permit,
	DWORD PermissionMask
    );

DWORD
ChangeAppIDLaunchACL (
    LPTSTR AppID,
    LPTSTR Principal,
    BOOL SetPrincipal,
    BOOL Permit,
	DWORD PermissionMask 
    );

DWORD GetRunAsPassword (
    LPTSTR AppID,
    LPTSTR Password
    );

DWORD SetRunAsPassword (
    LPTSTR AppID,
    LPTSTR Principal,
    LPTSTR Password
    );

DWORD GetRunAsPassword (
    LPTSTR AppID,
    LPTSTR Password
    );

DWORD SetRunAsPassword (
    LPTSTR AppID,
    LPTSTR Password
    );

//
// Internal functions
//

DWORD
CreateNewSD (
    SECURITY_DESCRIPTOR **SD,
    bool install_defaults = true
    );

DWORD
MakeSDAbsolute (
    PSECURITY_DESCRIPTOR OldSD,
    PSECURITY_DESCRIPTOR *NewSD
    );

DWORD
SetNamedValueSD (
    HKEY RootKey,
    LPTSTR KeyName,
    LPTSTR ValueName,
    SECURITY_DESCRIPTOR *SD
    );

DWORD
GetNamedValueSD (
    HKEY RootKey,
    LPTSTR KeyName,
    LPTSTR ValueName,
    SECURITY_DESCRIPTOR **SD,
    BOOL *NewSD,
    bool install_defaults = true
    );

DWORD
ListNamedValueSD (
    HKEY RootKey,
    LPTSTR KeyName,
    LPTSTR ValueName
    );

DWORD ZapNamedValueSD(HKEY RootKey, LPTSTR KeyName, LPTSTR ValueName);

DWORD
AddPrincipalToNamedValueSD (
    HKEY RootKey,
    LPTSTR KeyName,
    LPTSTR ValueName,
    LPTSTR Principal,
    BOOL Permit,
    bool install_defaults = true,
	DWORD PermissionMask = COM_RIGHTS_EXECUTE
    );

DWORD
RemovePrincipalFromNamedValueSD (
    HKEY RootKey,
    LPTSTR KeyName,
    LPTSTR ValueName,
    LPTSTR Principal,
    bool install_defaults = true
    );

DWORD
GetCurrentUserSID (
    PSID *Sid
    );

DWORD
GetPrincipalSID (
    LPTSTR Principal,
    PSID *Sid
    );

DWORD
CopyACL (
    PACL OldACL,
    PACL NewACL
    );

DWORD
AddAccessDeniedACEToACL (
    PACL *Acl,
    DWORD PermissionMask,
    LPTSTR Principal
    );

DWORD
AddAccessAllowedACEToACL (
    PACL *Acl,
    DWORD PermissionMask,
    LPTSTR Principal
    );

DWORD
RemovePrincipalFromACL (
    PACL Acl,
    LPTSTR Principal
    );

void
ListACL (
    PACL Acl
    );

DWORD
SetAccountRights (
    LPTSTR User,
    LPTSTR Privilege
    );

//
// Utility Functions
//

LPTSTR
SystemMessage (
    LPTSTR szBuffer,
    HRESULT hr
    );

