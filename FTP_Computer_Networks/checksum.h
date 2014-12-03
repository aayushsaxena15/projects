unsigned long sizeoffile(char *fileName)
{
	int filepointer=open(fileName,O_RDONLY);
	int temp=0;
	if(filepointer< 0)
	{
		printf("error opening the given file\n");
		temp=1;
		return -1;
	}

	struct stat sbuffer;
	fstat(filepointer, &sbuffer);

	close(filepointer);
	
	return sbuffer.st_size;
}

void printhash(unsigned char *s)
{
	int i;
	for(i=0;i<MD5_DIGEST_LENGTH;i++)
		printf("%02x",s[i]);
	printf("\n");
}

void printhash(FILE *fd,unsigned char *s)
{
	int i;
	for(i=0;i<MD5_DIGEST_LENGTH;i++)
		fprintf(fd,"%02x",s[i]);
	fprintf(fd,"\n");
}

unsigned char *getmd5hashing(char *filename)
{
	int filepointer=open(filename,O_RDONLY);
	if(filepointer<0)
	{
		printf("error opening the file\n");
		return NULL;
	}
	unsigned long filesize=sizeoffile(filename);

	char* fileBuffer;
	fileBuffer=(char *)mmap(0,filesize,PROT_READ,MAP_SHARED,filepointer,0);

	static unsigned char md5Hash[MD5_DIGEST_LENGTH];
	MD5((unsigned char*)fileBuffer, filesize, md5Hash);

	close(filepointer);

	return md5Hash;

}
