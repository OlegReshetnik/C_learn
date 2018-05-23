#include <winsock.h>
#include <stdio.h>
#include <string.h>

typedef struct
{
  char* name;
  char* value;
}
values;

const char base64[]="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
int base64_encode(unsigned char *in, unsigned char *out)
{
  int j, l, l3, ls;
  char *p;

  p=out; l=strlen(in); l3=l/3; ls=0;
  for(j=0;j<l3;j++)
  {
	*out++=base64[ (*in)>>2 ];
	*out++=base64[ ((*in++&0x03)<<4) | (*in>>4) ];
	*out++=base64[ ((*in++&0x0F)<<2) | (*in>>6) ];
	*out++=base64[ *in++&0x3F ];
	ls+=4; if(ls>=72){ls=0;*out++='\n';}
  }
  l-=j*3;
  if(l==1)
  {
	*out++=base64[ (*in)>>2 ];
	*out++=base64[ (*in&0x03)<<4 ];
	*out++='='; *out++='=';
  }
  else if(l==2)
  {
	*out++=base64[ (*in)>>2 ];
	*out++=base64[ ((*in++&0x03)<<4) | (*in>>4) ];
	*out++=base64[ (*in&0x0F)<<2 ];
	*out++='=';
  }
  *out++=0;

  return out-p;
}

char *auth64(char *user, char *pass, char *auth_buf)
{
  char tmp[1000];
  if(*user) { strcpy(tmp,user); strcat(tmp,":"); strcat(tmp,pass); base64_encode(tmp,auth_buf); }
  else *auth_buf=0;
  return auth_buf;
}

void err(char *str)
{
  char buff[2048];
  wsprintf(buff,"Error: ***\n%s\n***\n", str);
  puts(buff);
//MessageBox(NULL,str,"Error",MB_OK);
}

void stt(char *str)
{
  char buff[2048];
  wsprintf(buff,"Status: ***\n%s\n***\n", str);
  puts(buff);
//MessageBox(NULL,str,"Error",MB_OK);
}

int sock_init(char *serv, int port)
{
  WSADATA rec;
  struct sockaddr_in con;
  PHOSTENT pHost;
  int sock, ll;
  PCHAR p;
  char buff[2048];

  if( WSAStartup(0x0101,&rec) )	{ err("WSAStartup"); return -1; }

  if( (sock=socket(AF_INET,SOCK_STREAM,0)) == INVALID_SOCKET ) { err("sock"); return -1; }

  if ( *serv>='0' && *serv<='9' ) con.sin_addr.s_addr=inet_addr(serv);
  else
  {
	if( !(pHost=gethostbyname(serv) ) ) { err("gethostbyname"); return -1; }
	memmove(&p,*pHost->h_addr_list,sizeof(PCHAR)); memmove(&con.sin_addr.s_addr,&p,sizeof(PCHAR));
  }
  con.sin_family=AF_INET; con.sin_port=htons(port);

  if( connect(sock,(struct sockaddr*)&con,sizeof(con))==INVALID_SOCKET ) { err("connect"); return -1; }

  wsprintf(buff,"Ok connect host: %s, port: %d", (char*)inet_ntoa(con.sin_addr), port); stt(buff);

  return sock;
}

void sock_down(int sock)
{
  if( sock > 0 ) closesocket(sock);
  WSACleanup();
}

int filesize(char *file)
{
  FILE *fp;
  int l;

  if( (fp=fopen(file,"rb")) == NULL ) return -1;
  fseek(fp,0,SEEK_END); l=ftell(fp);
  fclose(fp);
  return l;
}

int send_file(int sock, char *file)
{
  char buf[4096], msg[100];
  FILE *fp;
  int cb, tb=0;

  if( (fp=fopen(file,"rb")) == NULL ) return -1;
  do
  {
	cb=fread(buf,1,4096,fp); tb+=cb;
	if(send(sock,buf,cb,0)==SOCKET_ERROR) cb=-1;
	wsprintf(msg,"%d",tb); puts(msg);
  }
  while(cb==4096);

  fclose(fp);
  return cb;
}

