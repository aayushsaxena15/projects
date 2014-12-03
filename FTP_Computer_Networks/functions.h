void ShortList(long int t1,long int t2)
{
	FILE *fp=fopen("temp","w");
	for(int i=0;i<fileindex.size();i++)
	{
		if(fileindex[i].fileTime>=t1&&fileindex[i].fileTime<=t2)
		{
			fprintf(fp,"%s\t%s\t%d\t%ld\t%s\n",fileindex[i].fileName,fileindex[i].filePath,
					fileindex[i].fileSize,fileindex[i].fileTime,fileindex[i].fileType);
		}
	}
	fclose(fp);
}

void LongList()
{
	FILE *fp=fopen("temp","w");
	for(int i=0;i<fileindex.size();i++)
	{
		fprintf(fp,"%s\t%s\t%d\t%ld\t%s\n",fileindex[i].fileName,fileindex[i].filePath,
				fileindex[i].fileSize,fileindex[i].fileTime,fileindex[i].fileType);
	}
	fclose(fp);
}

void Regex(const char *regEx)
{
	FILE *fp = fopen("temp","w");
	for(int i=0; i<fileindex.size(); i++)
	{
		if(!fnmatch(regEx,fileindex[i].fileName,FNM_CASEFOLD))
		{
			fprintf(fp,"%s\t%s\t%d\t%ld\t%s\n",fileindex[i].fileName,fileindex[i].filePath,
					fileindex[i].fileSize,fileindex[i].fileTime,fileindex[i].fileType);
		}
	}
	fclose(fp);
}

void filehashing(char *name_of_file)
{
	fileindex.clear();
	readfromindexfile(pathtoondexfile); //read from indexFile.conf
	readfromsharedfile(pathtosharedFolder); //read from sharedFolder.conf
	writetofile(pathtoondexfile); // write to indexFile.conf
	fileindex.clear();
	updateFileIndex(pathtoondexfile); //read from indexFile.conf again


	FILE *fp = fopen("temp","w");
	for(int i=0; i<fileindex.size(); i++)
	{
		if( ! strcmp(name_of_file,fileindex[i].fileName) )
		{
			fprintf(fp,"%s\t%s\t%ld\t",fileindex[i].fileName,fileindex[i].filePath,fileindex[i].fileTime);
			printhash(fp,fileindex[i].fileHash);
		}
	}
	fclose(fp);
}

void filechecking()
{
	//Check All
	fileindex.clear();
	readfromindexfile(pathtoondexfile); //read from indexFile.conf
	readfromsharedfile(pathtosharedFolder); //read from sharedFolder.conf
	writetofile(pathtoondexfile); // write to indexFile.conf
	fileindex.clear();
	updateFileIndex(pathtoondexfile); //read from indexFile.conf again


	FILE *fp = fopen("temp","w");
	for(int i=0; i<fileindex.size(); i++)
	{
		fprintf(fp,"%s\t%s\t%ld\t",fileindex[i].fileName,fileindex[i].filePath,fileindex[i].fileTime);
		printhash(fp,fileindex[i].fileHash);
	}
	fclose(fp);
}

void sendtoclient(int sock_client,char *name_of_file,char *filePath)
{
	int size_of_file;
	char filehash[50];
	long fileTime;
	int i=0;
	if(!strcmp("temp",name_of_file))
	{
		size_of_file=sizeoffile(filePath);
		fileTime=get_file_time(filePath);
		strcpy(filehash,"NULL");
	}
	else
	{
		for(i=0;i<fileindex.size(); i++)
		{
			if(!strcmp(fileindex[i].filePath,filePath) )
				break;
		}
		int temp=i;
		fileTime = fileindex[temp].fileTime;
		size_of_file=fileindex[temp].fileSize;
		strcpy(name_of_file,fileindex[temp].fileName);
		filehash[0]='\0';
		int j=0;

		while(j<MD5_DIGEST_LENGTH)
		{
			char s[10];
			sprintf(s,"%02x",fileindex[temp].fileHash[temp]);
			strcat(filehash,s);
			j++;
		}
	}

	FILE *fp = fopen(filePath,"rb");
	int fd = open(filePath,O_RDONLY);

	char fileHeader[chunk];
	bzero(fileHeader, chunk);
	sprintf(fileHeader,"%s\t%d\t%ld\t%s",name_of_file,size_of_file,fileTime,filehash);
	printf("file header---------------- %s\n",fileHeader);
	int le=strlen(fileHeader);
	int y=send(sock_client,fileHeader,le,0);
	if(y<0)
	{
		//puts("Send failed.");
		printf("failed to send the data");
		exit(0);
	}

	int sent = 0;
	int tempsize = size_of_file/chunk;
	for(int i=0; i<tempsize; i++)
	{
		char buffer[chunk+10];
		size_t n = fread ( buffer, sizeof(char), chunk , fp );
		bzero(buffer, chunk+10);
		buffer[n] = '\0';
		sent += n;
		int y = send(sock_client , buffer ,n, 0);
		if(y<0)
		{
		//	puts("Send failed.");
			printf("failed to send the data");
			exit(0);
		}
	}

	tempsize=size_of_file%chunk;
	if(tempsize)
	{
		char buffer[chunk+10];
		bzero(buffer, chunk+10);
		size_t n=fread(buffer,sizeof(char),tempsize,fp);
		sent=sent+n;
		buffer[n] = '\0';
		int y=send(sock_client,buffer,n,0);
		if(y<0)
		{
			//puts("Send failed.");
			printf("failed to send the data");
			exit(0);
		}

	}
	printf("data sent ---------- %d\n",sent);
	fclose(fp);
}

