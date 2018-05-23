#include <stdio.h>
#include <stdlib.h>
#include "sockets.h"

int main(int argc,char *argv[])
{
  int sock, len;
  char buf[4096];

  if(argc == 1)
  {
    puts("hzap site.ru"); return 1;
  }

  if( ( sock = open_sock(argv[1], 80) ) < 0 )
  {
    printf("Error open %s\n", argv[1]); return 1;
  }

  len = sprintf(buf, "GET / HTTP/1.1\r\nHost: %s\r\nUser-Agent: Robot\r\n\r\n", argv[1]);

  write_sock(sock, buf, len);

  len = read_sock(sock, buf, 4096);
  buf[len]=0;

  printf("%s\n",buf);

  len = read_sock(sock, buf, 4096);
  buf[len]=0;

  printf("%s\n",buf);

  return 0;
}
