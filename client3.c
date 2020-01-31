#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "pthread.h"
#include "unistd.h"
#include "sys/socket.h"
#include "sys/un.h"
#include "stddef.h"
#include "fcntl.h"
#include "sys/select.h"
#include "aio.h"
#include "errno.h"
#include "signal.h"

#define BUFFER_SIZE 128
#define MAX_CONNECT_NUM 5

#define SOCK_S "sock_server"
#define SOCK_C "sock_client"

#define err_log(errlog) do{perror(errlog);exit(EXIT_FAILURE);}while(0)

int main(int argc,char *argv[]) //Client Asynchronous Blocking
{
        int fd,ret,recv_bytes,send_bytes;
        struct sockaddr_un ser_addr;
        char recv_buff[BUFFER_SIZE],send_buff[BUFFER_SIZE];
       
        fd_set readset,writeset;
        
        memset(&ser_addr,0,sizeof(ser_addr));
        ser_addr.sun_family = AF_UNIX;
        strcpy(ser_addr.sun_path,SOCK_S);

        fd = socket(AF_UNIX,SOCK_STREAM,0);
        if (fd < 0)
                err_log("Client3 socket() failed");
        ret = connect(fd,(struct sockaddr *)&ser_addr,sizeof(ser_addr));
        if (ret < 0)
                err_log("Client3 connect() failed");
        memset(send_buff,0,BUFFER_SIZE);
        strcpy(send_buff,"I am asynchronous block");
        printf("Client send: %s\n",send_buff);
        send_bytes = send(fd,send_buff,BUFFER_SIZE,0);
        if (send_bytes < 0)
                err_log("Client3 send() failed");
        while (1){
                FD_ZERO(&readset);   //clear read set
                FD_SET(fd,&readset); //add descriptor
                ret = select((fd+1),&readset,NULL,NULL,NULL);//block
                if (ret < 0)
                        err_log("Client3 select() failed");
                else if(ret == 0)
                        break;
                else if(FD_ISSET(fd,&readset)){ 
                        memset(recv_buff,0,sizeof(recv_buff));
                        recv_bytes = recv(fd,recv_buff,BUFFER_SIZE,MSG_DONTWAIT);
                        recv_buff[recv_bytes] = '\0';
                        printf("Client recv: %s\n",recv_buff);

                        break;
                }
        }
        close(fd);
        return 0;
}
