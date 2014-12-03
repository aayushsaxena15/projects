#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string>
#include <sys/types.h>
#include <dirent.h>
#include <fcntl.h>
#include <limits.h>
#include <netinet/in.h>
#include <signal.h>
#include <vector>
#include <map>
#include <algorithm>
#include <iostream>
#include <unistd.h>
#include <errno.h>
#include <sys/sendfile.h>
#include <arpa/inet.h>
#include <math.h>
#include <netdb.h>
#include <time.h>
#include <fnmatch.h>
#include <fstream>
#include <sys/stat.h>
#include <openssl/md5.h>
#include <sys/mman.h>
#include "globals.h"
#include "times.h"
#include "checksum.h"
#include "getindex.h"
#include "functions.h"
#include "history.h"
void client(int port,char *ip);
void traverse();
void serversetup(int port,char *ip);
void listentoclient();

int overallflag=0,tttt=0;

int main(int argc,char *argv[])
{
	char ip[30];
	if(argc!=5)
	{
		printf("Input error\nCorrect format : ./PRP <Path to conf folder> <Port Num1> <Port Num2> <Ip Address>\n");
		exit(0);
	}
	strcpy(confFolder,argv[1]);
	int p1,p2;
	traverse();
	p1=atoi(argv[2]);
	p2=atoi(argv[3]);
	strcpy(ip,argv[4]);
	readfromindexfile(pathtoondexfile); //read from indexFile.conf
	readfromsharedfile(pathtosharedFolder); //read from sharedFolder.conf
	writetofile(pathtoondexfile); // write to indexFile.conf
	fileindex.clear();
	updateFileIndex(pathtoondexfile);
	if(fork()==0)
	{
		sleep(5);
		client(p1,ip);
		if(errno==2)
			kill(getpid(),SIGKILL);
	}
	else
	{
		serversetup(p2,ip);
	}
	return 0;
}
void serversetup(int port,char *ip)
{
	struct sockaddr_in server,client;
	int socket_of_client,read_buffer_size;
	char messagefromclient[chunk+10];
	int sd=socket(AF_INET,SOCK_STREAM,0);
	int g;
	if(sd==-1)
		printf("Error in socket creation.\n");
	server.sin_family=AF_INET;
	server.sin_addr.s_addr=INADDR_ANY;
	server.sin_port=htons(port);
	
	

	int hflag=0;

	if(bind(sd,(struct sockaddr *)&server,sizeof(server))<0)
	{
		perror("Binding Error.\n");
		hflag=1;
		exit(0);
	}
	listen(sd,3);
	int c=sizeof(struct sockaddr_in);
	socket_of_client=accept(sd,(struct sockaddr *)&client,(socklen_t*)&c);
	if(socket_of_client<0)
	{
		perror("failed to accept connection.\n");
		exit(0);
	}
	while((read_buffer_size=recv(socket_of_client,messagefromclient,sizeof(messagefromclient),0))>0)
	{
		messagefromclient[read_buffer_size]='\0';
		printf("recieved message---------------%s\n",messagefromclient);
		char *parsed;
		int i=0,flagwrong=0;
		char *temp[6];
		parsed=strtok(messagefromclient," ");
		while(1)
		{
			if(parsed==NULL)
				break;
			temp[i++] = parsed;
			parsed = strtok(NULL," ");
		}
		if(!strcmp("IndexGet",temp[0]))
		{
			if(!strcmp("ShortList",temp[1]))
			{ 
				ShortList(stringtoint(temp[2]),stringtoint(temp[3]));
				sendtoclient(socket_of_client,"temp","./temp");
				remove("temp");
			}
			else if(!strcmp("LongList",temp[1]))
			{
				LongList();
				sendtoclient(socket_of_client,"temp","./temp");
				remove("temp");
			}
			else if(!strcmp("RegEx",temp[1]))
			{
				Regex(temp[2]);
				sendtoclient(socket_of_client,"temp","./temp");
				remove("temp");
			}
			else
				flagwrong = 1;
		}
		else if(!strcmp("FileHash",temp[0]))
		{
			if(!strcmp("Verify",temp[1]))
			{
				filehashing(temp[2]);
				sendtoclient(socket_of_client,"temp","./temp");
				remove("temp");
			}
			else if(!strcmp("CheckAll",temp[1]))
			{
				filechecking();
				sendtoclient(socket_of_client,"temp","./temp");
				remove("temp");
			}
			else
				flagwrong=1;
		}
		else if(!strcmp("FileDownload",temp[0]))
		{
			sendtoclient(socket_of_client,temp[0],temp[1]);
		}
		else if(!strcmp("FileUpload",temp[0]))
		{
			if(uploadflag)
			{
				printf("Permission granted\n");
				printf("file to be uploaded-------------- %s\n",temp[1]);
				uploadtoserver(socket_of_client,temp[1]);
				downfromclient(temp[1],socket_of_client);
			}
			else
				printf("permission denied \n");
		}
		else
			flagwrong = 1;

		if(flagwrong)
			printf("wrong input.\n");
	}
	if(read_buffer_size==0)
	{
		//puts("Client Disconnected");
		printf("Client Disconnected");
	}
	else if(read_buffer_size==-1)
	{
		perror("failed to receive data\n");
	}

}
void client(int port,char *ip)
{
	struct sockaddr_in server;
	char message[chunk+10];
	int tempsock=socket(AF_INET,SOCK_STREAM,0);
	int i;
	if(tempsock==-1)
		printf("error in socket creation\n");
	server.sin_addr.s_addr=inet_addr(ip);
	server.sin_family=AF_INET;
	server.sin_port=htons(port);
	if(connect(tempsock,(struct sockaddr *)&server,sizeof(server))<0)
	{
		perror("unable to connect\n");
		exit(0);
	}
	while(1)
	{
		char ch=getchar();
		i=0;
		int tempbind;
		while(1)
		{
			if(ch=='\n')
				break;
			message[i++]=ch;
			ch=getchar();
		}
		message[i]='\0';
		printf("message sent ------------ %s\n",message);
		tempbind=send(tempsock,message,strlen(message),0);
		if(tempbind<0)
		{
		//	puts("Send failed.");
			printf("failed to send");
			exit(0);
		}
		receivefromserver(tempsock);
	}
	close(tempsock);
}
void listentoclient()
{
	char command[30];
	int flagwrong=0;
	scanf("%s",command);
	if(!strcmp(command,"IndexGet"))
	{
		char command1[30];
		scanf("%s",command1);
		if(!strcmp(command1,"ShortList"))
		{
			long int start,end;
			scanf("%ld%ld",&start,&end);
			ShortList(start,end);
		}
		else if(!strcmp(command1,"LongList"))
		{
			LongList();
		}
		else if(!strcmp(command1,"RegEx"))
		{
			char command2[30];
			scanf("%s",command2);
			Regex(command2);
		}
		else
			flagwrong=1;
	}
	else if(!strcmp(command,"FileHash"))
	{
		char command1[30];
		scanf("%s",command1);
		if(!strcmp(command1,"Verify"))
		{
			char command2[PATH_MAX];
			scanf("%s",command2);
			filehashing(command2);
		}
		else if(!strcmp(command1,"CheckAll"))
		{
			filechecking();
		}
		else
			flagwrong=1;
	}
	else if(!strcmp(command,"FileDownload"))
	{
		char command1[30];
		scanf("%s",command1);
	}
	else if(!strcmp(command,"FileUpload"))
	{
		char command1[30];
		scanf("%s",command1);
	}
	else
		flagwrong=1;
}
void traverse()
{
	sprintf(pathtoondexfile,"%sIndexFile.conf",confFolder);
	if(access(pathtoondexfile,F_OK)==-1)
	{
		FILE *fp=fopen(pathtoondexfile,"w");
		fclose(fp);
	}
	sprintf(pathtosharedFolder,"%sSharedFolder.conf",confFolder);
	if(access(pathtosharedFolder,F_OK) == -1)
	{
		FILE *fp=fopen(pathtosharedFolder,"w");
		fclose(fp);
	}
	sprintf(tempFilePath,"%stemp\n",confFolder);
}
