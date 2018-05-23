// sockets.h - функции для работы с сокетами

#ifndef __sockets_h__
#define	__sockets_h__

#include <string.h>
#include <winsock.h>

#define ER_SC_INIT -1
#define ER_SC_SOCK -2
#define ER_SC_HOST -3
#define ER_SC_CONN -4
#define ER_SC_LIST -5
#define ER_SC_BIND -6

u_long get_addr(char *serv);
int init_sock(void);
int new_sock(void);
void close_sock(int sock);
int open_sock(char *serv, int port);
int listen_sock(char *serv, int port);
int accept_sock(int sock, char *serv, int *port);
int read_sock(int sock, char *buf, int l);
int write_sock(int sock, char *buf, int l);

#endif
