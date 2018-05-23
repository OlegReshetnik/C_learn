#include <windows.h>
#include <winsock2.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <process.h>
#include <time.h>

//char root[]="C:\\Borland\\socet\\Release\\penolit\\";
//char index_file[] = "index.html";

//Cache-Control: no-cache
//If-Modified-Since: Sat, 27 Sep 2003 15:47:30 GMT
//Connection: close\n

char root[]="C:\\sibleswww\\";
char index_file[] = "index.htm";

char server_name[] = "Oleg_WEB_Server (0.8)";
char tmp_file[] = "_tmp_web_srv.html";

char html_head[]="<html><head><meta http-equiv=\"Content-Type\" content=\"text/html; charset=windows-1251\"><title>%s</title></head>\n";
char html_foot[]="<b><font size=2>Oleg_WEB_Server (v 0.8)</font></b></body></html>";

char err400[]="<html><head><meta http-equiv=\"Content-Type\" content=\"text/html; charset=windows-1251\"><meta http-equiv=\"Content-Language\" content=\"ru\">"
"<title>Непонятный запрос</title></head><body bgcolor=#FFFFFF text=#000000 leftmargin=20 topmargin=20><font color=#FF0000><b><font size=6>Ошибка 400.</font></b>"
"</font><br><i><font size=2>(сервер не понял запрос)<br>Допустимые вид запроса: <b>GET</b></font></i><br><br><br>Броузер: <b>%s</b><br><br>Host: <b>%s</b><br>"
"<br><b><font size=2>Oleg_WEB_Server (v 0.8)</font></b></body></html>";
char err404[]="<html><head><meta http-equiv=\"Content-Type\" content=\"text/html; charset=windows-1251\"><meta http-equiv=\"Content-Language\" content=\"ru\">"
"<title>Ресурс не найден</title></head><body bgcolor=#FFFFFF text=#000000 leftmargin=20 topmargin=20><font color=#FF0000><b><font size=6>Ошибка 404.</font></b>"
"</font><br><i><font size=2>(сервер не нашел требуемый ресурс)</font></i><br><br><br>Броузер: <b>%s</b><br><br>Ресурс: <b>%s</b><br><br>Host: <b>%s</b><br><br>"
"<b><font size=2>Oleg_WEB_Server (v 0.8)</font></b></body></html>";

char *days[] = {"Sun","Mon","Tue","Wed","Thu","Fri","Sat"};
char *months[] = {"Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"};


char *get_gm(char *file)
{
	char *str="Sun, 01 Jan 2000 01:01:01 GMT\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0";
	FILETIME ftWrite, ftLocal;
	SYSTEMTIME st;
	WIN32_FILE_ATTRIBUTE_DATA ad;

	GetFileAttributesEx( file, GetFileExInfoStandard, &ad);

	ftWrite=ad.ftLastWriteTime;

	FileTimeToLocalFileTime(&ftWrite, &ftLocal);

	FileTimeToSystemTime(&ftLocal, &st);

	sprintf(str,"%s, %02d %s %04d %02d:%02d:%02d GMT", days[st.wDayOfWeek], st.wDay, months[st.wMonth-1], st.wYear, st.wHour, st.wMinute, st.wSecond );

	return str;
}

void gt2ot( char *gt, char *ot )
{
	char bb[20], tt[32];
	char *p;
	int i;

	strcpy(tt,gt); *ot=0;

	if( (p=strstr(tt," GMT")) == NULL ) return;

	*p=0; p -= 8; strcpy(bb,p); // get time in bb

	*--p=0; p -= 4; strcpy(ot,p); ot += 4; *ot++=':'; // get year
	*--p=0; p -= 3; ot[0]=ot[1]='0';
	for(i=0; i<12; i++)
	{
		if( strcmp(months[i],p) == 0 ) { ot[0]=i/10+'0'; ot[1]=i%10+'0'; }
	}
	ot += 2; *ot++=':';
	*--p=0; p -= 2; *ot++=*p++; *ot++=*p++; *ot++=':'; *ot=0;
	strcat(ot,bb);

}

int old_time( char *tf, char *tq )
{
	char sf[32], sq[32];
	if( *tq == 0 ) return 0;
	gt2ot(tf,sf); gt2ot(tq,sq);
	if( strcmp(sf,sq) <= 0 ) return 1;
	return 0;
}

long flen( FILE *f )
{
	long cur, l;
	cur = ftell(f); fseek(f,0,SEEK_END); l = ftell(f); fseek(f,cur,SEEK_SET);
	return l;
}

void enc_str(char *s)
{
	while(*s) {	if( *s == ' ' ) *s = '+';  s++;	}
}

void bb(char *s)
{
	while(*s) {	if( *s == '/' ) *s = '\\';  s++; }
}

