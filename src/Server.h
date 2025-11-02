/* Serveur sockets TCP
 * affichage de ce qui arrive sur la socket
 *    socket_server port (port > 1024 sauf root)
 */

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include <signal.h>
#include <string.h>

int main(int argc, char **argv)
{
  char datas[] = "hello\n";
  int    sockfd,newsockfd,chilpid,ok,nleft,nbwriten;
  char c;
  struct sockaddr_in cli_addr,serv_addr;
  unsigned int clilen;

  if (argc!=2) {printf ("usage: socket_server port\n");exit(0);}
 
  printf ("server starting...\n");  

  /* ouverture du socket */
  sockfd = socket (AF_INET,SOCK_STREAM,0);
  if (sockfd<0) {printf ("impossible d'ouvrir le socket\n");exit(0);}

  /* initialisation des parametres */
  bzero((char*) &serv_addr, sizeof(serv_addr));
  serv_addr.sin_family       = AF_INET;
  serv_addr.sin_addr.s_addr  = htonl(INADDR_ANY);
  serv_addr.sin_port         = htons(atoi(argv[1]));

  /* effecture le bind */
  if (bind(sockfd,(struct sockaddr*)&serv_addr,sizeof(serv_addr))<0)
  {
    printf("impossible de faire le bind\n");
    exit(1);
  }

  /* petit initialisation */
  listen(sockfd, 1);

  while (1)
  {
    // Make the parent process allow multiple connections
    // by forking a child process to handle each connection
    // and closing the child socket in the parent process
    // and closing the parent socket in the child process
    /* attend la connection d'un client */
    clilen = sizeof(cli_addr);
    newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);
    if (newsockfd < 0)
    {
      printf("accept error\n");
      exit(0);
    }
    printf("connection accepted\n");

    if (fork() == 0)
    { // Child process
      close(sockfd);

      char full_buffer[256];
      bzero(full_buffer, 256);
      int total_bytes = 0;

      // Handle client connection
      while (1)
      {
        int n;
        char buffer[256];
        bzero(buffer, 256);
        n = read(newsockfd, buffer, 255);
        if (n <= 0)
        {
          close(newsockfd);
          exit(0);
        }
        // Read until newline or EOF
        if (total_bytes + n < sizeof(full_buffer))
        {
          memcpy(full_buffer + total_bytes, buffer, n);
          total_bytes += n;
          full_buffer[total_bytes] = '\0';
        }

        if (strchr(buffer, '\n') || strchr(buffer, '\r'))
        {
          printf("Received %d bytes: %s\n", n, full_buffer);
          n = write(newsockfd, full_buffer, total_bytes);
          if (n < 0)
            break;

          total_bytes = 0; // Reset for next message
          bzero(full_buffer, sizeof(full_buffer));
        }
      }
      close(newsockfd);
      exit(0);
    }
    else
    { // Parent process
      signal(SIGCHLD, SIG_IGN); // Prevent zombie processes
      close(newsockfd);
    }
  }

  /*  attention il s'agit d'une boucle infinie
   *  le socket n'est jamais ferme !
   */

  return 1;
}