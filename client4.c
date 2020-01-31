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

void aio_completion_handler(int signo,siginfo_t *info,void *context);

int main(int argc,char *argv[])//Client Asynchronous Non-blocking
{
        struct aiocb rd,wr;
        struct sigaction sig_act;
        int fd,ret,recv_bytes,send_bytes;
        char recv_buff[BUFFER_SIZE],send_buff[BUFFER_SIZE];
        struct sockaddr_un ser_addr;
        int i;

        memset(&ser_addr,0,sizeof(ser_addr));
        ser_addr.sun_family = AF_UNIX;
        strcpy(ser_addr.sun_path,SOCK_S);

        fd = socket(AF_UNIX,SOCK_STREAM,0);
        if (fd < 0)
                err_log("Client4 socket() failed");
        ret = connect(fd,(struct sockaddr *)&ser_addr,sizeof(ser_addr));
        if (ret < 0)
                err_log("Client4 connect() failed");
        memset(&wr,0,sizeof(wr));
        wr.aio_buf = send_buff;
        wr.aio_fildes = fd;
        wr.aio_nbytes = BUFFER_SIZE;
        
        memset(send_buff,0,BUFFER_SIZE);
        strcpy(send_buff,"I am asynchronous nonblock");
        printf("Client send: %s\n",(char *)(wr.aio_buf));
        ret = aio_write(&wr);
        if (ret < 0)
                err_log("Client4 aio_write failed");
        while (aio_error(&wr) == EINPROGRESS);
        ret = aio_return(&wr);

        sigemptyset(&sig_act.sa_mask);//Set up the signal handler
        sig_act.sa_flags = SA_SIGINFO;
        sig_act.sa_sigaction = aio_completion_handler;
        
        memset(&rd,0,sizeof(rd)); //Set up the AIO request
        rd.aio_buf = recv_buff;
        rd.aio_fildes = fd;
        rd.aio_nbytes = BUFFER_SIZE;
        rd.aio_offset = 0;
        //Link the AIO request with the signal handler
        rd.aio_sigevent.sigev_notify = SIGEV_SIGNAL;
        rd.aio_sigevent.sigev_signo = SIGIO;
        rd.aio_sigevent.sigev_value.sival_ptr = &rd;
        //Map the signal to the signal handler
        ret = sigaction(SIGIO,&sig_act,NULL);

        memset(recv_buff,0,sizeof(0));
        ret = aio_read(&rd);
        if (ret < 0)
                err_log("Client4 aio_read failed");
        //while (aio_error(&rd) == EINPROGRESS);
        //ret = aio_return(&rd);
        //printf("Client recv: %s\n",rd.aio_buf);
        for (i = 0; i < 5; ++i){
                printf("sleep...\n");
                sleep(1);
        }
        close(fd);
        return 0;
}
void aio_completion_handler(int signo,siginfo_t *info,void *context)
{
        struct aiocb *req;
        int ret;
        //Ensure it's our signal
        if(info->si_signo == SIGIO){
                req = (struct aiocb *)info->si_value.sival_ptr;
                if(aio_error(req) == 0){
                        ret = aio_return(req);
                        printf("Client recv: %s\n",(char *)(req->aio_buf));
                }
        }
        return;
}