char *make_upscript(char *host, char *port, char *upscript, char *proxy, char *upscript_buf)
{
  char b_port[10];

  if(*upscript='/')upscript++;
  if(proxy[0]!=0)
  {
	if(atoi(port)!=80) wsprintf(b_port,":%s",port); else b_port[0]=0;
	wsprintf(upscript_buf,"http://%s%s/%s",host,b_port,upscript);
  }
  else wsprintf(upscript_buf,"/%s",upscript);
  return upscript_buf;
}

int up_load(char *host, char *port, char *upscript, char *user, char *pass,
			char *proxy, char *proxy_port, char *proxy_user, char *proxy_pass,
			char *folder_field, char *file_field, char *in_file, char *to_folder)
{
  char boundary[]="MySuperPuperBoundary1234567890";
  char b_post[2000], b_head[2000], b_end[1000], auth[120], proxy_auth[120], b_auth[120], b_proxy_auth[120];
  char upscript_buf[256], b_refer[120], b_conn[120];
  int l_post, l_head, l_end;
  int fl, sock, al;

  if( (fl=filesize(in_file)) == -1 ) { err("Filesize"); return -1;}

  l_head=wsprintf(b_head,
				"--%s\n"
				"Content-Disposition: form-data; name=\"%s\"\n"
				"\n"
				"%s\n"
				"--%s\n"
				"Content-Disposition: form-data; name=\"%s\"; filename=\"%s\"\n"
				"Content-Type: application/octet-stream\n"
				"\n",
				boundary, folder_field, to_folder, boundary, file_field, in_file );
  l_end=wsprintf(b_end, "\n--%s--\n", boundary );

  al=l_head+fl+l_end;

  auth64(user,pass,auth);
  if(auth[0]!=0) wsprintf(b_auth,"Authorization: Basic %s\n",auth); else b_auth[0]=0;

  auth64(proxy_user,proxy_pass,proxy_auth);
  if(proxy_auth[0]!=0) wsprintf(b_proxy_auth,"Proxy-Authorization: Basic %s\n",proxy_auth); else b_proxy_auth[0]=0;

  make_upscript(host,port,upscript,proxy,upscript_buf);

  if(proxy[0]!=0) { wsprintf(b_refer,"Referer: %s\n",upscript_buf);	strcpy(b_conn,"Proxy-"); }
  else { b_refer[0]=0; b_conn[0]=0; }

  if(proxy[0]!=0) sock=sock_init(proxy,atoi(proxy_port));
  else sock=sock_init(host,atoi(port));
  if( sock == -1 ) { err("Sock init"); sock_down(sock); return -1; }

  l_post=wsprintf(b_post,
				"POST %s HTTP/1.0\n"
				"Host: %s\n"
				"%s" // refer
				"User-Agent: Mozilla/4.0 (compatible; OlegDownLoader 0.2; SV3)\n"
				"Accept: */*\n"
				"Accept-Encoding: deflate\n"
				"Accept-Language: ru\n"
				"Content-Type: multipart/form-data; boundary=%s\n"
				"Content-Length: %d\n"
				"%sConnection: close\n" // connection
				"Cache-Control: no-cache\n"
				"%s" // Auth
				"%s" // Proxy Auth
				"\n",
				upscript_buf, host, b_refer, boundary, al, b_conn, b_auth, b_proxy_auth );

  stt(b_post);
  if( send(sock,b_post,l_post,0)==SOCKET_ERROR ) { err("Send POST"); sock_down(sock); return -1; }

  stt(b_head);
  if( send(sock,b_head,l_head,0)==SOCKET_ERROR ) { err("Send head"); sock_down(sock); return -1; }

  if( send_file(sock,in_file)==-1 ) { err("Send file"); sock_down(sock); return -1; }

  if( send(sock,b_end,l_end,0)==SOCKET_ERROR ) { err("Send end"); sock_down(sock); return -1; }

  sock_down(sock);
  return 1;
}

