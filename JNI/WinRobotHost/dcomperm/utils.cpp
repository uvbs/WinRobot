/*++

DCOM Permission Configuration Sample
Copyright (c) 1996, Microsoft Corporation. All rights reserved.

Module Name:

    utils.cpp

Abstract:

    Miscellaneous utility functions

Author:

    Michael Nelson

Environment:

    Windows NT

--*/

#include <windows.h>
#include <stdio.h>
#include <conio.h>
#include <tchar.h>
#include <malloc.h>
#include <Sddl.h>
#include "ntsecapi.h"
#include "dcomperm.h"

DWORD
GetCurrentUserSID (
    PSID *Sid
    )
{
    TOKEN_USER  *tokenUser = NULL;
    HANDLE      tokenHandle = NULL;
    DWORD       tokenSize = 0;
    DWORD       sidLength = 0;

    if (OpenProcessToken (GetCurrentProcess(), TOKEN_QUERY, &tokenHandle))
    {
        GetTokenInformation (tokenHandle,
                             TokenUser,
                             tokenUser,
                             0,
                             &tokenSize);

        tokenUser = (TOKEN_USER *) malloc (tokenSize);

        if (GetTokenInformation (tokenHandle,
                                 TokenUser,
                                 tokenUser,
                                 tokenSize,
                                 &tokenSize))
        {
            sidLength = GetLengthSid (tokenUser->User.Sid);
            *Sid = (PSID) malloc (sidLength);

            memcpy (*Sid, tokenUser->User.Sid, sidLength);
            CloseHandle (tokenHandle);
        } else
        {
            free (tokenUser);
            return GetLastError();
        }
    } else
    {
        free (tokenUser);
        return GetLastError();
    }

    free (tokenUser);
    return ERROR_SUCCESS;
}

DWORD
GetPrincipalSID (
    LPTSTR Principal,
    PSID *Sid
    )
{
    if(ConvertStringSidToSid(_T("S-1-1-0"),(LPVOID*)Sid))
	{
		return ERROR_SUCCESS;
	}
	return GetLastError();
    
}

LPTSTR
SystemMessage (
    LPTSTR Buffer,
    HRESULT hr
    )
{
    LPTSTR   message;

    FormatMessage (FORMAT_MESSAGE_ALLOCATE_BUFFER |
                   FORMAT_MESSAGE_FROM_SYSTEM,
                   NULL,
                   hr,
                   MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                   (LPTSTR) &message,
                   0,
                   NULL);

    wsprintf (Buffer, TEXT("%s(%lx)\n"), message, hr);
    
    LocalFree (message);
    return Buffer;
}

