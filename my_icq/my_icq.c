#include <windows.h>
#include "my_icqres.h"
#include "settings.h"
#include "http_client.h"

HINSTANCE hInstance;
HANDLE WinIo;
HWND hWnd;

#define SCREEN_MAX 64000
char *screen_buffer;
int screen_index;

char *names[] = {"manager", "id", "host", "login", "password", "proxy", "port", "p_login", "p_password", "script", "interval", NULL };
char *values[] = {"manager             ","id                  ","host                ","login               ","password            ",
				  "proxy               ","port                ","p_login             ","p_password          ","script              ",
				  "interval            "};

int my_print(char *f,...)
{
  va_list argptr;
  int cnt;
  if(screen_index>=(SCREEN_MAX-1000)) return 0;
  va_start(argptr,f);
  cnt=wvsprintf(screen_buffer+screen_index,f,argptr); screen_index+=cnt;
  va_end(argptr);
  SetDlgItemText(hWnd,ID_SCREEN,screen_buffer);
  SendMessage(GetDlgItem(hWnd,ID_SCREEN),WM_VSCROLL,SB_BOTTOM,0);
  return cnt;
}

void status(char *f,...)
{
  char buf[1000]; va_list argptr;
  va_start(argptr,f); wvsprintf(buf,f,argptr); va_end(argptr); SetDlgItemText(hWnd,ID_STATUS,buf);
}

void SetMainIcon(void)
{
  HICON hIconMain;
  hIconMain=LoadIcon(hInstance,MAKEINTRESOURCE(ID_ICON_MAIN));
  SendMessage( hWnd, WM_SETICON, ICON_BIG, (LPARAM)hIconMain);
  SendMessage( hWnd, WM_SETICON, ICON_SMALL, (LPARAM)hIconMain);
}

void RestartTimer(void)
{
  int tt;
  KillTimer(hWnd,1);
  tt=atoi(values[10])*1000;
  SetTimer(hWnd,1,tt,NULL);
}

static int InitializeApp(HWND hDlg,WPARAM wParam, LPARAM lParam)
{
  hWnd=hDlg;
  screen_buffer=GlobalAlloc(GMEM_FIXED,SCREEN_MAX);
  SetMainIcon();
  LoadIniSettings(names,values,values);
  RestartTimer();
  return 1;
}

static void ShutdownApp(HWND hDlg, BOOL ret)
{
  KillTimer(hWnd,1);
  GlobalFree(screen_buffer);
  EndDialog(hDlg,ret);
}

int SendText(void)
{
  char buf[2000], b_sc[5000], out[5000];

  GetDlgItemText(hWnd,ID_TEXT,buf,2000);
  my_print("%s [%s]:\r\n%s\r\n\r\n",values[0],values[1],buf);
  SetDlgItemText(hWnd,ID_TEXT,"");
  SetFocus(GetDlgItem(hWnd,ID_TEXT));

  wsprintf(b_sc,"%s?id=%s&text=%s",values[9],values[1],url_encode(buf,out));
  GetServer(values[2],"80",b_sc,values[3],values[4],values[5],values[6],values[7],values[8],buf);

  return 1;
}

int GetText(void)
{
  char g_buf[4096], b_sc[2048];

  wsprintf(b_sc,"%s?id=%s",values[9],values[1]);
  if(GetServer(values[2],"80",b_sc,values[3],values[4],values[5],values[6],values[7],values[8],g_buf))
  {
	if(g_buf[0]!=0)
	{
	  my_print("Клиент:\r\n[%s]\r\n\r\n",g_buf);

	  SetForegroundWindow(hWnd);
	}
  }
  return 1;
}

static BOOL CALLBACK DialogFunc(HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
  switch(msg)
  {
    case WM_INITDIALOG:
		InitializeApp(hwndDlg,wParam,lParam);
		return 1;
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
			case IDOK:
				SendText();
				return 1;
			case IDCANCEL:
				ShutdownApp(hwndDlg,0);
				return 1;
			case ID_CONFIG: SettingsDialog(hInstance,hwndDlg,names,values); RestartTimer();
				return 1;
		}
		break;
	case WM_TIMER:
		GetText();
		return 1;
	case WM_CLOSE:
		ShutdownApp(hwndDlg,0);
		return 1;
	}
	return 0;
}

int APIENTRY WinMain(HINSTANCE hinst, HINSTANCE hinstPrev, LPSTR lpCmdLine, int nCmdShow)
{
  hInstance=hinst;
  return DialogBox(hinst, MAKEINTRESOURCE(IDD_MAINDIALOG), NULL, (DLGPROC) DialogFunc);
}
