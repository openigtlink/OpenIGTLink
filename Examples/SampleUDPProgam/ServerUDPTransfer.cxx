#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

#define BUFLEN 512
#define NPACK 10
#define PORT 9930

#include <string.h>

#ifndef BUFLEN
    #define BUFLEN 512
#endif
#ifndef NPACK
    #define NPACK 10000
#endif
#ifndef PORT
    #define PORT 9930
#endif
void diep(char *s)
{
  perror(s);
  _exit(1);
}

#define SRV_IP "127.0.0.1"
/* diep(), #includes and #defines like in the server */

int main(void)
{
 struct sockaddr_in si_other;
 int s, i, slen=sizeof(si_other);
 char buf[BUFLEN];

 if ((s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP))==-1)
   diep("socket");

 memset((char *) &si_other, 0, sizeof(si_other));
 si_other.sin_family = AF_INET;
 si_other.sin_port = htons(PORT);
 if (inet_aton(SRV_IP, &si_other.sin_addr)==0) {
   fprintf(stderr, "inet_aton() failed\n");
   _exit(1);
 }

 for (i=0; i<NPACK; i++) {
   printf("Sending packet %d\n", i);
   sprintf(buf, "This is packet %d\n", i);
   if (sendto(s, buf, BUFLEN, 0, (struct sockaddr *)&si_other, slen)==-1)
     diep("sendto()");
 }

 close(s);
 return 0;
}
