#include "client.h"
#include "renderer.h"

int main(int argc, char** argv ) {
    int sockfd, newsockfd, clilen, chilpid, ok, nleft, nbwriten;
    char c;
    struct sockaddr_in cli_addr, serv_addr;

    if (argc != 3) {
        printf("usage  socket_client server port\n");
        exit(0);
    }

    printf ("client starting\n");  

    /* initialise la structure de donnee */
    bzero((char*) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;

    struct hostent *he = gethostbyname(argv[1]);
    if (he == NULL) {
        perror("gethostbyname");
        exit(1);
    }
    memcpy(&serv_addr.sin_addr, he->h_addr_list[0], he->h_length);
    serv_addr.sin_port = htons(atoi(argv[2]));

    printf("Connecting to %s:%s\n", inet_ntoa(serv_addr.sin_addr), argv[2]);


    sockfd=socket(AF_INET,SOCK_STREAM,0);
    if (sockfd == -1) {
        printf("socket error\n");
        exit(0);
    }

    /*DECOMMENTER APRES*/
    /* effectue la connection */
    //if (connect(sockfd,(struct sockaddr*)&serv_addr,sizeof(serv_addr))<0)
    //{
        //perror("connect");
        //printf("socket EEerror\n");
        //exit(EXIT_FAILURE);
    //}

    printf("Connection successful.\n");

    // --- NEW LOGIC: Start the state/render loop ---
    client_state_loop(sockfd);
    
    // Close socket on exit
    close(sockfd);
    printf("Client disconnected.\n");

    return 0;
}
