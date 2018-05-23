/* Weditres generated include file. Do NOT edit */
#include <windows.h>
#include <lfc.h>
#define	IDD_MAINDIALOG	100
#define	ID_ICON_MAIN	101
#define	ID_SCREEN	102
#define	ID_STATUS	103
#define	ID_TEXT	104
#define	ID_CONFIG	105
#define	IDD_CONFIG_DIALOG	500
#define	ID_NAMES	501
#define	ID_VALUES	502
#define	ID_SAVE	503
#define	ID_RESET	504
#define	ID_EXIT	505
/*@ Prototypes @*/
#ifndef WEDIT_PROTOTYPES
#define WEDIT_PROTOTYPES
/*
 * Structure for dialog Dlg100
 */
struct _Dlg100 {
	ST_MULEDIT *id_text;
	ST_BUTTON *id_config;
	ST_STATIC *id_status;
	ST_MULEDIT *id_screen;
	ST_BUTTON *idok;
	ST_BUTTON *idcancel;
	HWND hwnd;
	WPARAM wParam;
	LPARAM lParam;
};


/*
 * Structure for dialog Dlg500
 */
struct _Dlg500 {
	ST_BUTTON *id_exit;
	ST_BUTTON *id_reset;
	ST_BUTTON *id_save;
	ST_MULEDIT *id_names;
	ST_MULEDIT *id_values;
	HWND hwnd;
	WPARAM wParam;
	LPARAM lParam;
};


#endif
void SetDlgBkColor(HWND,COLORREF);
BOOL APIENTRY HandleCtlColor(UINT,DWORD);
/*
 * Callbacks for dialog Dlg100
 */
HWND StartDlg100(HWND parent);
int RunDlg100(HWND parent);
void AddGdiObject(HWND,HANDLE);
BOOL WINAPI HandleDefaultMessages(HWND hwnd,UINT msg,WPARAM wParam,DWORD lParam);
/* Control: IDCANCEL*/
long Dlg100CancelSelected(ST_BUTTON *,struct _Dlg100 *);
/* Control: IDOK*/
BOOL Dlg100OkValidate(HWND hwnd);
/* Control: IDOK*/
long Dlg100OkSelected(ST_BUTTON *,struct _Dlg100 *);
/* Control: ID_SCREEN*/
long Dlg100Edit_field102KillFocus(ST_MULEDIT *,struct _Dlg100 *);
long Dlg100Init(ST_DIALOGBOX *,struct _Dlg100 *);
BOOL APIENTRY Dlg100(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);
/*
 * Callbacks for dialog Dlg500
 */
HWND StartDlg500(HWND parent);
int RunDlg500(HWND parent);
/* Control: ID_SAVE*/
long Dlg500Push_button503Selected(ST_BUTTON *,struct _Dlg500 *);
/* Control: ID_RESET*/
long Dlg500Push_button504Selected(ST_BUTTON *,struct _Dlg500 *);
/* Control: ID_EXIT*/
long Dlg500Push_button505Selected(ST_BUTTON *,struct _Dlg500 *);
long Dlg500Init(ST_DIALOGBOX *,struct _Dlg500 *);
void CenterWindow(HWND,int);
HFONT SetDialogFont(HWND hwnd,char *name,int size,int type);
BOOL APIENTRY Dlg500(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);
extern void *GetDialogArguments(HWND);
extern char *GetDico(int,long);
/*@@ End Prototypes @@*/
