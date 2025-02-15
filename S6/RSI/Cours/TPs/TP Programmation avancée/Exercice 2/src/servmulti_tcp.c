/*********************************************************************
 *                                                                   *
 * FICHIER: SERVER_TCP                                               *
 *                                                                   *
 * DESCRIPTION: Utilisation de TCP socket par une application serveur*
 *              application client                                   *
 *                                                                   *
 * principaux appels (du point de vue serveur) pour un protocole     *
 * oriente connexion:                                                *
 *     socket()                                                      *
 *                                                                   * 
 *     bind()                                                        *
 *                                                                   * 
 *     listen()                                                      *
 *                                                                   *
 *     accept()                                                      *
 *                                                                   *
 *     read()                                                        *
 *                                                                   *
 *     write()                                                       *
 *                                                                   *
 *********************************************************************/
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define BUFSIZE 1500

int str_echo (int sockfd)
{
  int nrcv, nsnd;
  char msg[BUFSIZE];

  /*    * Attendre  le message envoye par le client 
   */
  memset( (char*) msg, 0, sizeof(msg) );
  if ( (nrcv= read ( sockfd, msg, sizeof(msg)-1) ) < 0 )  {
    perror ("servmulti : : readn error on socket");
    exit (1);
  }
  msg[nrcv]='\0';
  printf ("servmulti :message recu=%s du processus %d nrcv = %d \n",msg,getpid(), nrcv);

  if ( (nsnd = write (sockfd, msg, nrcv) ) <0 ) {
    printf ("servmulti : writen error on socket");
    exit (1);
  }
  printf ("nsnd = %d \n", nsnd);
  return (nsnd);
} /* end of function */


usage(){
  printf("usage : servmulti numero_port_serveur\n");
}


int main (int argc,char *argv[])

{
  int sockfd, n, newsockfd, childpid, servlen,fin;
  struct sockaddr_in  serv_addr, cli_addr;
  socklen_t clilen;

 /* Verifier le nombre de param�tre en entr�e */
  if (argc != 2){
    usage();
    exit(1);}


/*
 * Ouvrir une socket (a TCP socket)
 */
if ((sockfd = socket(PF_INET, SOCK_STREAM, 0)) <0) {
   perror("servmulti : Probleme socket\n");
   exit (2);
 }

 
/*
 * Lier l'adresse  locale � la socket
 */
 memset( (char*) &serv_addr,0, sizeof(serv_addr) );
 serv_addr.sin_family = PF_INET;
 serv_addr.sin_addr.s_addr = htonl (INADDR_ANY);
 serv_addr.sin_port = htons(atoi(argv[1]));
 
 
 if (bind(sockfd,(struct sockaddr *)&serv_addr, sizeof(serv_addr) ) <0) {
   perror ("servmulti : erreur bind\n");
   exit (1);
 }

/* Param�trer le nombre de connexion "pending" */
 if (listen(sockfd, SOMAXCONN) <0) {
   perror ("servmulti : erreur listen\n");
   exit (1);
 }

 for (;;) {
     int tabclients[FD_SETSIZE]; // Les sockets de dialogue
     struct sockaddr cli_addr;
     int i= 0; // nb client
     fd_set rset;
     FD_ZERO(&rset);
     FD_SET(sockfd, &rset);
     int maxfdp = sockfd + 1;

     for (;;) {
         printf("Je dors bien là, sur le i : %i \n", i);
         int nbfd = select(maxfdp, &rset, NULL, NULL, NULL);
         if (FD_ISSET(sockfd, &rset)) {
             tabclients[i] = accept(sockfd, &cli_addr, &clilen);
             FD_SET(tabclients[i], &rset);
             if (maxfdp <= tabclients[i]){
                 maxfdp = tabclients[i] + 1;
             }
             i++;
         }
         for (int j = 0; j < i; j++) {
             if (FD_ISSET(tabclients[j], &rset)) {
                 printf("Je suis là\n");
                 str_echo(tabclients[j]);
             }
         }
     }
     close(newsockfd);
 }
}



















