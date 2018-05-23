#include "pop3.h"

int pop3_err(char* ans)
{
  if( strncmp("-ERR",ans,4)==0 ) return 1;
  return 0;
}

int cmd_pop3_server(int sk, char* cmd, char* buff, int l)
{
  char s[1024];
  int len;

  strcpy(s,cmd); strcat(s,"\r\n");
  if( write_sock(sk,s,strlen(s)) <= 0 ) return -1;
  if( (len=read_sock(sk,buff,l)) <=0 ) return -1;
  if( pop3_err(buff) ) return -1;

  return len;
}

int open_pop3_server(char *serv, char *user, char *pass)
{
  int sk,l;
  char buf[1000];

  if( (sk=open_sock(serv,110)) < 0 ) return -1;
  if( read_sock(sk,buf,1000) <= 0 ) { close_sock(sk); return -2; }
  if( pop3_err(buf) ) { close_sock(sk); return -3; }
  wsprintf(buf,"USER %s",user); if( cmd_pop3_server(sk,buf,buf,1000) < 0 ) { close_sock(sk); return -4; }
  if( pop3_err(buf) ) { close_sock(sk); return -4; }
  wsprintf(buf,"PASS %s",pass); if( cmd_pop3_server(sk,buf,buf,1000) < 0 ) { close_sock(sk); return -5; }
  if( pop3_err(buf) ) { close_sock(sk); return -5; }

  return sk;
}

void close_pop3_server(int sk)
{
  char buf[1000];
  cmd_pop3_server(sk,"QUIT",buf,1000);
  close_sock(sk);
}

int delete_msg_pop3_server(int sk, int n)
{
  char buf[1000];
  wsprintf(buf,"DELE %d",n);
  return cmd_pop3_server(sk,buf,buf,1000);
}

msg_info *list_messages_pop3_server(int sk)
{
  char buff[6000], ss[3560];
  char* mass[560];
  char* m[200];
  int i, j, nn, n_mess;
  msg_info *mb;
  values *val;

  mb=malloc(sizeof(msg_info)*100);

  mb[0].to=0;
  if( cmd_pop3_server(sk,"LIST",buff,6000) <= 0 ) return mb;
  nn=split_str(buff,"\r\n",mass); split_str(mass[0]," ",m);	n_mess=atoi(m[1]);
  if(n_mess<=0) return 0;

  for(i=0;i<n_mess;i++)
  {
    split_str(mass[i+1]," ",m); mb[i].size=atoi(m[1]);
  }

  if( cmd_pop3_server(sk,"UIDL",buff,6000) <=0 ) return mb;
  split_str(buff,"\r\n",mass);
  for(i=0;i<n_mess;i++)
  {
    split_str(mass[i+1]," ",m); mb[i].uidl=malloc(strlen(m[1])+5); strcpy(mb[i].uidl,m[1]);
  }

  for(i=0;i<n_mess;i++)
  {
    mb[i].to=0;
	wsprintf(ss,"TOP %d 0",i+1); if( cmd_pop3_server(sk,ss,buff,6000) <= 0 ) return mb;
    val=get_values(buff);
    get_by_name(val,"Date",ss,"-"); mb[i].date=malloc(strlen(ss)+5); strcpy(mb[i].date,ss);
    get_by_name(val,"From",ss,"-"); mb[i].from=malloc(strlen(ss)+5); strcpy(mb[i].from,ss);
    get_by_name(val,"To",ss,"-"); mb[i].to=malloc(strlen(ss)+5); strcpy(mb[i].to,ss);
    get_by_name(val,"Subject",ss,"-"); mb[i].subj=malloc(strlen(ss)+5); strcpy(mb[i].subj,ss);
	free_values(val);
  }

  mb[n_mess].to=0;
  return mb;
}

void free_messages(msg_info *mb)
{
  msg_info *p;
  p=mb;
  while(p->to) { free(p->date); free(p->from); free(p->to); free(p->subj); free(p->uidl); }
  free(mb);
}
