#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <netdb.h>
#define PORT 4444
#define MAX_EVENTS 10
#define BUFFOR_SIZE 255

int main(int argc, char * argv[]){

    int sockfd, ret;
    //struct sockaddr_in serverAddr; //struktura do tworzenia adresowania
    struct addrinfo* serverAddr;

    int newSocket; //nowe gniazdo
    struct sockaddr_in newAddr;  //struktura na nowe adresowanie

    socklen_t addr_size;

    char buffer[1024];
    pid_t childpid; // proces potomny

    sockfd = socket(AF_INET, SOCK_STREAM, 0); // tu tworzymy socket
    if(sockfd < 0){
        printf("[-]Error in connection.\n");
        exit(1);
    }
    printf("[+]Server Socket is created.\n");

    getaddrinfo("127.0.0.1", argv[1], NULL, &serverAddr);
    //memset(&serverAddr, '\0', sizeof(serverAddr));  // ustawiamy pamiec na adresowanie
    //serverAddr.sin_family = AF_INET; // rodzina adresow ip
    //serverAddr.sin_port = htons(PORT); //ustawiamy port
    //serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1"); // ustawiamy ip

    ret = bind(sockfd, serverAddr->ai_addr, serverAddr->ai_addrlen); //bindujemy serwer, wiazemy otwarty socket z adresem, zeby socket wiedzial, ze ten adres nalezy do tego socketa
    if(ret < 0){
        printf("[-]Error in binding.\n");
        exit(1);
    }
    printf("[+]Bind to port %s\n", argv[1]);

    if(listen(sockfd, 10) == 0){ // nasluchiwanie i tworzenie kolejki
        printf("[+]Listening....\n");
    }else{
        printf("[-]Error in binding.\n");
    }


           struct epoll_event ev, events[MAX_EVENTS];
           int  conn_sock, nfds, epollfd;

           /* Code to set up listening socket, 'listen_sock',
              (socket(), bind(), listen()) omitted */

           epollfd = epoll_create1(0);
           if (epollfd == -1) {
               perror("epoll_create1");
               exit(EXIT_FAILURE);
           }

           ev.events = EPOLLIN; //epoll kolejkuje wydarzenia i obsluguje po kolei
           ev.data.fd = sockfd; //przypisanie socketu ktory naslichuje do epolla czyli do epolla trzeba dac tego socketa ktory jest w listeningu
           if (epoll_ctl(epollfd, EPOLL_CTL_ADD, sockfd, &ev) == -1) {
               perror("epoll_ctl: sockfd");
               exit(EXIT_FAILURE);
           }

		char buff[BUFFOR_SIZE];

           for (;;) {
               nfds = epoll_wait(epollfd, events, MAX_EVENTS, -1); //zwraca ilosc deksrypotorow na ktorych cos jest
               if (nfds == -1) {
                   perror("epoll_wait");
                   exit(EXIT_FAILURE);
               }

               for (int n = 0; n < nfds; ++n) { //petla iteruje po tych deskryptoach
                   if (events[n].data.fd == sockfd)
		    { // data.fd to deskrtypor danego zdarzenia, jesli cos sie stalo na moim sokecie ktory naslichuje to wykonuje ifa
                       conn_sock = accept(sockfd, (struct sockaddr*)serverAddr->ai_addr, &serverAddr->ai_addrlen); //przyjecie polaczenia
                       if (conn_sock == -1) { //otwarcie nowego polaczneia, sprawdzneie czy accept sie powiodl
                           perror("accept");
                           exit(EXIT_FAILURE);
                       }
                      // setnonblocking(conn_sock);
                       ev.events = EPOLLIN | EPOLLET; //przypisanie do listy epolla nowego socketu/polaczenia
                       ev.data.fd = conn_sock;
                       if (epoll_ctl(epollfd, EPOLL_CTL_ADD, conn_sock, &ev) == -1)
		       {
                           perror("epoll_ctl: conn_sock");
                           exit(EXIT_FAILURE);
                       }

            	  }
			else{
                        recv(events[n].data.fd, buff, BUFFOR_SIZE, 0); //obsluga waidomosci wysylanych pomiedzy clientem a serverem
                        printf("Client: %s\n", buff);
                        //send(events[n].data.fd, buff, strlen(buff), 0);
                        bzero(buff, sizeof(buff)); // zerowanie bufforu
                        }




		}
	}
    close(newSocket);
    return 0;
}