int not_exe_file(char *file)
{
	FILE *in;
	char buf[2];

	if( strstr(file,".exe") != NULL ) return 0;
	if( strstr(file,".cgi") != NULL ) return 0;

	in=fopen(file,"rb"); if( in == NULL ) return 1;

	fread(buf,1,2,in); fclose(in);

	if( buf[0] == 'M' && buf[1] == 'Z' ) return 0;

	return 1;
}

/*
void parser(char *file)
{
	char iv[]="<!--#include virtual";
	char if[]="<!--#include file";
	char ie[]="-->";


	fgets(str,inf);
	r=get_ssi(str,command,str2);
	fputs(str,outf);
	switch(r)
	{
		case 1: do_file(command,outf); break;
		case 2: dd;


	}
	fputs(str2,outf);


	if( (pb=strstr(str,iv)) != NULL )
	{
		if( (pe=strstr(pb,ie) != NULL
		{
			*pb=0; fwrite(str,1,strlen(str),outf);
			while( ( *pe == ' ' || *pe == '"' || *pe == '\'' ) && pe > pb ) pe--;
			while( ( *pb == ' ' || *pb == '"' || *pb == '\'' || *pb == '=' ) && pb < pe ) pb++;
			while( pb <= pe ) *pf++=*pb++; *pf=0;
			if( not_file(in_file) )

		}
	}


		"banner.html" -->


}
*/

int test_exec(char *s, char *quer)
{
	char proc[1024], fil[1024];

	strcpy(proc,root); strcat(proc,s); bb(proc);

	if( not_exe_file(proc) ) return 0;

	strcpy(fil,root); strcat(fil,tmp_file); bb(fil); enc_str(fil);

	printf("Ececuting %s, %s, %s\n", proc,fil,quer );
	if( spawnl( _P_WAIT,proc,s,fil,quer,NULL) != 123 ) printf("Error Exec\n");

	strcpy(s,tmp_file);
	return 1;
}

char *get_ct(char *str)
{
	if( strstr(str,".htm") != NULL ) return "text/html; charset=windows-1251";
	if( strstr(str,".css") != NULL ) return "text/css";
	if( strstr(str,".txt") != NULL ) return "text/plain";
	if( strstr(str,".gif") != NULL ) return "image/gif";
	if( strstr(str,".jp") != NULL ) return "image/jpeg";
	if( strstr(str,".bmp") != NULL ) return "image/x-xbitmap";
	return "application/octet-stream";
}

int send_buff(SOCKET sock, char *buff, char *err, char *ct, int cl)
{
	int cb, retval;
	char buf[4096];

	if( *ct == 0 ) ct="text/html; charset=windows-1251";
	if( cl == 0 ) cl = strlen(buff);

	cb=sprintf(buf,"HTTP/1.0 %s\nServer: %s\nContent-type: %s\nContent-Length: %ld\n\n",err,server_name,ct,cl );

	retval = send(sock,buf,cb,0);
	if( retval == SOCKET_ERROR ) return retval;

	return send(sock,buff,cl,0);
}

int send_err(SOCKET sock, int err, char *agent, char *str, char *host)
{
	int cb;
	char buf[4096], *serr;

	if( err == 404 ) { cb=sprintf(buf,err404,agent,str,host); serr="404 Not Found"; }
	else if( err == 400 ) { cb=sprintf(buf,err400,agent,host); serr="400 Bad Request"; }
	else return 1;

	return send_buff(sock,buf,serr,"",cb);
}

int send_file(SOCKET sock, char *file, char *err, char *ldate)
{
	long cl;
	int cb, retval;
	FILE *in;
	char buf[4096], full[1024], *t;

	strcpy(full,root); strcat(full,file); bb(full);

	puts(file); puts(ldate);

	t=get_gm(full);

	if( old_time(t,ldate) )
	{
		cb=sprintf(buf,"HTTP/1.0 304 Not modified\nServer: %s\n\n",server_name );
		puts(buf);
		return send(sock,buf,cb,0);
	}

	in=fopen(full,"rb"); cl=flen(in);

	if( *ldate == 'Z' ) cb=sprintf(buf,"HTTP/1.0 %s\nServer: %s\nContent-type: %s\nContent-Length: %ld\n\n",err,server_name,get_ct(full),cl );
	else cb=sprintf(buf,"HTTP/1.0 %s\nServer: %s\nContent-type: %s\nContent-Length: %ld\nLast-Modified: %s\n\n",
					err, server_name, get_ct(full), cl, t );

	puts(buf);

	retval = send(sock,buf,cb,0);
	if( retval == SOCKET_ERROR ) return retval;

	while( cl > 0 )
	{
		cb = fread(buf,1,4096,in);
		retval = send(sock,buf,cb,0);
		if( retval == SOCKET_ERROR ) return retval;
		cl -= cb;
	}
	fclose(in);

	return retval;
}

