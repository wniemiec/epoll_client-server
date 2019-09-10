#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#define PORT 4444

int main(int argc, char *argv[]){

    int clientSocket, ret;
    //struct sockaddr_in serverAddr;
    struct addrinfo* serverAddr;
    char buffer[1024];

    clientSocket = socket(AF_INET, SOCK_STREAM, 0);  //Tutaj tworzymy gniazdo
    if(clientSocket < 0){ //jesli gniazdo jest < 0 to znaczy, ze sie nie utworyzlo i zwraca error
        printf("[-]Error in connection.\n");
        exit(1);
    }
    printf("[+]Client Socket is created.\n"); // tutaj informacja, ze gniazdo zostalo stworzone

    getaddrinfo("127.0.0.1", argv[1], NULL, &serverAddr);

    //memset(&serverAddr, '\0', sizeof(serverAddr)); // ustweienie pamieci na strukture w ktorej robimy adresowanie
    //serverAddr.sin_family = AF_INET; //tworzymy rodzine ip
    //serverAddr.sin_port = htons(PORT); // przypisujemy port
    //serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1"); // przypisujemy adres ip

    ret = connect(clientSocket, serverAddr->ai_addr, serverAddr->ai_addrlen); //tworzymy komunikacje
    if(ret < 0){
        printf("[-]Error in connection.\n");
        exit(1);
    }
    printf("[+]Connected to Server.\n");

    while(1){ //tutaj wpisujemy polecenia, tworzymy nieskoczona petle
        printf("Client: \t");
        scanf("%s", &buffer[0]);
        send(clientSocket, buffer, strlen(buffer), 0);  //wysylmy wiadomosc do innego socketa
      }

    return 0;
}
