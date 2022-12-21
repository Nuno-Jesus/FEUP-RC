#ifndef DOWNLOAD_H
#define DOWNLOAD_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <regex.h>

#define BLACK "\033[30" 
#define RED "\033[31" 
#define GREEN "\033[32" 
#define YELLOW "\033[33" 
#define BLUE "\033[34" 
#define MAGENTA "\033[35" 
#define CYAN "\033[36" 
#define WHITE "\033[37" 
#define RESET "\033[0m"

#define BBLACK "\033[1m\033[30m" 
#define BRED "\033[1m\033[31m" 
#define BGREEN "\033[1m\033[32m" 
#define BYELLOW "\033[1m\033[33m" 
#define BBLUE "\033[1m\033[34m" 
#define BMAGENTA "\033[1m\033[35m" 
#define BCYAN "\033[1m\033[36m" 
#define BWHITE "\033[1m\033[37m" 

#define FTP_PORT 21
#define READ_MAX 1024

#define CODE_SUCCESS 200
#define CODE_SERVICE_READY 220
#define CODE_ENTER_PASSIVE 227
#define CODE_LOGIN_SUCCESSFUL 230

#define CODE_FAILURE 500
#define CODE_NEED_USER 503
#define CODE_NO_LOGIN 530
#define CODE_WRONG_FILE 550

#define DELIMITER '|'

#endif