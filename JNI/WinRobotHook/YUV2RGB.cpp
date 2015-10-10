/*! \file $Id: YUV2RGB.cpp 66 2011-06-20 14:08:41Z caoym $
*  \author caoym
*  \brief YUV -> RGB			
*/
#include "StdAfx.h"
#include "YUV2RGB.h"



enum {
	CLIP_SIZE = 811,
	CLIP_OFFSET = 277,
	YMUL = 298,
	RMUL = 409,
	BMUL = 516,
	G1MUL = -100,
	G2MUL = -208,
};

static int tables_initialized = 0;

static int yuv2rgb_y[256];
static int yuv2rgb_r[256];
static int yuv2rgb_b[256];
static int yuv2rgb_g1[256];
static int yuv2rgb_g2[256];

static unsigned long yuv2rgb_clip[CLIP_SIZE];
static unsigned long yuv2rgb_clip8[CLIP_SIZE];
static unsigned long yuv2rgb_clip16[CLIP_SIZE];

#define COMPOSE_RGB(yc, rc, gc, bc)		\
	( 0x00000000 |				\
	yuv2rgb_clip16[(yc) + (rc)] |		\
	yuv2rgb_clip8[(yc) + (gc)] |		\
	yuv2rgb_clip[(yc) + (bc)] )

static void init_yuv2rgb_tables(void)
{
	int i;
	for ( i = 0; i < 256; ++i )
	{
		yuv2rgb_y[i] = (YMUL * (i - 16) + 128) >> 8;
		yuv2rgb_r[i] = (RMUL * (i - 128)) >> 8;
		yuv2rgb_b[i] = (BMUL * (i - 128)) >> 8;
		yuv2rgb_g1[i] = (G1MUL * (i - 128)) >> 8;
		yuv2rgb_g2[i] = (G2MUL * (i - 128)) >> 8;
	}
	for ( i = 0 ; i < CLIP_OFFSET ; ++i )
	{
		yuv2rgb_clip[i] = 0;
		yuv2rgb_clip8[i] = 0;
		yuv2rgb_clip16[i] = 0;
	}
	for ( ; i < CLIP_OFFSET + 256 ; ++i )
	{
		yuv2rgb_clip[i] = i - CLIP_OFFSET;
		yuv2rgb_clip8[i] = (i - CLIP_OFFSET) << 8;
		yuv2rgb_clip16[i] = (i - CLIP_OFFSET) << 16;
	}
	for ( ; i < CLIP_SIZE ; ++i )
	{
		yuv2rgb_clip[i] = 255;
		yuv2rgb_clip8[i] = 255 << 8;
		yuv2rgb_clip16[i] = 255 << 16;
	}

	tables_initialized = 1;
}

int uyvy_to_rgb32(int width, int height, const char * src,int src_pitch, char * dest,int dest_pitch)
{
	unsigned int * d = (unsigned int *)dest;
	int i, j;

	if ( !tables_initialized )
		init_yuv2rgb_tables();

	for ( i = 0; i < height; ++i )
	{
		const char * src_temp = src;
		const char * dest_temp = (const char *)d;
		for ( j = 0; j < width / 2; ++j )
		{
			const unsigned char u = *src++;
			const unsigned char y0  = *src++;
			const unsigned char v = *src++;
			const unsigned char y1  = *src++;
			const int rc = yuv2rgb_r[v];
			const int gc = yuv2rgb_g1[v] + yuv2rgb_g2[u];
			const int bc = yuv2rgb_b[u];
			const int yc0 = CLIP_OFFSET + yuv2rgb_y[y0];
			const int yc1 = CLIP_OFFSET + yuv2rgb_y[y1];

			*d++ = COMPOSE_RGB(yc0, rc, gc, bc);
			*d++ = COMPOSE_RGB(yc1, rc, gc, bc);
		}
		src = src_temp + src_pitch;
		d = (unsigned int *)(dest_temp + dest_pitch);
		
	}

	return 0;
}

int yuy2_to_rgb32(int width, int height, const char * src,int src_pitch, char * dest,int dest_pitch)
{
	unsigned int * d = (unsigned int *)dest;
	int i, j;

	if ( !tables_initialized )
		init_yuv2rgb_tables();

	for ( i = 0; i < height; ++i )
	{
		const char * src_temp = src;
		const char * dest_temp = (const char *)d;

		for ( j = 0; j < width / 2; ++j )
		{
			const unsigned char y0 = *src++;
			const unsigned char u  = *src++;
			const unsigned char y1 = *src++;
			const unsigned char v  = *src++;
			const int rc = yuv2rgb_r[v];
			const int gc = yuv2rgb_g1[v] + yuv2rgb_g2[u];
			const int bc = yuv2rgb_b[u];
			const int yc0 = CLIP_OFFSET + yuv2rgb_y[y0];
			const int yc1 = CLIP_OFFSET + yuv2rgb_y[y1];

			*d++ = COMPOSE_RGB(yc0, rc, gc, bc);
			*d++ = COMPOSE_RGB(yc1, rc, gc, bc);
		}
		src = src_temp + src_pitch;
		d = (unsigned int *)(dest_temp + dest_pitch);
	}

	return 0;
}