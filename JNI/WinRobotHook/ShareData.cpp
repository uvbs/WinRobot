/*! \file $Id: ShareData.cpp 37 2011-04-11 15:27:33Z caoym $
*  \author caoym
*  \brief dll share data
*/

#include "StdAfx.h"
#include "ShareData.h"

#pragma data_seg(".SHARED")
SHARE_DATA g_share_data = {0};
#pragma data_seg()
#pragma comment(linker, "/section:.SHARED,rws") 