void sendtoserver(int sock_server,char *name_of_file)
{
	int i = 0;
	for(i=0; i<fileindex.size(); i++)
	{
		if( !strcmp(fileindex[i].filePath,name_of_file) )
			break;
	}
	int x = i;
	int   size_of_file = fileindex[x].fileSize;
	long int fileTime = fileindex[x].fileTime;
	char filehash[50];
	filehash[0] = '\0';
	for(int i=0; i < MD5_DIGEST_LENGTH; i++)
	{
		char s[10];
		sprintf(s,"%02x",fileindex[x].fileHash[i]);
		strcat(filehash,s);
	}

	FILE *fp = fopen(name_of_file,"rb");

	char fileHeader[chunk];
	bzero(fileHeader, chunk);
	sprintf(fileHeader,"%s\t%d\t%ld\t%s",name_of_file,size_of_file,fileTime,filehash);
	//write(sock_server_client , fileHeader , strlen(fileHeader));
	printf("header of file----------- %s\n",fileHeader);
	int y = send(sock_server , fileHeader , strlen(fileHeader), 0);
	if( y < 0)
	{
		puts("Send failed.");
		exit(0);
	}

	int sent = 0;
	//Send File
	x = size_of_file/chunk;
	for(int i=0; i<x; i++)
	{
		char buf[chunk+10];
		bzero(buf, chunk+10);
		size_t n = fread ( buf, sizeof(char), chunk , fp );
		buf[n] = '\0';
		sent += n;
		//write(sock_server_client , buf , n);
		int y = send(sock_server, buf ,n, 0);
		if( y < 0)
		{
			puts("Send failed.");
			exit(0);
		}
	}

	x = size_of_file % chunk;
	if(x)
	{
		char buf[chunk+10];
		bzero(buf, chunk+10);
		size_t n = fread ( buf, sizeof(char), x , fp );
		sent += n;
		buf[n] = '\0';
		//write(sock_server_client , buf , n);
		int y = send(sock_server , buf ,n, 0);
		if( y < 0)
		{
			puts("Send failed.");
			exit(0);
		}
	}
	printf("Total Sent == %d\n",sent);
	fclose(fp);
}

void downfromclient(char *name_of_file,int sock)
{
	char replyfromserver[chunk];
	bzero(replyfromserver, chunk);

	//Recieve FileSize
	int size;
	size = recv(sock , replyfromserver , sizeof(replyfromserver) , 0);
	if(size < 0)
	{
		puts("Recieve failed.");
		return;
	}
	char *token;
	char *values[4];
	int i = 0;
	token = strtok(replyfromserver,"\t");
	while(token !=NULL)
	{
		values[i++] = token;
		token = strtok(NULL,"\t");
	}
	int size_of_file = atoi(values[1]);

	char newFileName[50];
	int m = 0;
	for(i=strlen(name_of_file)-1;i>=0;i--)
	{
		if(name_of_file[i] == '/')
			break;
	}
	int k = 0;
	for(m=i+1;m<strlen(name_of_file);m++)
		newFileName[k++] = name_of_file[m];
	newFileName[k] = '\0';

	printf("FileName == %s\n",newFileName);
	FILE *fp = fopen(newFileName,"wb");

	int total_recieve = 0;

	int x = size_of_file / chunk;
	for(int i=0; i<x; i++)
	{
		char replyfromserver[chunk];
		bzero(replyfromserver, chunk);
		size = recv(sock , replyfromserver , sizeof(replyfromserver) , 0);
		total_recieve += size;
		if(size < 0)
		{
			puts("Recieve failed.");
			return;
		}
		replyfromserver[size] = '\0';
		fwrite(replyfromserver,sizeof(char),size,fp);
	}
	x = size_of_file % chunk;
	if(x)
	{
		char replyfromserver[chunk];
		bzero(replyfromserver, chunk);
		size = recv(sock , replyfromserver , sizeof(replyfromserver) , 0);
		total_recieve += size;
		if(size < 0)
		{
			puts("Recieve failed.");
			return;
		}
		replyfromserver[size] = '\0';
		fwrite(replyfromserver,sizeof(char),size,fp);
	}
	printf("Total Recieved == %d\n",total_recieve);
	fclose(fp);
	printf("Recieving Done.\n");

	printf("Checking Hash Value...\n");
	printf("Hash Value Recieved == %s\n",values[3]);
	unsigned char *tempHash;
	tempHash = getmd5hashing(newFileName);

	unsigned char filehash[50];
	for(int i=0; i<MD5_DIGEST_LENGTH; i++)
		filehash[i] = (hextodec(values[3][2*i]) << 4 ) + hextodec(values[3][2*i+1]);

	printf("Hash Value Calculated == ");
	int check = 1;
	for(int i=0; i < MD5_DIGEST_LENGTH; i++)
	{
		printf("%02x",tempHash[i]);
		if(filehash[i] != tempHash[i])
			check = 0;
	}
	printf("\n");
	if( check )
		printf("File Recieved Correctly.\n");
	else
		printf("File Recieved Incorrectly.Some Chunks Lost.\n");

	return;
}


