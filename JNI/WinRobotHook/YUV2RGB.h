/*! \file $Id: YUV2RGB.h 64 2011-06-15 14:01:00Z caoym $
*  \author caoym
*  \brief YUV -> RGB			
*/
#pragma once
//! uyvy to rgb32
int uyvy_to_rgb32(int width, int height, const char * src,int src_pitch, char * dest,int dest_pitch);
//! yuy2 to rgb32
int yuy2_to_rgb32(int width, int height, const char * src,int src_pitch, char * dest,int dest_pitch);