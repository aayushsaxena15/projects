const char *getFileType(char *filename)
{
	const char *temp = strrchr(filename, '.');
	if(!temp || temp == filename)
		return "NULL";

	return temp + 1;
}

void details_of_the_file(char *filepath,char *filename)
{
	int length;
	length=strlen(filepath)+5;
	int i,filepointer;
	files tempFile;
	struct stat sbuffer;
	filepointer=open(filepath,O_RDONLY);
	fstat(filepointer, &sbuffer);

	tempFile.filePath=(char *)malloc(length);

	strcpy(tempFile.filePath,filepath);
	tempFile.fileTime=(long)sbuffer.st_mtime;
	map<string,long int>::iterator it;
	it=fileLocTime.find(filepath);
	if(it!=fileLocTime.end())
		if (tempFile.fileTime == it->second)
			return;
	if(it != fileLocTime.end())
		if(tempFile.fileTime != it->second)
			deleteLocTime[it->first] = it->second;
	length=strlen(filename)+5;
	tempFile.fileName = (char *) malloc (length);
	strcpy(tempFile.fileName,filename);
	const char *temp;
	temp =getFileType(filename);
	length = strlen(temp) + 5;
	tempFile.fileType = (char *) malloc (length);
	strcpy(tempFile.fileType,temp);
	tempFile.fileSize = sbuffer.st_size;
	unsigned char *temp2;
	temp2 = getmd5hashing(filepath);
	for(i=0; i<MD5_DIGEST_LENGTH; i++)
		tempFile.fileHash[i] = temp2[i];
	fileindex.push_back(tempFile);
	close(filepointer);
	return;
}
void detailsofindexedfiles()
{
	int k;
	for(k=0;k<fileindex.size();k++)
	{
		printf("%s\n",fileindex[k].filePath);
		printf("-----------------------------------\n");
		printf("%s\n",fileindex[k].fileName);
		printf("-----------------------------------\n");
		printf("%d\n",fileindex[k].fileSize);
		printf("-----------------------------------\n");
		printf("%ld\n",fileindex[k].fileTime);
		printf("-----------------------------------\n");
		printf("%s\n",fileindex[k].fileType);
		printf("-----------------------------------\n");
		printhash(fileindex[k].fileHash);
		printf("-----------------------------------\n");
	}
}
void sharedfiles(char *folderpath,int level)
{
	DIR *dir;
	struct dirent *label;
	if(!(dir=opendir(folderpath)))
	{
		return;
	}
	if(!(label=readdir(dir)))
	{
		return;
	}
	do
	{
		if(label->d_type==DT_DIR)
		{
			if(!strcmp(label->d_name, ".")||!strcmp(label->d_name, "..") )
				continue;

			char newpath[PATH_MAX];
			int length = snprintf(newpath, sizeof(newpath)-1, "%s/%s", folderpath,label->d_name);
			newpath[length] = 0;

			sharedfiles(newpath, level + 1);
		}
		else if(label->d_type == DT_REG)
		{
			char absolute[PATH_MAX];

			char newpath[PATH_MAX];
			int length=snprintf(newpath,sizeof(newpath)-1,"%s/%s",folderpath,label->d_name);
			newpath[length]=0;

			realpath(newpath,absolute);

			if(fileLocTime.find(absolute) != fileLocTime.end())
				checkFileExist[absolute] = 1;
			else
				checkFileExist[absolute] = 0;

			details_of_the_file(absolute,label->d_name);
		}
	}
	while (label=readdir(dir));
	closedir(dir);
}
long int stringtoint(char *a)
{
	long int ans = 0;
	int length = strlen(a);
	for(int i=0; i<length; i++)
		ans = (ans << 3) + (ans << 1) + a[i]-'0';
	return ans;
}

