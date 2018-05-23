#include <windows.h>

#define IDD_CONFIG_DIALOG 500
#define ID_NAMES 501
#define ID_VALUES 502
#define ID_SAVE 503
#define ID_RESET 504
#define ID_EXIT 505

char **main_names;
char **main_values;
HWND hConfigWnd;

void get_ini_file(char *file)
{
  file += GetModuleFileName(NULL,file,1000)-3;
  *file++='i'; *file++='n'; *file='i';
}

int LoadIniSettings(char **names, char **default_values, char **values)
{
  int n=0;
  char ini_file[1000];
  get_ini_file(ini_file);
  while(names[n]!=NULL) { GetPrivateProfileString("config",names[n],default_values[n],values[n],100,ini_file); n++; }
  return n;
}

int SaveIniSettings(char **names, char **values)
{
  int n=0;
  char ini_file[1000];
  get_ini_file(ini_file);
  while(names[n]!=NULL) { WritePrivateProfileString("config",names[n],values[n],ini_file); n++; }
  return n;
}

void LoadSettingsAndShow(void)
{
  char buf[10000];
  int i;

  LoadIniSettings(main_names,main_values,main_values);

  buf[0]=0; i=0;
  while(main_names[i]!=NULL) { lstrcat(buf,main_names[i++]); lstrcat(buf,"\r\n"); }
  SetDlgItemText(hConfigWnd,ID_NAMES,buf);

  buf[0]=0; i=0;
  while(main_names[i]!=NULL) { lstrcat(buf,main_values[i++]); lstrcat(buf,"\r\n"); }
  SetDlgItemText(hConfigWnd,ID_VALUES,buf);
}

void SaveSettings(void)
{
  char buf[10000], *p, *s;
  int i;

  i=GetDlgItemText(hConfigWnd,ID_VALUES,buf,10000);
  if(buf[i-2]!='\r') { buf[i++]='\r'; buf[i++]='\n'; buf[i]=0; }
  p=s=buf; i=0;
  while(*p || main_names[i]!=NULL )
  {
	while( *s != '\r' )s++;
	*s=0; lstrcpy(main_values[i++],p); p=s+2; s=p;
  }
  SaveIniSettings(main_names, main_values);
}

void QuerySaveSettings(void)
{
  if( MessageBox(hConfigWnd, "Сохранить изменения ?", "Настройки", MB_YESNO) == IDYES ) SaveSettings();
}

static BOOL CALLBACK ConfigDialogFunc(HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
  switch(msg)
  {
    case WM_INITDIALOG: hConfigWnd=hwndDlg; LoadSettingsAndShow(); return 1;
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		  case ID_SAVE: SaveSettings(); return 1;
		  case ID_RESET: LoadSettingsAndShow(); return 1;
		  case ID_EXIT: QuerySaveSettings(); EndDialog(hwndDlg,0); return 1;
		}
		break;
	case WM_CLOSE: QuerySaveSettings(); EndDialog(hwndDlg,0); return 1;
	}
	return 0;
}

int SettingsDialog(HINSTANCE hInst,HWND hWnd,char **names,char **values)
{
  main_names=names; main_values=values;
  return DialogBox(hInst, MAKEINTRESOURCE(IDD_CONFIG_DIALOG), hWnd, (DLGPROC)ConfigDialogFunc);
}
