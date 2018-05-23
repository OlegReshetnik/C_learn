#include "pop3.h"
#include "base64.h"


static char koi8tbl[]="şàáöäåôãõèéêëìíîïÿğñòóæâüûçøıù÷úŞÀÁÖÄÅÔÃÕÈÉÊËÌÍÎÏßĞÑÒÓÕÂÜÛÇØİÙ×Ú";

char *koi8_to_windows1251(unsigned char *s)
{
  unsigned char c, d, *pr;
  pr=s;

  while(c=*s)
  {
	if(c>=0xC0) d=koi8tbl[c-0xC0];
	else if(c==0xA3) d='¸';
	else if(c==0xB3) d='¨';
	else d=c;
	*s++=d;
  }
  return pr;
}

char hex2bin(char d)
{
  if( d >= '0' && d <= '9' ) return d-'0';
  if( d >= 'A' && d <= 'F' ) return d-'A'+10;
  if( d >= 'a' && d <= 'f' ) return d-'a'+10;
  return 0;
}

int quoted_decode(char *in, char *out, int l)
{
  int nn=0;
  while(l)
  {
	if(*in=='=') { in++; *out++=hex2bin(*in++)*16+hex2bin(*in++); nn++; l-=3; }
	else if(*in=='_') { in++; *out++=' '; nn++; l--; }
	else { *out++=*in++; nn++; l--; }
  }
  *out=0;
  return nn;
}

int pop_decode(char *in, char *out, int l)
{
  int nn=0;
  char *p, bb[100];

  p=strchr(in,'?'); if(p==NULL) return 0;
  nn=p-in; l-=nn;
  strncpy(bb,in,nn); bb[nn]=0; in=p+1;
  if(*in=='B' || *in=='b') nn=base64_decode(in+2,out,l-3);
  else if(*in=='Q' || *in=='q') nn=quoted_decode(in+2,out,l-3);
  else return 0;
  CharLower(bb);
  if(strcmp(bb,"koi8-r")==0) koi8_to_windows1251(out);
 // âñòàâèòü êîä ïåğåêîäèğîâêè
  return nn;
}

char *decode_string(char *str)
{
  char *p1, *p2, *p, *out;
  char buff[10000];
  int nn;

  out=buff; p=str;
  while(*str)
  {
    p1=strstr(str,"=?");
	if(p1!=NULL) { p2=strchr(p1+2,'?'); p2=strstr(p2+3,"?="); }
	else p2=NULL;
	if( p1 != NULL || p2 != NULL )
	{
      nn=pop_decode(p1+2,out,p2-p1-2);
	  out+=nn; str=p2+2;
    }
	else
	{
	  while(*str) *out++=*str++;
	}
  }
  *out=0;
  strcpy(p,buff);
  return p;
}

int pop3_err(char* ans)
{
  if( strncmp("-ERR",ans,4)==0 ) return 1;
  return 0;
}

int cmd_pop3_server(int sk, char* cmd, int point, char* buff, int l)
{
  char s[1024];
  int len, l2;

  strcpy(s,cmd); strcat(s,"\r\n");
  if( write_sock(sk,s,strlen(s)) <= 0 ) return -1;
  if( (len=read_sock(sk,buff,l)) <=0 ) return -1;
  if( pop3_err(buff) ) return -1;

  buff+=len; l-=len; *buff=0; l2=len;
  while(point && strcmp(buff-5,"\r\n.\r\n")!=0)
  {
    if( (len=read_sock(sk,buff,l)) <=0 ) return -1;
    buff+=len; l-=len; *buff=0; l2+=len;
  }
  return l2;
}

int open_pop3_server(char *serv, char *user, char *pass)
{
  int sk,l;
  char buf[1000];

  if( (sk=open_sock(serv,110)) < 0 ) return -1;
  if( read_sock(sk,buf,1000) <= 0 ) { close_sock(sk); return -2; }
  if( pop3_err(buf) ) { close_sock(sk); return -3; }
  wsprintf(buf,"USER %s",user); if( cmd_pop3_server(sk,buf,0,buf,1000) < 0 ) { close_sock(sk); return -4; }
  wsprintf(buf,"PASS %s",pass); if( cmd_pop3_server(sk,buf,0,buf,1000) < 0 ) { close_sock(sk); return -5; }

  return sk;
}

void close_pop3_server(int sk)
{
  char buf[1000];
  cmd_pop3_server(sk,"QUIT",0,buf,1000);
  close_sock(sk);
}

int delete_msg_pop3_server(int sk, int n)
{
  char buf[1000];
  wsprintf(buf,"DELE %d",n);
  return cmd_pop3_server(sk,buf,0,buf,1000);
}

msg_info *list_messages_pop3_server(int sk)
{
  char buff[20000], ss[5000];
  char* mass[1000];
  char* m[1000];
  int i, j, nn, n_mess;
  msg_info *mb;
  values *val;

  mb=malloc(sizeof(msg_info)*200);

  mb[0].to=0;
  if( cmd_pop3_server(sk,"LIST",1,buff,20000) <= 0 ) return mb;

  nn=split_str(buff,"\r\n",mass); split_str(mass[0]," ",m);	n_mess=atoi(m[1]);

  if(n_mess<=0) return mb;

  for(i=0;i<n_mess;i++)
  {
    split_str(mass[i+1]," ",m); mb[i].size=atoi(m[1]);
  }

  if( cmd_pop3_server(sk,"UIDL",1,buff,20000) <=0 ) return mb;
  split_str(buff,"\r\n",mass);
  for(i=0;i<n_mess;i++)
  {
    split_str(mass[i+1]," ",m); mb[i].uidl=malloc(strlen(m[1])+5); strcpy(mb[i].uidl,m[1]);
  }

  for(i=0;i<n_mess;i++)
  {
    mb[i].to=0;
	wsprintf(ss,"TOP %d 0",i+1); if( cmd_pop3_server(sk,ss,1,buff,20000) <= 0 ) return mb;
    val=get_values(buff);
    get_by_name(val,"Date",ss,"-"); mb[i].date=malloc(strlen(ss)+5); strcpy(mb[i].date,ss);
    get_by_name(val,"From",ss,"-"); decode_string(ss); mb[i].from=malloc(strlen(ss)+5); strcpy(mb[i].from,ss);
    get_by_name(val,"To",ss,"-"); decode_string(ss); mb[i].to=malloc(strlen(ss)+5); strcpy(mb[i].to,ss);
    get_by_name(val,"Subject",ss,"-"); decode_string(ss); mb[i].subj=malloc(strlen(ss)+5); strcpy(mb[i].subj,ss);
	free_values(val);
  }

  mb[n_mess].to=0;
  return mb;
}

void free_messages(msg_info *mb)
{
  msg_info *p;
  p=mb;
  while(p->to) { free(p->date); free(p->from); free(p->to); free(p->subj); free(p->uidl); p++; }
  free(mb);
}
