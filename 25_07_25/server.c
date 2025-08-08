#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
void client_info(struct sockaddr_in x)
{
        printf("IP address of client is : %s\n",inet_ntoa(x.sin_addr));
        printf("Port number of clint is : %hu\n",ntohs(x.sin_port));
}


int main(int argc , char **argv)
{
        int cfd,sfd;

        // master/listi9ng socket created for communication
        puts("creating socket...\n");

        sfd=socket(AF_INET , SOCK_STREAM , 0);
        if(sfd==-1)
        {
                perror("socket");
                return 0;
        }

        puts("master socket has been created...\n");

        puts("binding....");
        struct sockaddr_in saddr,caddr;

        saddr.sin_family=AF_INET;
        saddr.sin_port=htons(atoi(argv[1]));
        saddr.sin_addr.s_addr=inet_addr("0.0.0.0");

        if(bind(sfd,(const struct sockaddr *)&saddr,sizeof(saddr))==-1)
        {
                perror("bind");
                return 0;

        }

        puts("local address assigned to socket...\n");

        puts("listing....\n");

        if(listen(sfd,1)==-1)
  {
                perror("listen");
                return 0;

        }

        puts("wating for new connection....\n");

        int size=sizeof(caddr);

        cfd=accept(sfd,(struct sockaddr *)&caddr,&size);

        if(cfd==-1)
        {
                perror("accept");
                return 0;
        }
        puts("server has connected with client....\n");



        client_info(caddr);







        while(1);

}

