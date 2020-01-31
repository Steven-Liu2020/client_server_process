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

int main(int agrc,char *argv[])
{
        int ser_fd,cli_fd,ret;
        int recv_bytes,send_bytes;
        struct sockaddr_un un;
        char recv_buff[BUFFER_SIZE],send_buff[BUFFER_SIZE];
        int reuse_val = 1;
        memset(&un, 0, sizeof(un));
        un.sun_family = AF_UNIX;
        unlink(SOCK_S);
        strcpy(un.sun_path,SOCK_S);

        ser_fd = socket(AF_UNIX,SOCK_STREAM,0);
        if (ser_fd < 0)
                err_log("Server socket() failed");
        //Set addr reuse
        setsockopt(ser_fd,SOL_SOCKET,SO_REUSEADDR,(void *)&reuse_val,sizeof(reuse_val));
        ret = bind(ser_fd,(struct sockaddr *)&un,sizeof(un));
        if (ret < 0)
                err_log("Server bind() failed");
        ret = listen(ser_fd,MAX_CONNECT_NUM);
        if (ret < 0)
                err_log("Srevet listen() failed");
        while (1){
                struct sockaddr_un client_addr;
                cli_fd = accept(ser_fd,NULL,NULL);
                if (cli_fd < 0)
                        continue;
                memset(recv_buff,0,BUFFER_SIZE);
                recv_bytes = recv(cli_fd,recv_buff,BUFFER_SIZE,0);
                if (recv_bytes <= 0)
                        err_log("Server recv() failed");
                recv_buff[recv_bytes] = '\0';
                printf("Server recv: %s\n",recv_buff);
                sleep(3);
                *send_buff = '\0';
                strcat(send_buff,"welcome");
                printf("Server send: %s\n",send_buff);
                send_bytes = send(cli_fd,send_buff,strlen(send_buff),0);
                if (send_bytes < 0)
                        err_log("Server send() failed");
                close(cli_fd);
        }
        close(ser_fd);
        return 0;
}