void receivefromserver(int sock)
{
	char replyfromserver[chunk];
	bzero(replyfromserver, chunk);

	//Recieve FileSize
	int size;
	size = recv(sock , replyfromserver , sizeof(replyfromserver) , 0);
	if(size < 0)
	{
		puts("Recieve failed.");
		return;
	}

	char *token;
	char *values[4];
	int i = 0;
	token = strtok(replyfromserver,"\t");
	while(token !=NULL)
	{
		values[i++] = token;
		token = strtok(NULL,"\t");
	}
	int size_of_file = atoi(values[1]);

	if( !strcmp("Allow",values[3]))
	{
		//Send File from client to server
		sendtoserver(sock,values[0]);
		return;
	}
	else if( strcmp(values[3],"NULL") )
	{
		downloadFileFromServer(size_of_file,values[0],sock,values[3]);
		return;
	}

	int total_recieve = 0;
	//Recieve File
	int x = size_of_file / chunk;
	for(int i=0; i<x; i++)
	{
		char replyfromserver[chunk];
		bzero(replyfromserver, chunk);
		size = recv(sock , replyfromserver , sizeof(replyfromserver) , 0);
		total_recieve += size;
		if(size < 0)
		{
			puts("Recieve failed.");
			return;
		}
		replyfromserver[size] = '\0';
		printf("%s",replyfromserver);
	}

	x = size_of_file % chunk;
	if(x)
	{
		char replyfromserver[chunk];
		bzero(replyfromserver, chunk);
		size = recv(sock , replyfromserver , x , 0);
		total_recieve += size;
		if(size < 0)
		{
			puts("Recieve failed.");
			return;
		}
		replyfromserver[size] = '\0';
		printf("%s",replyfromserver);
	}
	printf("Total Recieved == %d\n",total_recieve);

	printf("Recieving Done.\n");

	return;
}

void uploadtoserver(int sock_client,char *name_of_file)
{
	char fileHeader[chunk];
	bzero(fileHeader, chunk);
	sprintf(fileHeader,"%s\t0\t0\tAllow",name_of_file);
	printf("%s\n",fileHeader);
	int y = send(sock_client , fileHeader , strlen(fileHeader), 0);
	if( y < 0)
	{
		puts("Send failed.");
		exit(0);
	}
	return;
}

void downloadFileFromServer(int size_of_file,char *name_of_file,int sock,char *hash)
{
	FILE *fp = fopen(name_of_file,"wb");

	int x = size_of_file / chunk;
	int size;
	int total_recieve = 0;
	for(int i=0; i<x; i++)
	{
		char replyfromserver[chunk];
		bzero(replyfromserver, chunk);
		size = recv(sock , replyfromserver , sizeof(replyfromserver) , 0);
		total_recieve += size;
		if(size < 0)
		{
			puts("Recieve failed.");
			return;
		}
		replyfromserver[size] = '\0';
		fwrite(replyfromserver,sizeof(char),size,fp);
		//printf("%s",replyfromserver);
	}
	x = size_of_file % chunk;
	if(x)
	{
		char replyfromserver[chunk];
		bzero(replyfromserver, chunk);
		size = recv(sock , replyfromserver , sizeof(replyfromserver) , 0);
		total_recieve += size;
		if(size < 0)
		{
			puts("Recieve failed.");
			return;
		}
		replyfromserver[size] = '\0';
		fwrite(replyfromserver,sizeof(char),size,fp);

		//printf("%s",replyfromserver);
	}
	printf("Total Recieved == %d\n",total_recieve);
	fclose(fp);
	printf("Recieving Done.\n");

	printf("Checking Hash Value...\n");
	printf("Hash Value Recieved == %s\n",hash);
	unsigned char *tempHash;
	tempHash = getmd5hashing(name_of_file);

	unsigned char filehash[50];
	for(int i=0; i<MD5_DIGEST_LENGTH; i++)
		filehash[i] = (hextodec(hash[2*i]) << 4 ) + hextodec(hash[2*i+1]);

	printf("Hash Value Calculated == ");
	int check = 1;
	for(int i=0; i < MD5_DIGEST_LENGTH; i++)
	{
		printf("%02x",tempHash[i]);
		if(filehash[i] != tempHash[i])
			check = 0;
	}
	printf("\n");
	if( check )
		printf("File Recieved Correctly.\n");
	else
		printf("File Recieved Incorrectly.Some Chunks Lost.\n");

	return;
}

