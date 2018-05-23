#include "base64.h"

const char base64[]="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

int base64_decode_str(unsigned char* in, unsigned char* out)
{
  return base64_decode(in,out,strlen(in));
}

int base64_decode(unsigned char* in, unsigned char* out, int l)
{
  int i, ld;
  unsigned int b[4], d;
  char *p;

  ld=0; p=out;
  while(l>0)
  {
    for(i=0;i<4;)
    {
      d=*in++; l--;
      switch(d)
      {
        case '\t':
        case '\n':
        case '\r':
        case ' ': break;
        case '=': d='A'; ld--;
        default: b[i++]=strchr(base64,d)-base64; break;
      }
    }
    *out++=((b[0]<<2)&0xFC) | ((b[1]>>4)&0x03);
    *out++=((b[1]<<4)&0xF0) | ((b[2]>>2)&0x0F);
    *out++=((b[2]<<6)&0xC0) | b[3]&0x3F;
    ld+=3;
  }
  p[ld]=0;
  return ld;
}

int base64_encode_str(unsigned char *in, unsigned char *out)
{
  return base64_encode(in,out,strlen(in));
}

int base64_encode(unsigned char *in, unsigned char *out, int l)
{
  int j, l3, ls;
  char *p;

  p=out;

  l3=l/3; ls=0;
  for(j=0;j<l3;j++)
  {
    *out++=base64[ (*in)>>2 ];
	*out++=base64[ ((*in++&0x03)<<4) | (*in>>4) ];
	*out++=base64[ ((*in++&0x0F)<<2) | (*in>>6) ];
	*out++=base64[ *in++&0x3F ];
	ls+=4; if(ls>=72){ls=0;*out++='\n';}
  }
  l-=j*3;
  if(l==1)
  {
	*out++=base64[ (*in)>>2 ];
	*out++=base64[ (*in&0x03)<<4 ];
    *out++='='; *out++='=';
  }
  else if(l==2)
  {
    *out++=base64[ (*in)>>2 ];
	*out++=base64[ ((*in++&0x03)<<4) | (*in>>4) ];
	*out++=base64[ (*in&0x0F)<<2 ];
	*out++='=';
  }
  *out++=0;

  return out-p;
}

char *auth64(char *user, char *pass, char *auth_buf)
{
  char tmp[1000];
  if(*user) { strcpy(tmp,user); strcat(tmp,":"); strcat(tmp,pass); base64_encode_str(tmp,auth_buf); }
  else *auth_buf=0;
  return auth_buf;
}
