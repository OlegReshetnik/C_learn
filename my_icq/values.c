#include "values.h"

char *get_name(char *str, char *name)
{
  char *p1, *p2, *pt, *ps;

  ps=strstr(str,": "); pt=strstr(str,":\t"); if(pt==NULL) pt=ps; if(pt==NULL) return 0;
  if(ps<pt)p2=ps; else p2=pt;
  p1=strchr(str,'\n'); if(p1==NULL) return 0;
  p1++; if(p1>p2)p1=str;
  while(p1<p2)*name++=*p1++;
  *name=0;
  return (p2+2);
}

char *get_value(char *str, char *val)
{
  while(*str)
  {
    if(*str=='\r') str++;
    else if(*str=='\n')
    {
      str++;
      if(*str==' ' || *str=='\t') { *val++=' '; str++; }
      else { *val=0; return str; }
    }
    else { *val++=*str++; }
  }
  *val=0; return str;
}

values *get_values(char* buff)
{
  values *val, *p;
  char *pn, *pv;
  pn=malloc(10000); pv=malloc(10000); p=val=malloc(sizeof(values)*500);
  while(buff=get_name(buff,pn))
  {
    buff=get_value(buff,pv);
    strcpy(val->name=malloc(strlen(pn)+5),pn);
    strcpy(val->value=malloc(strlen(pv)+5),pv);
    val++;
  }
  val->name=NULL;
  return p;
}

void free_values(values *val)
{
  values *p;
  p=val;
  while(val->name!=NULL) { free(val->name); free(val->value); val++; }
  free(p);
}

int get_by_name(values *val, char *name, char *get_value, char *def_value)
{
  while(val->name!=NULL)
  {
    if(strcmp(val->name,name)==0) { strcpy(get_value,val->value); return 1; }
    val++;
  }
  strcpy(get_value,def_value); return 0;
}

int split_str(char* str, char* mask, char** mass)
{
  int n, l;
  char* p;

  l=strlen(mask); if(l==0) return 0;
  p=strstr(str,mask); if(p==NULL) return 0;
  n=0;

  while(p!=NULL)
  {
    *p=0; *mass++=str; n++; str=p+l; p=strstr(str,mask);
  }
  *mass=str; n++;

  return n;
}
