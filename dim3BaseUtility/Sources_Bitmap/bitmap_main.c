/****************************** File *********************************

Module: dim3 Base Utility
Author: Brian Barnes
 Usage: Main Bitmap Functions
 
***************************** License ********************************

This code can be freely used as long as these conditions are met:

1. This header, in its entirety, is kept with the code
2. This credit �Created with dim3 Technology� is given on a single
application screen and in a single piece of the documentation
3. It is not resold, in it's current form or modified, as an
engine-only product

This code is presented as is. The author of dim3 takes no
responsibilities for any version of this code.

Any non-engine product (games, etc) created with this code is free
from any and all payment and/or royalties to the author of dim3,
and can be sold or given away.

(c) 2000-2011 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#ifdef D3_PCH
	#include "dim3baseutility.h"
#endif

/* =======================================================

      New Bitmaps
      
======================================================= */

void bitmap_new(bitmap_type *bitmap)
{
	bitmap->wid=bitmap->high=0;
	bitmap->alpha_mode=alpha_mode_none;
	bitmap->gl_id=-1;
}

/* =======================================================

      Bitmaps Utilities
      
======================================================= */

void bitmap_scrub_black_to_alpha(bitmap_type *bitmap,unsigned char *png_data)
{
	int					n,psz;
	unsigned char		*data;

	data=png_data;
	psz=(bitmap->wid<<2)*bitmap->high;

	for (n=0;n<psz;n+=4) {
		*(data+3)=*data;
		data+=4;
	}
}

void bitmap_set_alpha_mode(bitmap_type *bitmap,unsigned char *png_data)
{
	int					n,psz;
	unsigned char		*data;

		// assume it's opaque until we see different data

	bitmap->alpha_mode=alpha_mode_none;
					
		// search for transparencies

	data=png_data+3;
	psz=(bitmap->wid<<2)*bitmap->high;
		
	for (n=0;n<psz;n+=4) {
			
			// a single non-0xFF and non-0x00 means transparency
			
		if ((*data!=0xFF) && (*data!=0x0)) {
			bitmap->alpha_mode=alpha_mode_transparent;
			return;
		}
		
			// a single 0x0 means it's possibly a cut out
			// but never an opaque
			
		if (*data==0x0) bitmap->alpha_mode=alpha_mode_cut_out;

		data+=4;
	}
}

unsigned char* bitmap_strip_alpha(bitmap_type *bitmap,unsigned char *png_data)
{
	int					n,psz;
	unsigned char		*strip_data,*srce,*dest;

		// new data
		
	psz=(bitmap->wid*3)*bitmap->high;
	strip_data=(unsigned char*)malloc(psz);
	
		// strip out alphas
		
	srce=png_data;
	dest=strip_data;
	
	psz=(bitmap->wid<<2)*bitmap->high;
		
	for (n=0;n<psz;n+=4) {
		*dest++=*srce++;
		*dest++=*srce++;
		*dest++=*srce++;
		srce++;
	}
	
	return(strip_data);
}

int bitmap_find_nearest_power_2(int sz)
{
	int			n,p2;

	p2=4;
	for (n=0;n!=11;n++) {
		if (p2>sz) return(p2/2);
		p2*=2;
	}

	return(1024);
}

unsigned char* bitmap_fix_power_2_and_quality(bitmap_type *bitmap,bool has_alpha,int texture_quality_mode,unsigned char *png_data)
{
	int				max_sz,wid,high,byte_sz,x,y,dsz;
	float			x_skip,y_skip;
	unsigned char	*data,*sptr,*dptr;

		// get power of 2 sizes

	wid=bitmap_find_nearest_power_2(bitmap->wid);
	high=bitmap_find_nearest_power_2(bitmap->high);

			// get quality

	if (texture_quality_mode!=texture_quality_mode_high) {
		max_sz=512;
		if (texture_quality_mode==texture_quality_mode_low) max_sz=256;

		if (wid>max_sz) wid=max_sz;
		if (high>max_sz) high=max_sz;
	}

		// any changes?

	if ((wid==bitmap->wid) && (high==bitmap->high)) return(png_data);

		// is it 3 or 4 bytes?

	if (has_alpha) {
		byte_sz=4;
	}
	else {
		byte_sz=3;
	}

		// convert data

	dsz=(wid*byte_sz)*high;

	data=(unsigned char*)malloc(dsz);
	if (data==NULL) return(png_data);

	x_skip=(float)bitmap->wid/(float)wid;
	y_skip=(float)bitmap->high/(float)high;

	dptr=data;

	for (y=0;y!=high;y++) {
	
		for (x=0;x!=wid;x++) {
			sptr=png_data+(byte_sz*(int)(x_skip*(float)x))+((bitmap->wid*byte_sz)*(int)(y_skip*(float)y));

			*dptr++=*sptr++;
			*dptr++=*sptr++;
			*dptr++=*sptr++;

			if (byte_sz==4) *dptr++=*sptr;
		}
	}

	free(png_data);

		// reset width and height

	bitmap->wid=wid;
	bitmap->high=high;

	return(data);
}

