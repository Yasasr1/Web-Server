#include "common.h"
#include <strings.h>

char *outputType;

void handleFile(int fd, char *fileName);

int main(int argc, char **argv)
{
	int listenfd, connfd, n;
	struct sockaddr_in servaddr;

	char browserInput[1024] = {0}; 
    	char type[4];       
    	char path[1024];

	if((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
		printf("socket error");
		return -1;
		}

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(SERVER_PORT);

	if((bind(listenfd, (SA *) &servaddr, sizeof(servaddr))) < 0){
		printf("bind error");
		return -1;
	}

	if((listen(listenfd, 10)) < 0){
		printf("listen error");
		return -1;
		}

	for( ; ; ) {
		struct sockaddr_in addr;
		socklen_t addr_len;

		printf("Waiting for a connection on port %d\n", SERVER_PORT);
		fflush(stdout);

		if((connfd = accept(listenfd, (SA *)&servaddr, (socklen_t*)&servaddr)) == -1){
			printf("accept error");
			return -1;
		}

		read( connfd, browserInput, 1024);



		int count = 0;
		int j = 0;
		bool isTypeSaved = false;

		for(int i=0; i<=(strlen(browserInput)); i++){
			if(isTypeSaved == false && browserInput[i] == ' '){
				type[j] = '\0';
				j = 0;
				isTypeSaved = true;
				continue;
			}
			else if(isTypeSaved == true && browserInput[i] == ' '){
				path[j] = '\0';
				j = 0;
				break;
			}
			if(isTypeSaved == false){
				type[j] = browserInput[i];
				j++;
			}else if(isTypeSaved == true){
				path[j] = browserInput[i];
				j++;
			}
		}		
		
		printf("request Type: %s \n", browserInput);
		printf("%s %s \n",type,path);


		char extentionType[1024];
		bool isSeperatorFound = false;
		int k = 0;
		for(int i=0; i<strlen(path); i++){
			
			if(path[i] == '.'){
				isSeperatorFound = true;
				continue;
			}
			
			if(isSeperatorFound == true){
				extentionType[k] = path[i];
				k++;
			}
			
			if(path[i] == '\0'){
				extentionType[k] = '\0';
				isSeperatorFound = false;
				k = 0;
	
			}
		}

		if(strlen(path) <=2){
			extentionType[0] = '\0';
			
		}

		char *name = strtok(path, "/");

		printf("File type is:%s ",extentionType);


		if(strlen(extentionType) > 1)
			outputType = extentionType;
		else
			outputType = NULL;

		if(!strcmp(type, "GET") && !strcmp(path, "/")) {
			char *data = "Get request received: Displaying root page";
			outputType = html;
			char responce[strlen(data)+100];
			int responce_length = sprintf(responce,"HTTP/1.1 200 OK\n""Connection: close\n""Content-length: %ld\n""Content-type: %s\n""\n",strlen(data),outputType);
	
			memcpy(responce + responce_length, data,strlen(data));
			send(connfd, responce,responce_length + strlen(data), 0);
		}else{
			handleFile(connfd,name);
		}

		close(connfd);

	}

	return 0;
}


void handleFile(int fd, char *name){
	char *source;
	FILE *file = fopen(name, "r");
	size_t browserInputSize;

	if(file != NULL){
		if(fseek(file,0L,SEEK_END) == 0){
			browserInputSize = ftell(file);
			source = malloc(sizeof(char) * (browserInputSize + 1));
			fseek(file, 0L, SEEK_SET);
			fread(source, sizeof(char), browserInputSize, file);

			char responce[browserInputSize+100];
			int responce_length = sprintf(responce,"HTTP/1.1 OK\n""Connection: close\n""Content-length: %ld\n""Content-type: %s\n""\n",browserInputSize,outputType);
	
			memcpy(responce + responce_length, source,browserInputSize);
			send(fd, responce,responce_length + browserInputSize, 0);
		}
		free(source);
		fclose(file);
	}else{
		char *error = "404: file cannot be found";
        	outputType = html;

		char responce[strlen(error)+100];
		int responce_length = sprintf(responce,"HTTP/1.1 404 NOT FOUND\n""Connection: close\n""Content-length: %ld\n""Content-type: %s\n""\n",strlen(error),outputType);
	
		memcpy(responce + responce_length, error,strlen(error));
		send(fd, responce,responce_length + strlen(error), 0);
	}
}


