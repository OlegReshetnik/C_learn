// values.h - функции для работы со списками параметров

#ifndef __values_h__
#define	__values_h__

#include <string.h>
#include <malloc.h>


typedef struct
{
  char *name;
  char *value;
}
values;

char *get_name(char *str, char *name);
char *get_value(char *str, char *val);
values *get_values(char* buff);
void free_values(values *val);
int get_by_name(values *val, char *name, char *get_value, char *def_value);
int split_str(char* str, char* mask, char** mass);

#endif
