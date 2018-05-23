#include <windows.h>
#include <winsock2.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void err(char *str)
{
  char buff[2048];
  wsprintf(buff,"Error: %s", str);
  puts(buff);
//MessageBox(NULL,str,"Error",MB_OK);
}

void stt(char *str)
{
  char buff[2048];
  wsprintf(buff,"Msg: %s", str);
  puts(buff);
//MessageBox(NULL,str,"Error",MB_OK);
}

SOCKET server_socket_init(int port) // port 21 - FTP, 80 - http
{
  WSADATA rec;
  SOCKET listen_sock;
  struct sockaddr_in local;

  if( WSAStartup(0x202,&rec) ) { err("WSAStartup failed"); WSACleanup(); return -1; }

  listen_sock=socket(AF_INET, SOCK_STREAM, 0);

  if(listen_sock==INVALID_SOCKET) { err("Error listen socket"); WSACleanup(); return -1; }

  local.sin_family = AF_INET; local.sin_addr.s_addr = INADDR_ANY; local.sin_port = htons(port);
  if( bind(listen_sock,(struct sockaddr*)&local,sizeof(local) ) == SOCKET_ERROR ) { err("bind() failed"); WSACleanup(); return -1; }

  if( listen(listen_sock,5) == SOCKET_ERROR ) { err("listen() failed"); WSACleanup(); return -1; }

  stt("Listening"); return listen_sock;
}

void socket_down(SOCKET sock)
{
  if( sock > 0 ) closesocket(sock);
  WSACleanup();
}

int main(int argc, char **argv)
{
  SOCKET listen_socket, msgsock;
  int flag;
  char msg[1024];
  char Buffer[1024*10];

  char *ans="HTTP/1.0 200 OK\nServer: Test\nContent-type: text/plain\nContent-Length: 10\nConnection: closed\n\nLa-la-la\n\n\n\n\n";
  char *ans2="HTTP/1.0 200 OK\nServer: Test\nTransfer-Encoding: chunked\nContent-Type: text/plain\n\n";

  int retval, rr, fromlen;
  struct sockaddr_in local, from;

  listen_socket=server_socket_init(8080);

  if( listen_socket <=0 ) { flag=0; err("Flag=0"); }
  else flag=1;

  while(flag)
  {
	fromlen=sizeof(from);
	msgsock = accept(listen_socket,(struct sockaddr*)&from, &fromlen);
	if(msgsock == INVALID_SOCKET) { err("accept() error"); flag=0; continue; }

    wsprintf(msg,"Connect from %s:%d",inet_ntoa(from.sin_addr),htons(from.sin_port)); stt(msg);

    retval=recv(msgsock,Buffer,sizeof(Buffer),0);
    if(retval == SOCKET_ERROR) { err("recv() failed"); closesocket(msgsock); continue; };
    if(retval == 0) { stt("Client disconnect"); closesocket(msgsock); continue; };
	Buffer[retval]=0; puts(Buffer);
	send(msgsock,ans,strlen(ans),0);

	closesocket(msgsock);
  }

  socket_down(listen_socket);
}