unsigned char* bitmap_setup_alpha(bitmap_type *bitmap,unsigned char *png_data,bool alpha_channel,bool scrub_black_to_alpha)
{
	unsigned char				*strip_data;
	
	bitmap->alpha_mode=alpha_mode_none;

	if (alpha_channel) {

			// scrub blacks to alphas
			// this is mostly used for glow maps

		if (scrub_black_to_alpha) bitmap_scrub_black_to_alpha(bitmap,png_data);
		
			// find if bitmap has transparencies

		bitmap_set_alpha_mode(bitmap,png_data);
		
			// if alpha is actually all 0x0, then strip it
			
		if (bitmap->alpha_mode==alpha_mode_none) {
			strip_data=bitmap_strip_alpha(bitmap,png_data);
			if (strip_data!=NULL) {
				free(png_data);
				png_data=strip_data;
			}
		}
	}
	
	return(png_data);
}

/* =======================================================

      Open Bitmaps
      
======================================================= */

bool bitmap_open(bitmap_type *bitmap,char *path,int anisotropic_mode,int mipmap_mode,int texture_quality_mode,bool compress,bool rectangle,bool pixelated,bool scrub_black_to_alpha)
{
	unsigned char		*png_data;
	bool				ok,alpha_channel;
	
	bitmap_new(bitmap);
	
		// read bitmap
	
	png_data=png_utility_read(path,&bitmap->wid,&bitmap->high,&alpha_channel);
	if (png_data==NULL) return(FALSE);

		// if not a rectangle, fix size
		// if not a power of two and do any
		// texture quality changes

		// opengl es doesn't support non-square textures

#ifndef D3_OPENGL_ES
	if (!rectangle) png_data=bitmap_fix_power_2_and_quality(bitmap,alpha_channel,texture_quality_mode,png_data);
#else
	png_data=bitmap_fix_power_2_and_quality(bitmap,alpha_channel,texture_quality_mode,png_data);
#endif

		// set alphas and scrubbing
		
	png_data=bitmap_setup_alpha(bitmap,png_data,alpha_channel,scrub_black_to_alpha);
		
		// get the texture
		
	ok=bitmap_texture_open(bitmap,png_data,anisotropic_mode,mipmap_mode,compress,rectangle,pixelated);
		
	free(png_data);
	
	return(ok);
}

/* =======================================================

      Color Bitmap
      
======================================================= */

bool bitmap_color(bitmap_type *bitmap,d3col *col)
{
	int				n,kr,kg,kb;
	unsigned char	*png_data,*dptr;
	bool			ok;
	
	bitmap->wid=bitmap->high=32;
	bitmap->alpha_mode=alpha_mode_none;
	
	png_data=malloc((32*32)*3);
	if (png_data==NULL) return(FALSE);
	
	kr=(int)(0xFF*col->r);
	kg=(int)(0xFF*col->g);
	kb=(int)(0xFF*col->b);
	
	dptr=png_data;

	for (n=0;n!=(32*32);n++) {
		*dptr++=kr;
		*dptr++=kg;
		*dptr++=kb;
	}
	
	ok=bitmap_texture_open(bitmap,png_data,anisotropic_mode_none,mipmap_mode_none,FALSE,FALSE,TRUE);

	free(png_data);
	
	return(ok);
}

/* =======================================================

      Data Bitmap
      
======================================================= */

bool bitmap_data(bitmap_type *bitmap,unsigned char *data,int wid,int high,bool alpha_channel,int anisotropic_mode,int mipmap_mode,bool compress,bool rectangle)
{
	bitmap->wid=wid;
	bitmap->high=high;

		// if not a rectangle, fix size
		// if not a power of two

	if (!rectangle) data=bitmap_fix_power_2_and_quality(bitmap,alpha_channel,texture_quality_mode_high,data);
	
		// find if bitmap has transparencies
	
	bitmap->alpha_mode=alpha_mode_none;
	if (alpha_channel) bitmap_set_alpha_mode(bitmap,data);
	
		// get the texture
		
	return(bitmap_texture_open(bitmap,data,anisotropic_mode,mipmap_mode,compress,rectangle,FALSE));
}

/* =======================================================

      Combine Bitmaps
      
======================================================= */