int not_file(char *file)
{
	FILE *in;
	char full[1024];

	strcpy(full,root); strcat(full,file); bb(full);
	in=fopen(full,"rb");
	if( in == NULL ) return 1;

	fclose(in);
	return 0;
}

void my_copy(char *d, char *s)
{
	while( *s != 0 && *s != '\r' && *s != '\n' ) *d++=*s++;
	*d=0;
}

int get_token(char *str, char *token, char *value)
{
	char *p;

	if( (p=strstr(str,token)) == NULL ) { *value=0; return 0; }

	p += strlen(token);	my_copy(value,p); return 1;
}


int get_file_query(char *str, char *file, char *query, char *ldate, char *agent, char *host)
{
	char *fp;

	*file=0; *query=0; *ldate=0;

	get_token(str,"User-Agent: ",agent);

	get_token(str,"Host: ",host);

	get_token(str,"If-Modified-Since: ",ldate);
	if( (fp=strstr(ldate,"GMT")) != NULL ) { fp += 3; *fp=0; }
	else *ldate=0;

	if( strncmp(str,"GET",3) != 0 ) return 1;
	str += 4; if( *str == '/' ) str++;

	if( (fp=strstr(str," HTTP")) == NULL ) return 2;
	*fp=0;

	fp=strchr(str,'?');
	if( fp != NULL )
	{
		*fp++=0;
		strcpy(query,fp);
	}

	if( *str == 0 ) strcpy(file, index_file);
	else strcpy(file, str);

	return 0;
}

int do_comand(SOCKET sock, char *str)
{
	char file[512], query[2048], ldate[256], agent[256], host[128];

//	printf(str); printf("\n");

	if( get_file_query(str, file, query, ldate, agent, host) != 0 ) return send_err(sock, 400, agent, "", host);

	puts(file);puts(query);puts(ldate);

	if( strcmp(file,"bardak") == 0 ) return send_buff(sock,"Ooooops!<br>Вы запросили, то, что нужно!","200 Ok","",0);

	if( not_file(file) ) return send_err(sock, 404, agent, file, host);

	if ( test_exec(file, query) ) return send_file(sock,file,"200 Ok","Z");

	return send_file(sock,file,"200 Ok",ldate);
}

int main(int argc, char **argv) {

    char Buffer[1024*2];
    unsigned short port=80;
    int retval, rr, fromlen;
    struct sockaddr_in local, from;
    WSADATA wsaData;
    SOCKET listen_socket, msgsock;

    retval = WSAStartup(0x202,&wsaData);

	if( retval != 0 )
	{
        printf("WSAStartup failed with error %d\n",retval);
        WSACleanup();
        return -1;
    }

	local.sin_family = AF_INET;
	local.sin_addr.s_addr = INADDR_ANY;
	local.sin_port = htons(port);

	listen_socket = socket(AF_INET, SOCK_STREAM, 0); // TCP socket

	if( listen_socket == INVALID_SOCKET )
	{
		printf("socket() failed with error %d\n",WSAGetLastError());
		WSACleanup();
		return -1;
    }

	if( bind(listen_socket,(struct sockaddr*)&local,sizeof(local) ) == SOCKET_ERROR )
	{
		printf("bind() failed with error %d\n",WSAGetLastError());
		WSACleanup();
		return -1;
    }


	if( listen(listen_socket,5) == SOCKET_ERROR )
	{
		printf("listen() failed with error %d\n",WSAGetLastError());
		WSACleanup();
		return -1;
	}

	printf("Listening port %d\n", port);

    while(1)
	{
		fromlen =sizeof(from);
		msgsock = accept(listen_socket,(struct sockaddr*)&from, &fromlen);

		if (msgsock == INVALID_SOCKET)
		{
			printf("accept() error %d\n",WSAGetLastError());
			WSACleanup();
			return -1;
		}

		printf("Connect from %s:%d\n", inet_ntoa(from.sin_addr), htons(from.sin_port) );

		retval = recv( msgsock, Buffer, sizeof (Buffer), 0 );

        if( retval == SOCKET_ERROR )
		{
			printf("recv() failed: error %d\n",WSAGetLastError() );
			closesocket(msgsock);
			continue;
		}

		if( retval == 0 )
		{
			printf("Client disconnect\n");
			closesocket(msgsock);
			continue;
		}

		Buffer[retval]=0; rr=retval;
        retval = do_comand(msgsock, Buffer);

		if( retval == SOCKET_ERROR ) printf("send() failed: error %d\n",WSAGetLastError() );

		printf("Server disconnect\n");
		closesocket(msgsock);
		if( retval == -99999 ) return -1;
    }
}

