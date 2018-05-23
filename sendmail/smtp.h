// smtp.h - функции для работы с почтовым сервером

#ifndef __smtp_h__
#define	__smtp_h__

#include "sockets.h"
#include <windows.h>
#include <string.h>

int smtp_err(char* ans);
int cmd_smtp_server(int sk, char* cmd);
int open_smtp_server(char *serv, char *user, char *pass);
char *get_gmt(char *buf);
int smtp_mail(int sk, char *from, char *to, char *subj, char *data);
void close_smtp_server(int sk);

#endif
