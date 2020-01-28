#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <pthread.h>
#include <string.h>
#include <dirent.h>
#define DIM 50

struct sockaddr_in addrport_srv;
struct sockaddr_in addrport_clt;

struct Message{
  char string[DIM];
};
struct Message msgSend, msgRecv;

pthread_t thread;
void *Thread_Function(void *param);

struct connection{
	int connection_socket;
  struct sockaddr client_address;
	int clt_addr_len;
};

struct connection *Con;

int i;
char req[DIM];

pthread_mutex_t lock;

int main(){

pthread_mutex_init(&lock, NULL);

label:i = 0;

  /*create socket*/
  /*int socket_id = socket(family, type, protocol);*/
  int socket_id_srv = socket(PF_INET, SOCK_STREAM, 0);

  if(socket_id_srv != -1){
    printf("Server socket created!\n");
    sleep(2);
  }
  else{
    printf("Server socket creation failed!\n");
  };

  int port;
  printf("Please type a port for server socket and press Enter...\n");
  scanf("%i",&port);

  /*assigning address to socket*/
  addrport_srv.sin_family = AF_INET;
  addrport_srv.sin_port = htons(port);
  addrport_srv.sin_addr.s_addr = htonl(INADDR_ANY);

  /*int status = bind(socket_id, &addrport, size);*/
  int bind_status_srv = bind(socket_id_srv, (struct sockaddr *)&addrport_srv, sizeof(addrport_srv));

  if(bind_status_srv != -1){
    printf("Server socket binded!\n");
    sleep(2);
  }
  else{
    printf("Server socket binding failed!\n");
    goto label;
  };

  /*listening for connections*/
  /*int status = listen(sockid, queueLimit); ... listen is used only by the server*/
  /*queueLimit = no. of active participants that can wait for a connection*/
  int listen_status_srv = listen(socket_id_srv, 2);

  if(listen_status_srv != -1){
    printf("Listening process initialized!\n");
    sleep(2);
  }
  else{
    printf("Listening process failed!\n");
    exit(0);
  };

  printf("To start listening, type 0 and press Enter...\n");
  scanf("%i",&i);

  while(i == 0){
    /*the server is accepting connection request from client*/
    /*accept() returns the clients socket descriptor*/
    /*create new connection dynamically*/
    /*int s = accept(sockid_srv, &clientAddr, &addrLen); -> clientAddr and addrLen will be filled in upon return*/

    Con = (struct connection *)malloc(sizeof(struct connection));
    Con->connection_socket = accept(socket_id_srv, &Con->client_address, &Con->clt_addr_len);

    if(Con->connection_socket != -1){
      printf("Connection accepted by server!\n");
      printf("Connected!\n");
      pthread_create(&thread, NULL, Thread_Function, (void *)Con);
      pthread_detach(thread);
    }
    else{
      printf("Connection failed!\n");
      free(Con);
      sleep(2);
    };
  };

  int close_status_server = close(socket_id_srv);

  if(close_status_server != -1){
    printf("Server closed!\n");
    sleep(2);
  }
  else{
    printf("Failed to close server!\n");
  };

  pthread_mutex_destroy(&lock);

  return 0;
};



void *Thread_Function(void *param){

  pthread_mutex_lock(&lock);

  /*Pointer for directory entry*/
  struct dirent *de;

  /*opendir() returns a pointer of DIR type*/
  DIR *dr = opendir(".");

  /*opendir returns NULL if couldn't open directory*/
  if (dr == NULL){
      printf("Could not open current directory\n");
      return 0;
  }

  struct connection *conn;
  conn = (struct connection *)param;

  /*create connection dynamically*/
  int receive_server = recv(conn->connection_socket, &msgRecv.string, sizeof(msgRecv.string), 0);

  if(receive_server != -1){
    printf("File request received from client!\n");
  }
  else{
    printf("Receiving request failed!\n");
  };

  while ((de = readdir(dr)) != NULL){
          if(strcmp(de->d_name, msgRecv.string) == 0){
            strcpy(req, msgRecv.string);
          };
  };

  if(strcmp(req, msgRecv.string) == 0){
    printf("Requested file: %s\n", req);
    strcpy(msgSend.string, msgRecv.string);

    int send_server = send(conn->connection_socket, &msgSend.string, sizeof(msgSend.string), 0);

    if(send_server != -1){
      printf("File sent to client!\n");
      printf("%s\n",msgSend.string);
    }
    else{
      printf("Sending file failed!\n");
    };
  }
  else{
    printf("File not found!\n");
  };

  /*close connection and free the port used by the socket*/
  int close_status_connection = close(conn->connection_socket);

  if(close_status_connection != -1){
    printf("Connection closed!\n");
    free(conn);
    printf("Still listening...\n");
    printf("CTRL+C to stop server...\n");
  }
  else{
    printf("Failed to close connection\n");
    sleep(2);
  };
  closedir(dr);
  pthread_mutex_unlock(&lock);
};
