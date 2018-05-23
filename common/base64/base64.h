// base64.h - функции для паковки-распаковки base64

#ifndef __base64_h__
#define	__base64_h__

#include <string.h>

int base64_decode_str(unsigned char* in, unsigned char* out);
int base64_decode(unsigned char* in, unsigned char* out, int l);
int base64_encode_str(unsigned char *in, unsigned char *out);
int base64_encode(unsigned char *in, unsigned char *out, int l);
char *auth64(char *user, char *pass, char *auth_buf);

#endif
