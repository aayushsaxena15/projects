Computer Networks- Assignment 1

Pankaj Malhotra- 201201056
Aayush Saxena-   201201021


main code is in main.c
rest are the header files


for compiling just type on terminal:		"make"



Before executing, just mention the path of the folder which you want to share in the SharedFolder.conf file in the conf folder.
then for executing: ./proto <path to conf folder ending with "/"> <binding port> <listening port> <IP>



first argument- path to conf folder ending with a "/"
second argument- binding port
third argument- listening port 
foruth argument- IP of server

Make sure that all the header files are in the same folder.


conf folder:  contains   IndexFile.conf------- which will store all the indexed files
              and        SharedFolder.conf-----which has the absolute path to the shared folder.

Commands:


1. IndexGet ShortList
2. IndexGet LongList
3. IndexGet RegEx
4. FileHash Verify
5. FileHash CheckAll
6. FileDownload <name of file>
7. FileUpload <name of file>













