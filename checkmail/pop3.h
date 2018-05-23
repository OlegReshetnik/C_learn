// pop3.h - функции для работы с почтовым сервером

#ifndef __pop3_h__
#define	__pop3_h__

#include "sockets.h"
#include "values.h"
#include <windows.h>

typedef struct msg_info
{
  char *date;
  char *from;
  char *to;
  char *subj;
  char *uidl;
  int  size;
}
msg_info;


char *decode_string(char *str);
int pop3_err(char* ans);
int cmd_pop3_server(int sk, char* cmd, int point, char* buff, int l);
int open_pop3_server(char *serv, char *user, char *pass);
void close_pop3_server(int sk);
msg_info *list_messages_pop3_server(int sk);
void free_messages(msg_info *mb);

#endif
