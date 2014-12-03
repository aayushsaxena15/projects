#define sharedFolders "SharedFolder.conf"
#define history "History.conf"
#define indexFile "IndexFile.conf"
#define chunk 1000
#define MAX 1000

using namespace std;

unsigned long sizeoffile(char *fileName);



int uploadflag=1;

void printhash(unsigned char *s);
void printhash(FILE *fp,unsigned char *s);
unsigned char *getmd5hashing(char *fileName);

int hextodec(char digit);
char *getsystemtime();
long int get_file_time(char *fileName);
char *epochToTimeStamp(long int epochTime);

const char *getFileType(char *fileName);
void details_of_the_file(char *filePath,char *fileName);
void sharedfiles(const char *folderPath , int pathLevel);
void detailsofindexedfiles();

unsigned char tempfilehash[MD5_DIGEST_LENGTH];
void writetofile(const char *folderPath);
void deleteChangedIndex(const char *fileName);
void readfromsharedfile(char *fileName);
void readfromindexfile(char *fileName);
long int stringtoint(char *str);

void listentoclient();
void listenServerInput(char *message);

void receivefromserver(int sock);
void sendtoclient(int clientSock,char *fileName,char *filePath);
void downloadFileFromServer(int fileSize,char *fileName,int sock,char *hash);
void uploadtoserver(int clientSock,char *fileName);
void sendtoserver(int sock,char *fileName);
void downfromclient(char *fileName,int sock);

class files
{
	public:
		int fileSize;
		char *fileName;
		long int fileTime;
		char *fileType;
		unsigned char fileHash[MD5_DIGEST_LENGTH];
		char *filePath;
};

class ShareFolder
{
	public:
		char *folderPath;
};

class CommandHistory
{
	public:
		char *command;
		char *commandTime;
};

vector <files> fileindex;
vector <ShareFolder> shareFolder;
vector <CommandHistory> commandHistory;

map <string,long int> fileLocTime;
map <string,long int> deleteLocTime;
map <string,int> checkFileExist;

char confFolder[PATH_MAX];
char pathtoondexfile[PATH_MAX];
char pathtosharedFolder[PATH_MAX];
char tempFilePath[PATH_MAX];

