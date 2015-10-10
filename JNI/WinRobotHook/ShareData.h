/*! \file $Id: ShareData.h 88 2011-09-30 14:26:46Z caoym $
*  \author caoym
*  \brief dll share data
*/
#pragma once

#define MAX_RECORDS_COUNT (1024*16)
/**
 * share data between GDI hooked processes
 */
struct SHARE_DATA 
{
	/** specify who create installed hooks 
	 * while CChangeRecorder::Stop,only the owner can uninstall hooks  
	 */
	LONG owner; 
	HHOOK hooks[WH_MAX];
	LONG changes_begin;
	LONG changes_writting_end; //the end the include the data which is being writing
	LONG changes_done_end;     //the end the include the data without writing
	LONG image_file_size;
	RECT changes[MAX_RECORDS_COUNT]; //
};
extern SHARE_DATA g_share_data;