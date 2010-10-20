/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: File Open Routines

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

(c) 2000-2007 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#include "dim3editor.h"
#include "resource.h"

extern HINSTANCE				hinst;
extern HWND						wnd;

char							fp_file_name[256],*fp_dialog_name;
bool							fp_cancel;
file_path_directory_type		*fpd;

extern file_path_setup_type		file_path_setup;

/* =======================================================

      File List Event Handlers
      
======================================================= */

HTREEITEM dialog_file_open_fill_list(HWND diag,HTREEITEM parent_item,int parent_idx)
{
	int					n;
	TV_INSERTSTRUCT		tv_item;
	HTREEITEM			item,first_item;

	first_item=NULL;

	for (n=0;n!=fpd->nfile;n++) {
		if (fpd->files[n].parent_idx==parent_idx) {
			tv_item.hParent=parent_item;
			tv_item.hInsertAfter=TVI_LAST;
			tv_item.item.mask=TVIF_TEXT|TVIF_PARAM;
			tv_item.item.pszText=fpd->files[n].file_name;
			tv_item.item.lParam=fpd->files[n].is_dir?1:0;
			item=(HTREEITEM)SendDlgItemMessage(diag,IDC_FILE_OPEN_TREE,TVM_INSERTITEM,0,(LPARAM)&tv_item);

			if (first_item==NULL) first_item=item;
			dialog_file_open_fill_list(diag,item,n);
		}
	}

	return(first_item);
}

void dialog_file_open_set(HWND diag)
{
	HTREEITEM			item;

	item=dialog_file_open_fill_list(diag,NULL,-1);

	if (item!=NULL) SendDlgItemMessage(diag,IDC_FILE_OPEN_TREE,TVM_EXPAND,TVE_EXPAND,(LPARAM)item);
}

bool dialog_file_open_get(HWND diag)
{
	HTREEITEM			item;
	TV_ITEM				info;
	
	fp_file_name[0]=0x0;

	item=(HTREEITEM)SendDlgItemMessage(diag,IDC_FILE_OPEN_TREE,TVM_GETNEXTITEM,TVGN_CARET,(LPARAM)0);
	if (item==NULL) return(FALSE);

	info.mask=TVIF_TEXT|TVIF_PARAM;
	info.pszText=fp_file_name;
	info.cchTextMax=256;
	info.hItem=item;

	SendDlgItemMessage(diag,IDC_FILE_OPEN_TREE,TVM_GETITEM,0,(LPARAM)&info);

	return(info.lParam==0);
}

LRESULT CALLBACK dialog_file_open_proc(HWND diag,UINT msg,WPARAM wparam,LPARAM lparam)
{
	LPNMHDR			hdr;

	switch (msg) {

		case WM_INITDIALOG:
			SetWindowText(diag,fp_dialog_name);
			dialog_file_open_set(diag);
			return(TRUE);

		case WM_COMMAND:
			
			switch (wparam) {

				case IDOK:
					dialog_file_open_get(diag);
					EndDialog(diag,0);
					return(TRUE);

				case IDCANCEL:
					EndDialog(diag,1);
					return(TRUE);

			}

			break;

		case WM_NOTIFY:

			hdr=(LPNMHDR)lparam;

			if ((hdr->idFrom==IDC_FILE_OPEN_TREE) && (hdr->code==NM_DBLCLK)) {
				if (!dialog_file_open_get(diag)) return(FALSE);
				EndDialog(diag,0);
			}

			break;

	}

	return(FALSE);
}

/* =======================================================

      Run File Open
      
======================================================= */

bool dialog_file_open_run(char *dialog_name,char *search_path,char *extension,char *required_file_name,char *file_name)
{
	bool			ok;

		// scan for files
		
	if (extension!=NULL) {
		fpd=file_paths_read_directory_data(&file_path_setup,search_path,extension);
	}
	else {
		fpd=file_paths_read_directory_data_dir(&file_path_setup,search_path,required_file_name);
	}

		// run dialog

	fp_dialog_name=dialog_name;

	ok=(DialogBox(hinst,MAKEINTRESOURCE(IDD_FILE_OPEN),NULL,dialog_file_open_proc)==0);

		// close the files

	file_paths_close_directory(fpd);
	
	if (!ok) return(FALSE);

		// get the file

	strcpy(file_name,fp_file_name);
	return(TRUE);
}
