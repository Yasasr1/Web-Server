#include "common.h"
#include <strings.h>

char *mime_type;

void respond(int fd, char *header, void *body, int length);
void handleFile(int fd, char *fileName);

int main(int argc, char **argv)
{
	int listenfd, connfd, n;
	struct sockaddr_in servaddr;

	char buffer[1024] = {0}; 
    	char requestType[4];       
    	char requestpath[1024];

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

		if((connfd = accept(listenfd, (SA *)&servaddr, (socklen_t*)&servaddr)) == -1){
			printf("accept error");
		}

		read( connfd, buffer, 1024);



		int count = 0;
		int j = 0;
		bool isTypeSaved = false;

		for(int i=0; i<=(strlen(buffer)); i++){
			if(isTypeSaved == false && buffer[i] == ' '){
				requestType[j] = '\0';
				j = 0;
				isTypeSaved = true;
				continue;
			}
			else if(isTypeSaved == true && buffer[i] == ' '){
				requestpath[j] = '\0';
				j = 0;
				break;
			}
			if(isTypeSaved == false){
				requestType[j] = buffer[i];
				j++;
			}else if(isTypeSaved == true){
				requestpath[j] = buffer[i];
				j++;
			}
		}		
		
		printf("request Type: %s \n", buffer);
		printf("%s %s \n",requestType,requestpath);

		for(int i=0; i< 100; i++){
			requestpath[i] = tolower(requestpath[i]);
		}

		char *mime = strrchr(requestpath, '.')+1;
        	char *name = strtok(requestpath, "/");

		if(mime)
			mime_type = mime;
		else
			mime_type = NULL;

		if(!strcmp(requestType, "GET") && !strcmp(requestpath, "/")) {
			char *data = "Get request received: Displaying root page";
			mime_type = html;
			respond(connfd,"HTTP/1.1 200 OK", data, strlen(data));
		}else if(!strcmp(requestType, "POST") && !strcmp(requestpath, "/")){
			char *data = "Post request received";
			mime_type = html;
			respond(connfd,"HTTP/1.1 200 OK",data,strlen(data));
		}else{
			handleFile(connfd,name);
		}

		close(connfd);

	}

	return 0;
}

void respond(int fd, char *HTMLheader, void *HTMLbody, int length){
	char responce[length+100];
	int responce_length = sprintf(responce,"%s\n""Connection: close\n""Content-length: %d\n""Content-type: %s\n""\n",HTMLheader,length,mime_type);
	
	memcpy(responce + responce_length, HTMLbody,length);
	send(fd, responce,responce_length + length, 0);
}

void handleFile(int fd, char *name){
	char *source;
	FILE *file = fopen(name, "r");
	size_t bufferSize;

	if(file != NULL){
		if(fseek(file,0L,SEEK_END) == 0){
			bufferSize = ftell(file);
			source = malloc(sizeof(char) * (bufferSize + 1));
			fseek(file, 0L, SEEK_SET);
			fread(source, sizeof(char), bufferSize, file);
			respond(fd, "HTTP/1.1 OK", source, bufferSize);
		}
		free(source);
		fclose(file);
	}else{
		char *error = "404: file cannot be found";
        	mime_type = html;
        	respond(fd, "HTTP/1.1 404 NOT FOUND", error, strlen(error));
	}
}
