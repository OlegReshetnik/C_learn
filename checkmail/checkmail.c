#include "pop3.h"
#include <stdio.h>

void print_msg_info(msg_info *mb)
{
  char b[10000], b2[20];
  while(mb->to)
  {
	strcpy(b,"Дата: "); strcat(b,mb->date); puts(b);
	strcpy(b,"От: "); strcat(b,mb->from); puts(b);
	strcpy(b,"Кому: "); strcat(b,mb->to); puts(b);
	strcpy(b,"Тема: "); strcat(b,mb->subj); puts(b);
	strcpy(b,"Размер: "); strcat(b,itoa(mb->size,b2,10) ); puts(b);
	strcpy(b,"Id: "); strcat(b,mb->uidl); puts(b);
    puts(" ");
    mb++;
  }
}

int main(int argc, char* argv[])
{
  msg_info *mb;
  int sk;

  if( argc < 4 ) { puts("checkmail pop3.mail.com myname mypass"); return 0; }

  if( (sk=open_pop3_server(argv[1],argv[2],argv[3])) < 0 ) { puts("Error open pop3 server:"); puts(argv[1]); return 0; }

  mb=list_messages_pop3_server(sk);

  print_msg_info(mb);

  free_messages(mb);

  close_pop3_server(sk);

  return 1;
}