bool bitmap_combine(bitmap_type *bitmap,char *bitmap_path,char *bumpmap_path,int anisotropic_mode,int mipmap_mode,int texture_quality_mode,bool compress,bool pixelated)
{
	int					n,wid,high,pixel_cnt,data_sz;
	float				f,pf[3];
	unsigned char		*bitmap_data,*bumpmap_data,
						*srce,*dest;
	bool				ok,bitmap_only,alpha_channel;
	d3vct				normal,bump;
	bitmap_type			bumpmap;
	
		// load the bitmap
		
	bitmap_new(bitmap);
	
	bitmap_data=png_utility_read(bitmap_path,&bitmap->wid,&bitmap->high,&alpha_channel);
	if (bitmap_data==NULL) return(FALSE);
	
	bitmap_data=bitmap_fix_power_2_and_quality(bitmap,alpha_channel,texture_quality_mode,bitmap_data);
	bitmap_data=bitmap_setup_alpha(bitmap,bitmap_data,alpha_channel,FALSE);
	
		// load the bump
		// if no bump, just use bitmap
		
	bitmap_new(&bumpmap);
	
	bumpmap_data=png_utility_read(bumpmap_path,&bumpmap.wid,&bumpmap.high,&alpha_channel);
	if (bumpmap_data!=NULL) {
		bumpmap_data=bitmap_fix_power_2_and_quality(&bumpmap,alpha_channel,texture_quality_mode,bumpmap_data);
		bumpmap_data=bitmap_setup_alpha(&bumpmap,bumpmap_data,alpha_channel,FALSE);
	}
	
		// if no bump map or
		// different sizes, just use bitmap
		
	bitmap_only=TRUE;
	
	if (bumpmap_data!=NULL) {
		bitmap_only=(bitmap->wid!=bumpmap.wid) || (bitmap->high!=bumpmap.high);
	}
	
	if (bitmap_only) {
		ok=bitmap_texture_open(bitmap,bitmap_data,anisotropic_mode,mipmap_mode,compress,FALSE,pixelated);
		if (bumpmap_data!=NULL) free(bumpmap_data);
		free(bitmap_data);
		return(ok);
	}
	
		// work the bump into the bitmap

	pixel_cnt=wid*high;

	if (bitmap->alpha_mode!=alpha_mode_none) {
		data_sz=pixel_cnt*4;
	}
	else {
		data_sz=pixel_cnt*3;
	}
		
		// our fake normal

	normal.x=normal.y=0.0f;
	normal.z=0.5f;

	dest=bitmap_data;
	srce=bumpmap_data;

	for (n=0;n!=pixel_cnt;n++) {

			// unpack the bump normal

		bump.x=((((float)*srce++)/255.0f)*2.0f)-1.0f;
		bump.y=((((float)*srce++)/255.0f)*2.0f)-1.0f;
		bump.z=((((float)*srce++)/255.0f)*2.0f)-1.0f;
		vector_normalize(&bump);

		if (bumpmap.alpha_mode!=alpha_mode_none) srce++;

			// get the dot3
			// we make sure it never gets too
			// dark for this simple version

		f=vector_dot_product(&normal,&bump);
		if (f<0.75f) f=0.75f;

			// multiply it into bitmap

		pf[0]=((float)*dest)/255.0f;
		pf[1]=((float)*(dest+1))/255.0f;
		pf[2]=((float)*(dest+2))/255.0f;

		*dest++=(int)((pf[0]*f)*255.0f);
		*dest++=(int)((pf[1]*f)*255.0f);
		*dest++=(int)((pf[2]*f)*255.0f);

		if (bitmap->alpha_mode!=alpha_mode_none) dest++;
	}

		// and finally make the combine bitmap
	
	ok=bitmap_texture_open(bitmap,bitmap_data,anisotropic_mode,mipmap_mode,compress,FALSE,pixelated);
	
	free(bumpmap_data);
	free(bitmap_data);
	
	return(ok);
}

/* =======================================================

      Close Bitmaps
      
======================================================= */

void bitmap_close(bitmap_type *bitmap)
{
	if (bitmap->gl_id!=-1) bitmap_texture_close(bitmap);
	bitmap->gl_id=-1;
}

/* =======================================================

      Misc Bitmap Functions
      
======================================================= */

unsigned char* bitmap_load_png_data(char *path,int *p_wid,int *p_high,bool *alpha_channel)
{
	return(png_utility_read(path,p_wid,p_high,alpha_channel));
}

bool bitmap_write_png_data(unsigned char *data,int wid,int high,bool alpha_channel,char *path)
{
	return(png_utility_write(data,wid,high,alpha_channel,path));
}

bool bitmap_check(char *path,char *err_str)
{
	return(png_utility_check(path,err_str));
}

bool bitmap_copy(char *srce_path,char *dest_path)
{
	return(png_utility_copy(srce_path,dest_path));
}



