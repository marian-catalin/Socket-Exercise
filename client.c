#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <pthread.h>
#include <string.h>
#define DIM 50

struct sockaddr_in addrport_srv;

struct Message{
  char string[DIM];
};
struct Message msgSend, msgRecv;

pthread_t thread;
void *Thread_Function(void *param);

int socket_id_client;

int main(){
  int i;

label:  printf("To send a request, type 0 and press Enter...\n");
  scanf("%i",&i);

  while(i == 0){

    /*create socket*/
    /*int socket_id = socket(family, type, protocol);*/
    socket_id_client = socket(PF_INET, SOCK_STREAM, 0);

    if(socket_id_client != -1){
      printf("Client socket created!\n");
      sleep(2);
    }
    else{
      printf("Client socket creation failed!\n");
    };

    int port;
    printf("Please type the server socket port and press Enter...\n");
    scanf("%i",&port);

    printf("Please type the name of the requested file and press Enter...\n");
    scanf("%s",&msgSend.string);

    /*assigning address to socket*/
    addrport_srv.sin_family = AF_INET;
    addrport_srv.sin_port = htons(port);
    addrport_srv.sin_addr.s_addr = htonl(INADDR_ANY);

    /*establish connection (client side)*/

    int connection_status_client = connect(socket_id_client, (struct sockaddr *)&addrport_srv, sizeof(addrport_srv)); //int status = connect(socketid_client, &addrport_srv, addrlen);

    if(connection_status_client != -1){
      printf("Connection request sent to server!\n");
      pthread_create(&thread, NULL, Thread_Function, NULL);
      pthread_join(thread, NULL);
    }
    else{
      printf("Connection request failed!\n");
      goto label;
    };

    printf("To send another request, type 0 and press Enter...\n");
    scanf("%i",&i);

  };

  printf("Client closed!\n");

  return 0;
};

void *Thread_Function(void *param){

    int send_client = send(socket_id_client, &msgSend.string, sizeof(msgSend.string), 0);

    if(send_client != -1){
      printf("Request sent!\n");
      printf("File requested: %s\n", msgSend.string);
    }
    else{
      printf("Sending request failed!\n");
    };

    int receive_client = recv(socket_id_client, &msgRecv.string, sizeof(msgRecv.string), 0);

    if(receive_client != -1 && strcmp(msgSend.string, msgRecv.string) == 0){
      printf("Requested file received from server!\n");
      printf("Received file: %s\n", msgRecv.string);
    }
    else{
      printf("Receiving requested file failed!\n");
    };

    /*close connection and free the port used by the socket*/
    int close_status_connection = close(socket_id_client);

    if(close_status_connection != -1){
      printf("Connection closed!\n");
    }
    else{
      printf("Failed to close connection\n");
      sleep(2);
    };
};
