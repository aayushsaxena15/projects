char *getsystemtime()
{
    struct tm st;
    static char ostime[1000];

    time_t current;
    time(&current);
    st=*localtime(&current);
    strftime(ostime,sizeof(ostime),"%a %Y-%m-%d %H:%M:%S %Z",&st);
    return ostime;
}
char *epochToTimeStamp(long int etime)
{
    static char presenttime[1000];
    strcpy(presenttime,ctime(&etime));
    return presenttime;
}
long int get_file_time(char *fileName)
{
    struct stat filestamp;
    bzero(&filestamp,sizeof(filestamp));
    stat(fileName,&filestamp);
    return (long) filestamp.st_mtime;
}
