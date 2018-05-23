/*=============================================================================
*
*       Simple POP3 box checker (Console, win32, Win API source code )
*
*       1999, 2000 Alex Shilonosov
*
*       Compiled on VisualC++ 6.0 (no packs)
*		(Windows 95/ 98/ NT/ NT2000)
*		
*
*       The author of this program may be contacted at shilonosov@mail.ru (@mail.md)
*       
*
*       visit:  www.shilonosov.f2s.com  ( www.shilonosov.da.ru ) 
*               for more source code and software.
*     
*       ѕрограмерам которые не знают англ. €зыка:
*				–еб€та учите енглиш, все равно пригодитс€.
*      
*=============================================================================
*/

// .exe mail.ru[:110] name:pass


#include "winsock.h"
#include <stdio.h>

int s,x;

//FILE *f_out;

bool is_error(char* text)   { return strncmp("+",text,3); }

int send_to(char* chr )
{
	//printf(chr);
    if ( send(s,chr,strlen(chr),0)==SOCKET_ERROR )
	{
		printf("error send: %d\n",WSAGetLastError());
		return 0;
	}
	return 1;
}

bool reciv_and_noerr(char* chr )
{
	//  return true;
	int len;
	
	if ( (len=(recv(s,chr,5490,0    )))==SOCKET_ERROR )
	{ printf("error recv %d \n",WSAGetLastError()); return false;}
	
	*(chr+len-1)=0;
	
	printf(chr);
	printf("\n");
	
	
	return is_error(chr);
}


// cl tiny.cpp /MD /Og /Os /link /ALIGN:0x10 /merge:.rdata=.data

//*********************************************************************
//*********************************************************************
int main(int argc, char* argv[])
{
	if (argc < 3)
	{                
		printf("\n============================================================");
		printf("\nPOP3 email box checker  by Shilonosov.A.  (shilonosov@mail.ru ..@mail.md)" ) ;
		printf("\nsee: www.shilonosov.f2s.com, www.shilonosov.da.ru");
		printf("\nQMAIL.EXE [@I.P]POP3_Server[:port] user:pass");
		printf("\noptions :\n");                
		printf("\n");                
		printf(" programm output:  answers of SMTP server\n (errorlevel=1 if succes)\n" );
		printf(" examples:\n");
		printf("pop3.exe pop3.mail.com myname:mypass\n");                
		printf("============================================================\n");
		
		return 0;
	}
	
	WSADATA rec;
	struct sockaddr_in con;
	PHOSTENT pHost;
	char* text= new char[5000];

	int err=0;
	char *s2;
	char server[100];
	char port[10];	
	
	
	strcpy(server,argv[1]);
	strcpy(port,"110");
	
	if ( strchr(server,':') ) // another port
	{
		strncpy(port,strchr(server,':')+1,5) ;
		*strchr(server,':')  = 0;
	}
	
	
	//Open Sokets
	int wVer = MAKEWORD(1, 1);
	if ( WSAStartup(wVer,&rec) )
	{
        printf("error WSAstarup \n");
        return 0;
	}
	
	if ((s = socket(AF_INET, SOCK_STREAM,0)) == INVALID_SOCKET)
	{ printf("Socket Error:     %d \n",WSAGetLastError()); goto exit;}
	//resolve
	if ( server[0]=='@' ) con.sin_addr.s_addr=inet_addr(server+1);
	else
	{
		printf("Look up for POP3 server....\n%s",server);
		if ( !(pHost=gethostbyname(server) ) )
		{ printf("Cant find (error %d)\n",WSAGetLastError()); goto exit; }
		//
		PCHAR p;
		memmove(&p,*pHost->h_addr_list,sizeof(PCHAR));
		memmove(&con.sin_addr.s_addr,&p,sizeof(PCHAR));
	}
	printf("\n");
	printf((char*)inet_ntoa(con.sin_addr));
	printf("\n");
	
	con.sin_family=AF_INET;
	con.sin_port=htons( atoi(port) );
	//conect
	if ( connect(s,(struct sockaddr*)&con,sizeof(con))==INVALID_SOCKET )
	{ printf("error connect %d\n",WSAGetLastError()); goto exit; }
	printf("Connected...\n");	
	
	if ( !reciv_and_noerr(text) ) goto exit;
	
	//Talk to SMTP server begin....
	
	s2=strchr( argv[2],':');
	if ( s2 ) *s2=0; else goto exit;
	
	strcpy(text,"user "); strcat(text,argv[2]);  strcat(text,"\xD\xA\x0");
	if ( !send_to( text) ) goto exit;
	if ( !reciv_and_noerr(text) ) goto exit;
	//if ( !
	strcpy(text,"pass "); strcat(text,s2+1);  strcat(text,"\xD\xA\x0");
	if ( !send_to( text) ) goto exit;
	if ( !reciv_and_noerr(text) ) goto exit;
	
	strcpy(text,"QUIT"); strcat(text,"\xD\xA\x0");
	if ( !send_to( text) ) goto exit;
	if ( !reciv_and_noerr(text) ) goto exit;                                
	
exit:
	delete text;
	closesocket(s);
	WSACleanup();
	return 1;
}