void readfromindexfile(char *filename)
{
	FILE *fp;
	char *line = NULL;
	char *line1 = NULL;
	size_t length = 0;
	ssize_t read;

	fp = fopen(filename,"r");

	while((read = getline(&line, &length, fp)) != -1)
	{
		line1 = strndup(line,strlen(line)-1);
		char *token;
		char *values[6];
		int i = 0;
		token = strtok(line1,"\t");
		while(token !=NULL)
		{
			values[i++] = token;
			token = strtok(NULL,"\t");
			if(i == 4)
				break;
		}

		fileLocTime[values[0]] = stringtoint(values[3]);
	}
	fclose(fp);
}

int hextodec(char digit)
{
	return digit - (digit & 64 ? 55 : 48) & 15;
}

void updateFileIndex(char *filename)
{
	FILE *fp;
	char *line = NULL;
	char *line1 = NULL;
	size_t length = 0;
	ssize_t read;

	fp = fopen(filename,"r");

	while((read = getline(&line, &length, fp)) != -1)
	{
		line1 = strndup(line,strlen(line)-1);
		char *token;
		char *values[6];
		int i = 0;
		token = strtok(line1,"\t");
		while(token !=NULL)
		{
			values[i++] = token;
			token = strtok(NULL,"\t");
		}

		files tempFile;
		int length;

		//FilePath
		length = strlen(values[0]) + 5;
		tempFile.filePath = (char *) malloc (length);
		strcpy(tempFile.filePath,values[0]);

		//FileName
		length = strlen(values[1]) + 5;
		tempFile.fileName = (char *) malloc (length);
		strcpy(tempFile.fileName,values[1]);

		//FileSize
		tempFile.fileSize = int(stringtoint(values[2]));

		//FileTime
		tempFile.fileTime = stringtoint(values[3]);

		//FileType
		length = strlen(values[4]) + 5;
		tempFile.fileType = (char *) malloc (length);
		strcpy(tempFile.fileType,values[4]);

		//FileHash
		for(int i=0; i<MD5_DIGEST_LENGTH; i++)
			tempFile.fileHash[i] = (hextodec(values[5][2*i]) << 4 ) + hextodec(values[5][2*i+1]);

		fileindex.push_back(tempFile);
	}
	fclose(fp);
}
void readfromsharedfile(char *filename)
{
	FILE *fp;
	char *line = NULL;
	char *line1 = NULL;
	size_t length = 0;
	ssize_t read;

	fp = fopen(filename,"r");

	while((read = getline(&line, &length, fp)) != -1)
	{
		line1 = strndup(line,strlen(line)-1);
		sharedfiles(line1,0);
	}
	fclose(fp);
}

void deleteChangedIndex(const char *filename)
{
	FILE *fp;
	FILE *temp;
	char *line = NULL;
	char *line1 = NULL;
	size_t length = 0;
	ssize_t read;

	fp = fopen(filename,"r");
	temp = fopen("temp.conf","w");

	while((read = getline(&line, &length, fp)) != -1)
	{
		line1 = strndup(line,strlen(line)-1);
		char *token;
		char *values[6];
		int i = 0;
		token = strtok(line1,"\t");
		while(token !=NULL)
		{
			values[i++] = token;
			token = strtok(NULL,"\t");
			if(i == 4)
				break;
		}

		if(deleteLocTime.find(values[0]) == deleteLocTime.end() && checkFileExist.find(values[0])->second == 1)
			fprintf(temp,"%s",line);
	}

	fclose(fp);
	fclose(temp);

	remove(filename);
	rename("temp.conf",filename);

}

void writetofile(const char *filename)
{

	deleteChangedIndex(filename);

	FILE *fp;
	fp = fopen(filename,"a");
	for(int i=0; i<fileindex.size(); i++)
	{
		fprintf(fp,"%s\t%s\t%d\t%ld\t%s\t",fileindex[i].filePath,fileindex[i].fileName,fileindex[i].fileSize,fileindex[i].fileTime,fileindex[i].fileType);
		printhash(fp,fileindex[i].fileHash);
	}

	fclose(fp);
}

