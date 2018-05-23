#include <stdio.h>
#include <stdlib.h>
#include "smtp.h"

int main(int argc,char *argv[])
{
  int sk;
  char a1[]="sloboda@sloboda.ru";
  char p1[]="ckj,jlf";
  char s1[]="smtp.yandex.ru";
  char a2[]="byte2000@inbox.ru";
  char p2[]="gopa";
  char s2[]="smtp.inbox.ru";
  char a3[]="79500522230@sms.bwc.ru";

  sk = open_smtp_server(s2,a2,p2);

  smtp_mail(sk, a2, a3, "Проверка", "Текст письма");

  close_smtp_server(sk);

  return 0;
}
