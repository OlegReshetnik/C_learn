#include "sockets.h"

int sock_count=0;

u_long get_addr(char *serv)
{
  HOSTENT *hst;
  u_long ret;

  if ( (ret=inet_addr(serv)) == INADDR_NONE )
  {
    if( !(hst=gethostbyname(serv) ) ) return INADDR_NONE;
    ret=((u_long **)hst->h_addr_list)[0][0];
  }
  return ret;
}

int init_sock(void)
{
  WSADATA rec;
  int ret;
  if( sock_count<=0 ) ret=WSAStartup(0x0101,&rec); else ret=1;
  return ret;
}

int new_sock(void)
{
  int ret;
  ret=socket(AF_INET,SOCK_STREAM,0);
  if( ret!=INVALID_SOCKET ) sock_count++;
  return ret;
}

void close_sock(int sock)
{
  if( closesocket(sock)!=SOCKET_ERROR ) sock_count--;
  if( sock_count<=0 ) WSACleanup();
}

int open_sock(char *serv, int port)
{
  struct sockaddr_in con;
  int sock;

  if( init_sock() ) return ER_SC_INIT;

  if( (con.sin_addr.s_addr=get_addr(serv)) == INADDR_NONE ) return ER_SC_HOST;
  con.sin_family=AF_INET; con.sin_port=htons(port);

  if( (sock=new_sock()) == INVALID_SOCKET ) return ER_SC_SOCK;

  if( connect(sock,(struct sockaddr*)&con,sizeof(con))==INVALID_SOCKET ) { close_sock(sock); return ER_SC_CONN; }

  return sock;
}

int listen_sock(char *serv, int port)
{
  struct sockaddr_in con;
  int list_sock;

  if( init_sock() ) return ER_SC_INIT;

  if( (con.sin_addr.s_addr=get_addr(serv)) == INADDR_NONE ) return ER_SC_HOST;
  con.sin_family=AF_INET; con.sin_port=htons(port);

  if( (list_sock=new_sock()) == INVALID_SOCKET ) return ER_SC_SOCK;

  if( bind(list_sock,(struct sockaddr*)&con,sizeof(con) ) == SOCKET_ERROR ) { close_sock(list_sock); return ER_SC_BIND; }

  if( listen(list_sock,5) == SOCKET_ERROR ) { close_sock(list_sock); return ER_SC_LIST; }

  return list_sock;
}

int accept_sock(int sock, char *serv, int *port)
{
  struct sockaddr_in from;
  int fromlen, msgsock;

  fromlen =sizeof(from);
  if( (msgsock=accept(sock,(struct sockaddr*)&from, &fromlen)) == INVALID_SOCKET ) return ER_SC_SOCK;
  sock_count++; strcpy(serv,inet_ntoa(from.sin_addr)); *port=htons(from.sin_port);
  return msgsock;
}

int get_sock_info(int sock, char *serv, int *port)
{
  struct sockaddr_in from;
  int fromlen, ret;

  fromlen =sizeof(from);
  ret = getsockname(sock,(struct sockaddr*)&from, &fromlen);
  strcpy(serv,inet_ntoa(from.sin_addr)); *port=htons(from.sin_port);

  return ret;
}

int read_sock(int sock, char *buf, int l)
{
  return recv(sock,buf,l,0);
}

int write_sock(int sock, char *buf, int l)
{
  int ret, r;
  ret=0;
  while( (r=send(sock,buf,l,0)) > 0 )
  {
    buf+=r; l-=r; ret+=r;
    if(l<=0) return ret;
  }
  if(r==SOCKET_ERROR) return 0;
  return ret;
}
