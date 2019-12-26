#include "common.h"
#include <strings.h>

int main(int argc, char **argv)
{
	int listenfd, connfd, n;
	struct sockaddr_in servaddr;
	uint8_t buff[MAXLINE+1];
	uint8_t recvline[MAXLINE+1];

	if((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		printf("socket error");

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(SERVER_PORT);

	if((bind(listenfd, (SA *) &servaddr, sizeof(servaddr))) < 0)
		printf("bind error");

	if((listen(listenfd, 10)) < 0)
		printf("listen error");

	for( ; ; ) {
		struct sockaddr_in addr;
		socklen_t addr_len;

		printf("Waiting for a connection on port %d\n", SERVER_PORT);
		fflush(stdout);
		connfd = accept(listenfd, (SA *) NULL, NULL);

		memset(recvline, 0, MAXLINE);

		while( (n = read(connfd, recvline, MAXLINE-1) ) > 0)
		{
			//fprintf(stdout, "\n%s\n\n%s",bin2hex(recvline,n),recvline);

			if(recvline[n-1] == '\n'){
				break;
			}

			memset(recvline, 0 , MAXLINE);
		}
		
		if(n < 0)
			printf("read error");

		snprintf((char*)buff, sizeof(buff), "HTTP/1.1 200 OK\r\n\r\nHello");
		write(connfd, (char*)buff, strlen((char*)buff));
		close(connfd);
	}

	return 0;
}
