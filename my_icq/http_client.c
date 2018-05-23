#include "sockets.h"
#include "base64.h"

char user_agent[]="Mozilla/4.0 (compatible; OlegDownLoader 0.2; SV3)";

char *url_encode(unsigned char *s, char *out)
{
  char *p;
  p=out;
  while(*s) p+=wsprintf(p,"%%%02X",*s++);
  return out;
}

int GetServer(char *host, char *port, char *script, char *user, char *pass, char *proxy, char *p_port, char *p_user, char *p_pass, char *response)
{
  char b_get[32000], auth[128], b_auth[256], p_auth[128], b_p_auth[256], b_conn[32], b_port[32], b_script[2048], *p;
  int l_get, sock, prt;

  *response=0;
  if(proxy[0]!=0) prt=atoi(p_port); else prt=atoi(port);
  if( (sock=open_sock(host,prt)) <= 0 ) return 0;

  if(port[0]!='8' && port[1]!='0') wsprintf(b_port,":%s",port); else b_port[0]=0;

  auth64(user,pass,auth);
  if(auth[0]!=0) wsprintf(b_auth,"Authorization: Basic %s\r\n",auth); else b_auth[0]=0;

  auth64(p_user,p_pass,p_auth);
  if(p_auth[0]!=0) wsprintf(b_p_auth,"Proxy-Authorization: Basic %s\r\n",p_auth); else b_p_auth[0]=0;

  if(*script=='/')script++;
  if(proxy[0]!=0) { strcpy(b_conn,"Proxy-"); wsprintf(b_script,"http://%s%s/%s",host,b_port,script); }
  else { b_conn[0]=0; wsprintf(b_script,"/%s",script); }

  l_get=wsprintf(b_get,
				"GET %s HTTP/1.0\r\n"
				"Host: %s%s\r\n"
				"User-Agent: %s\r\n"
				"Accept: */*\r\n"
				"Accept-Encoding: deflate\r\n"
				"Accept-Language: ru\r\n"
				"%sConnection: close\r\n" // connection
				"%s" // Auth
				"%s" // Proxy Auth
				"\r\n",
				b_script, host, b_port, user_agent, b_conn, b_auth, b_p_auth );

  if(write_sock(sock,b_get,l_get) <= 0 ) { close_sock(sock); return 0; }
  if( (l_get=read_sock(sock,b_get,4096)) <= 0 ) { close_sock(sock); return 0; }
  b_get[l_get]=0;

  p=b_get;
  int last=0;
  while(*p)
  {
	if(*p=='\n')
	{
	  if(last) { p++; break; }
	  else last=1;
	}
	else if(*p!='\r') last=0;
	p++;
  }
  lstrcpy(response,p);
  close_sock(sock);
  return 1;
}
