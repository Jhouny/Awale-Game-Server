#include "client.h"

int main(int argc, char** argv ) {
  int sockfd, newsockfd, clilen, chilpid, ok, nleft, nbwriten;
  char c;
  struct sockaddr_in cli_addr, serv_addr;

  if (argc != 3) {
    printf("usage  socket_client server port\n");
    exit(0);
  }

  /*
   *  partie client 
   */
  printf ("client starting\n");  

  /* initialise la structure de donnee */
  bzero((char*) &serv_addr, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;

  struct hostent *he = gethostbyname(argv[1]);
  if (he == NULL) {
      perror("gethostbyname");
      exit(1);
  }
  memcpy(&serv_addr.sin_addr, he->h_addr, he->h_length);
  serv_addr.sin_port = htons(atoi(argv[2]));

  printf("Connecting to %s:%s\n", inet_ntoa(serv_addr.sin_addr), argv[2]);

  /* ouvre le socket */
  if ((sockfd=socket(AF_INET,SOCK_STREAM,0))<0) {
    printf("socket error\n");
    exit(0);
  }

  /* effectue la connection */
  if (connect(sockfd,(struct sockaddr*)&serv_addr,sizeof(serv_addr))<0)
    {
      perror("connect");
      printf("socket EEerror\n");
      exit(EXIT_FAILURE);
    }
    
  
  char header[512];
  snprintf(header, sizeof(header), "GET /index.html HTTP/1.1\r\nHost: %s\r\nConnection: close\r\n\r\n", argv[1]);
  write (sockfd, header, sizeof(header) - 1); // -1 to exclude null terminator
  printf("Sent HTTP GET request\n");

  int byte_count = 0;
  int total_bytes = 0;
  char buffer[256];
  bzero(buffer, 256);
  // Read the response from the server
  while ((byte_count = read(sockfd, buffer, 255)) > 0) {
      total_bytes += byte_count;
      buffer[byte_count] = '\0'; // Null-terminate the string
      printf("%s", buffer);
      bzero(buffer, 256);
  }
  printf("\nTotal bytes received: %d\n", total_bytes);

  return 1;
}