char *get_name(char *str, char *name)
{
  char *p1, *p2;

  p2=strstr(str,": "); if(p2==NULL) return 0;
  p1=strchr(str,'\n'); if(p1==NULL) return 0;
  p1++; if(p1>p2)p1=str;
  while(p1<p2)*name++=*p1++;
  *name=0;
  return (p2+2);
}

char *get_value(char *str, char *val)
{
  while(*str)
  {
	if(*str=='\r') str++;
	else if(*str=='\n')
	{
	str++;
	if(*str==' ' || *str=='\t') { *val++=' '; str++; }
	else { *val=0; return str; }
	}
	else { *val++=*str++; }
  }
  *val=0; return str;
}

int parse_buff(char* buff, values* val)
{
  char buf_name[2000], buf_value[2000];
  int n;

  n=0;
  while(buff=get_name(buff,buf_name))
  {
	buff=get_value(buff,buf_value); n++;
	strcpy(val->name=malloc(strlen(buf_name)+5),buf_name);
	strcpy(val->value=malloc(strlen(buf_value)+5),buf_value);
	val++;
  }
  val->name=NULL;
  return n;
}

void free_values(values *val)
{
  while(val->name!=NULL) { free(val->name); free(val->value); val++; }
}

char *get_file(char *file)
{
  FILE *fp;
  int cur, l;
  char *buf;

  if( (fp=fopen(file,"rb")) == NULL ) return NULL;
  cur=ftell(fp); fseek(fp,0,SEEK_END); l=ftell(fp); fseek(fp,cur,SEEK_SET);

  if(l==0) { fclose(fp); return NULL; }
  if((buf=malloc(l+1))==NULL) { fclose(fp); return NULL; }
  fread(buf,l,1,fp); fclose(fp); buf[l]=0;
  return buf;
}

int load_config(char *conf_file, values *val)
{
  char *fc;
  int n;
  if( (fc=get_file(conf_file))==NULL ) return 0;
  n=parse_buff(fc,val); free(fc);
  return n;
}

int get_by_name(values *val, char *name, char *get_value, char *def_value)
{
  while(val->name!=NULL)
  {
	if(strcmp(val->name,name)==0) { strcpy(get_value,val->value); return 1; }
	val++;
  }
  strcpy(get_value,def_value); return 0;
}

void load_settings(char *exe,
				char *host, char *port, char *upscript, char *user, char *pass,
				char *proxy, char *proxy_port, char *proxy_user, char *proxy_pass,
				char *folder_field, char *file_field, char *in_file, char *to_folder)
{
  values val[20];
  int n;

  n=strlen(exe);  if( exe[n-4]=='.' ) exe[n-4]=0; strcat(exe,".cfg");
  stt(exe);
  load_config(exe,val);

  get_by_name(val,"host",host,"localhost");
  get_by_name(val,"port",port,"80");
  get_by_name(val,"upscript",upscript,"/admin/___d___.php");

  get_by_name(val,"user",user,"z");
  get_by_name(val,"pass",pass,"z");

  get_by_name(val,"proxy",proxy,"");
  get_by_name(val,"proxy_port",proxy_port,"");
  get_by_name(val,"proxy_user",proxy_user,"");
  get_by_name(val,"proxy_pass",proxy_pass,"");

  get_by_name(val,"folder_field",folder_field,"___folder___");
  get_by_name(val,"file_field",file_field,"___file___");
  get_by_name(val,"in_file",in_file,exe);
  get_by_name(val,"to_folder",to_folder,"");

  free_values(val);
}


int main(int argc, char* argv[])
{
  char host[100], port[10], upscript[256], user[120], pass[120];
  char proxy[100], proxy_port[10], proxy_user[120], proxy_pass[120];
  char folder_field[100], file_field[100], in_file[256], to_folder[120];

  load_settings(argv[0], host, port, upscript, user, pass, proxy, proxy_port, proxy_user, proxy_pass, folder_field, file_field, in_file, to_folder);

  up_load(host, port, upscript, user, pass, proxy, proxy_port, proxy_user, proxy_pass, folder_field, file_field, in_file, to_folder);

  return 1;
}
