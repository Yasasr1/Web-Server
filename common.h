#ifndef _COMMON_H_
#define _COMMON_H_

#include <sys/socket.h>
#include <sys/types.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdarg.h>

#include <errno.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <netdb.h>

#define SERVER_PORT 18000
#define MAXLINE 4096
#define SA struct sockaddr

#define html "text/html"
#define mp3	"audio/mpeg"
#define mp4	"video/mp4"
#define jpg "image/jpeg"
#define jpeg "image/jpeg"
#define png "image/png"
#define txt "text/plain"


#endif
