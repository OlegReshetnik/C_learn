#include "smtp.h"
#include "base64.h"
#include <stdio.h>

char *days[] = {"Sun","Mon","Tue","Wed","Thu","Fri","Sat","\0"};
char *months[] = {"UUU","Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec","\0"};
char *eol="\r\n";

int smtp_err(char* ans)
{
  int vv;

  ans[3]=0;
  vv = atoi(ans);
  if( vv >= 400 ) return 1;
  return 0;
}

int cmd_smtp_server(int sk, char* cmd)
{
  char s[64000];
  int l;

  strcpy(s,cmd); strcat(s,eol);
  if( write_sock(sk,s,strlen(s)) <= 0 ) return -1;
  printf(s);
  if( (l=read_sock(sk,s,1024)) <= 0 ) return -1;
  s[l]=0;
  printf(s);
  if( smtp_err(s) ) return -1;

  return 1;
}

int open_smtp_server(char *serv, char *user, char *pass)
{
  int sk, pp;
  char buf[1000], ss[100];

  if( (sk=open_sock(serv,25)) < 0 ) return -1;
  if( read_sock(sk,buf,1000) <= 0 ) { close_sock(sk); return -2; }
  if( smtp_err(buf) ) { close_sock(sk); return -3; }

  get_sock_info(sk, ss, &pp);
  strcpy(buf,"EHLO "); strcat(buf,ss);

  if( cmd_smtp_server(sk,buf) < 0 ) { close_sock(sk); return -4; }
  if( cmd_smtp_server(sk,"AUTH LOGIN") < 0 ) { close_sock(sk); return -5; }

  base64_encode_str(user, buf);
  if( cmd_smtp_server(sk,buf) < 0 ) { close_sock(sk); return -6; }

  base64_encode_str(pass, buf);
  if( cmd_smtp_server(sk,buf) < 0 ) { close_sock(sk); return -6; }

  return sk;
}

char *get_gmt(char *buf)
{
  SYSTEMTIME st;

  GetLocalTime(&st);
  wsprintf(buf,"%s, %02d %s %04d %02d:%02d:%02d GMT", days[st.wDayOfWeek],st.wDay,months[st.wMonth],st.wYear,st.wHour,st.wMinute,st.wSecond );
  return buf;
}

int smtp_mail(int sk, char *from, char *to, char *subj, char *data)
{
  char buf[1000], tt[200];

  get_gmt(tt);

  strcpy(buf, "MAIL FROM: "); strcat(buf, from);
  if( cmd_smtp_server(sk,buf) < 0 ) { close_sock(sk); return -1; }

  strcpy(buf, "RCPT TO: "); strcat(buf, to);
  if( cmd_smtp_server(sk,buf) < 0 ) { close_sock(sk); return -2; }

  if( cmd_smtp_server(sk,"DATA") < 0 ) { close_sock(sk); return -3; }

  // Content-Type: text/plain; charset=\"windows-1251\"\r\nMIME-Version: 1.0\r\n

  sprintf(buf,"From: %s <%s>\r\nTo: %s <%s>\r\nDate: %s\r\nSubject: %s\r\n\r\n%s\r\n.",from,from,to,to,tt,subj,data);

  if( cmd_smtp_server(sk, buf) < 0 ) { close_sock(sk); return -4; }

  return 1;
}

void close_smtp_server(int sk)
{
  cmd_smtp_server(sk,"QUIT");
  close_sock(sk);
}
