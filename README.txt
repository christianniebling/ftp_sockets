How to use:
1) open a terminal in the TigerC directory and run the command "make all"
2) run command "./up_create" this should create all of the upload files on the client side
3) open a termianl in the TigerS directory and run the command "make all"
4) run command "./down_create", this should create all of the download files on the server
5) run command "./TigerS" on the server terminal. The server should now be waiting for a client connection
6) on the client terminal execute "./run.sh". this bash script should iterate 100 times with 100 different clients, each doing 1 tget and tput before exiting.

Note: This FTP server was tested using only 1 computer in the VSLI lab over loopback via address 127.0.0.1
This is an iterative server so the creation of the 100 clients on the shell script does not include the use of '&'. 
On the server side "secret.txt" stores user/password information. any user/password there can be used or even updated granted a recompile. 
After running the run.sh script the client program should close gracefull but the server should stay running to look for more clients.
down_create.c and up_create.c make 100 files with varying length in ASCII text using a rand() function. the size of the files can be adjusted here.

Note*: this FTP server will not replace files, this is intentional. 
in the event of:
-tget but the client has the file
-tget but the server cant find the file
-tput but the server has the file
The code will skip over these commands and proceede to the next ones.
