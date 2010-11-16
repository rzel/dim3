/****************************** File *********************************

Module: dim3 Model Utility
Author: Brian Barnes
 Usage: Main Routines

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

(c) 2000-2010 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#ifdef D3_PCH
	#include "dim3modelutility.h"
#endif

modelutility_settings_type		modelutility_settings;

/* =======================================================

      Model Setup Routines
      
======================================================= */

void model_setup(file_path_setup_type *file_path_setup,int anisotropic_mode,int mipmap_mode,int texture_quality_mode,bool in_engine)
{
	memmove(&modelutility_settings.file_path_setup,file_path_setup,sizeof(file_path_setup_type));
	modelutility_settings.anisotropic_mode=anisotropic_mode;
	modelutility_settings.mipmap_mode=mipmap_mode;
	modelutility_settings.texture_quality_mode=texture_quality_mode;
	modelutility_settings.in_engine=in_engine;
}

/* =======================================================

      New Model
      
======================================================= */

bool model_new(model_type *model,char *name)
{
	int				n;
	char			sub_path[1024];
	
    memset(model,0x0,sizeof(model_type));
	
		// model name and default path
		
	strcpy(model->name,name);
	
	sprintf(sub_path,"Models/%s",name);
	file_paths_data_default(&modelutility_settings.file_path_setup,model->load_base_path,sub_path,NULL,NULL);
		
		// default settings
	
	model->deform_mode=deform_mode_comulative_rotate;
	model->diffuse_boost=0.0f;
	
	for (n=0;n!=max_model_light;n++) {
		model->tags.light_bone_tag[n]=model_null_tag;
		model->tags.light_bone_idx[n]=-1;
	}

	for (n=0;n!=max_model_halo;n++) {
		model->tags.halo_bone_tag[n]=model_null_tag;
		model->tags.halo_bone_idx[n]=-1;
	}

	model->tags.name_bone_tag=model_null_tag;
	model->tags.name_bone_idx=-1;

	model->rigid_body.on=FALSE;

	model->rigid_body.y.max_ang=0.0f;			// unused, but in struct
	model->rigid_body.y.reset_factor=0.8f;
	model->rigid_body.y.smooth_factor=0.2f;
	model->rigid_body.x.max_ang=45.0f;
	model->rigid_body.x.reset_factor=0.8f;
	model->rigid_body.x.smooth_factor=0.2f;
	model->rigid_body.z.max_ang=45.0f;
	model->rigid_body.z.reset_factor=0.8f;
	model->rigid_body.z.smooth_factor=0.2f;

		// setup first mesh

	model->nmesh=1;

	model->meshes[0].nvertex=0;
	model->meshes[0].ntrig=0;
		
	model->meshes[0].vertexes=NULL;
	model->meshes[0].trigs=NULL;

	model->meshes[0].no_lighting=FALSE;
	model->meshes[0].diffuse=TRUE;
	model->meshes[0].blend_add=FALSE;
	model->meshes[0].tintable=FALSE;

	model->meshes[0].materials=malloc(max_model_texture*sizeof(model_material_type));
	if (model->meshes[0].materials==NULL) return(FALSE);

	bzero(model->meshes[0].materials,(max_model_texture*sizeof(model_material_type)));

		// other model structures

	model->nbone=0;
	model->npose=0;
	model->nanimate=0;
	model->nhit_box=0;

	model->bones=NULL;
	model->poses=NULL;
	model->animates=NULL;
	model->hit_boxes=NULL;

		// textures

	model->textures=malloc(max_model_texture*sizeof(texture_type));
	if (model->textures==NULL) return(FALSE);

	bzero(model->textures,(max_model_texture*sizeof(texture_type)));

	model_textures_clear(model);
	
	return(TRUE);
}

/* =======================================================

      Open Model
      
======================================================= */

bool model_open(model_type *model,char *name,bool load_bitmaps)
{
	char			sub_path[1024];

		// create model
		
	if (!model_new(model,name)) return(FALSE);
	
		// load base path is location of mesh.xml
		
	sprintf(sub_path,"Models/%s",model->name);
	file_paths_data(&modelutility_settings.file_path_setup,model->load_base_path,sub_path,NULL,NULL);
	
		// read XML
		
	if (!read_mesh_xml(model)) {
		model_close(model);
		return(FALSE);
	}
	
	read_pose_xml(model);
	read_animate_xml(model);
	
		// pre-calculate parents
		
	model_calculate_parents(model);
	
		// read bitmaps

	if (load_bitmaps) model_textures_read(model);
	
	return(TRUE);
}

/* =======================================================

      Save Model
      
======================================================= */

bool model_save(model_type *model)
{
	bool			ok,ok2,ok3;
	
		// write XML

    ok=write_mesh_xml(model);
    ok2=write_pose_xml(model);
    ok3=write_animate_xml(model);
	
	return(ok||ok2||ok3);
}

/* =======================================================

      Close Model
      
======================================================= */

void model_close(model_type *model)
{
	int				n;
	
	model_textures_close(model);
	
	for (n=0;n!=model->nmesh;n++) {
		if (model->meshes[n].vertexes!=NULL) free(model->meshes[n].vertexes);
		if (model->meshes[n].trigs!=NULL) free(model->meshes[n].trigs);
		if (model->meshes[n].materials!=NULL) free(model->meshes[n].materials);
	}

	free(model->textures);
	
	if (model->bones!=NULL) free(model->bones);
	if (model->poses!=NULL) free(model->poses);
	if (model->animates!=NULL) free(model->animates);
	if (model->hit_boxes!=NULL) free(model->hit_boxes);
}

/* =======================================================

      Refresh Model Textures
      
======================================================= */

void model_refresh_textures(model_type *model)
{
	model_textures_close(model);
	model_textures_read(model);
}

/* =======================================================

      Model Memory Size
      
======================================================= */

int model_memory_size(model_type *model)
{
	int			n,sz;

	sz=0;

	for (n=0;n!=model->nmesh;n++) {
		sz+=(model->meshes[n].nvertex*sizeof(model_vertex_type));
		sz+=(model->meshes[n].ntrig*sizeof(model_trig_type));
		sz+=(max_model_texture*sizeof(model_material_type));

		sz+=((model->meshes[n].nvertex*3)*sizeof(float));
		sz+=((model->meshes[n].nvertex*3)*sizeof(float));
		sz+=((model->meshes[n].nvertex*3)*sizeof(float));
		sz+=((model->meshes[n].nvertex*3)*sizeof(float));
	}

	sz+=(max_model_texture*sizeof(texture_type));
	
	sz+=(model->nbone*sizeof(model_bone_type));
	sz+=(model->npose*sizeof(model_pose_type));
	sz+=(model->nanimate*sizeof(model_animate_type));
	sz+=(model->nhit_box*sizeof(model_hit_box_type));

	return(sz);
}
