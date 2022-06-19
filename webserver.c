/**
 * webserver.c
 * 
 * Description: Creates a simple server that binds to a socket. The server can
 *              handle simple GET requests, and respond with 400, 404 or 200.
 * 
 * Synopsis:    $ ./webserver [directory] [port]
 *              directory   - The directory where files are gotten from.
 *              port        - The port the server will run on.
 * 
 * Author:      Buster Hultgren Wärn - busterw@cs.umu.se
 * Date:        2019-09-10
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <poll.h>
#include <inttypes.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "webserver.h"
#include "str.h"

struct requestBody {
    char *body;
};


struct response {
    long length;
    char *status;
    char *type;
    char *body;
};

/*
* description:  Main funcrtion.
*/
int main(int argc, const char** argv) {

    if (argc != 3) {
        fprintf(stderr, "Incorrect arguments\n");
        return 1;
    }

    uint16_t port = strtol(argv[1], NULL, 10);
    if(!port) {
        fprintf(stderr, "Failed to parse port argument\n");
        return 2;
    }
    int server = create_socket(port);

    listen(server, MAX_CONNECTIONS);
    runServer(server, argv[2]);
    close(server);
}

/*
* description:  Runs the server; handles client connection to server; handles 
*               clients requests
* param[in]: server - The server filed descriptor.
* param[in]: filePath - filePath to directory where files are stored.
*/
void runServer(int server, const char *filePath) {

    struct pollfd fds[MAX_CONNECTIONS]; 
    int clientCount = 2;

    fds[0].fd = server;
    fds[0].events = POLLIN;
    fds[1].fd = STDIN_FILENO;
    fds[1].events = POLLIN;

    int running = 1;
    while(running) {
        int ret = poll(fds, clientCount, 0);

        if (ret < 1) {
           continue; 
        }
        for(int i = 0; i < clientCount; i++) {

            if(fds[i].revents & POLLIN) {
                if(i == 0) {
                    if (clientCount >= MAX_CONNECTIONS - 1) {
                        rejectClient(server);
                    } else {
                        fds[clientCount].fd = accept(server, NULL, NULL);
                        fds[clientCount].events = POLLIN;
                        clientCount++;
                        printf("Client connected!\n");
                    }
                } else {
                    char buffer[256] = {0};

                    if (read(fds[i].fd, buffer, 255) == 0) {
                        close(fds[i].fd);
                        clientCount--;
                        fds[i] = fds[clientCount];
                        i--;
                        continue;
                    } else if (fds[i].fd == STDIN_FILENO) {
                        if(strncmp(buffer, "quit", 4) == 0) {
                            fprintf(stdout, "Quitting program\n");
                            running = 0;
                            break;
                        } else {
                            fprintf(stdout, "Internal transmission: %s\r\n"
                                , buffer);
                        }
                    } else {
                        struct response *r = parseHTTPRequest(buffer, filePath);
                        sendResponseToSocket(fds[i].fd, r);
                        responseKill(r);
                        clientCount--;
                        close(fds[i].fd);
                        fds[i] = fds[clientCount];
                        i--;
                    }
                }
            }
        }
    }
}

/*
* description:  Creates an empty response struct.
* returns: The respose struct.
*/
struct response *responseEmpty(void) {

    struct response *r = malloc(sizeof(*r));
    r->length = 0;
    r->status = NULL;
    r->type = NULL;
    r->body = NULL;
    return r;
}

/*
* description:  Frees all memory allocated in a response struct.
* param[in]: r - The response struct.
*/
void responseKill(struct response* r) {

    if (r->status != NULL) free(r->status);
    if (r->type != NULL) free(r->type);
    if (r->body != NULL) free(r->body);
    free(r);
}

/*
* description:  Parses a string containg an HTTP GET request. Handles the
                response with a response struct.
* param[in]: request - The request.
* param[in]: filePath - Path to the file to GET.
* returns: The response struct.
*/
struct response *parseHTTPRequest(char *request, const char *filePath) {

    struct response *r = responseEmpty();
    int reqLen = strlen(request);

    if (reqLen >= 12 && strStartsWith(request, "GET")) {
        char *fileName = strGetWordFromString(request + 3);
        int fileLen = strlen(filePath) + strlen(fileName);
        char file[fileLen + 1];
        file[0] = '\0';
        strcat(file, filePath);
        strcat(file, fileName);
        file[fileLen] = '\0';
        free(fileName);

        if (access(file, F_OK ) != -1) {
            r->status = strCreate("200 OK");
            readFileToResponse(r, file);
        } else {      
            r->status = strCreate("404 Not Found\r\n");
        }
    } else {
        r->status = strCreate("400 Bad Request\r\n");
    }
    return r;
}

/*
* description:  From a file that can be opened, reads that file and parses
                the information to a response struct.
* param[in]: r - The response struct.
* param[in]: filename - The path to the file.
*/
void readFileToResponse(struct response *r, char *filename) {

    char *body = 0;
    long length = 0;
    FILE *file = fopen(filename, "rb");

    if (file) {
        fseek(file, 0, SEEK_END);
        length = ftell(file);
        fseek(file, 0, SEEK_SET);
        body = malloc(length);
        if (body)
        {
            fread(body, 1, length, file);
        }
        fclose(file);
    }
    r->length = length;
    r->body = body;

    if (strEndsWith(filename, ".html")) {
        r->type = strCreate("text/html");
    } else if (strEndsWith(filename, ".png")) {
        r->type = strCreate("image/png");
    } else {
        r->type = strCreate("text/plain");
    }
}

/*
* description:  Sends a response to a socket from a respone struct.
* param[in]: sockfd - The socket file descriptor.
* param[in]: r - The response struct.
*/
void sendResponseToSocket(int sockfd, struct response *r) {

    if (r == NULL || r->status == NULL) return;
    if (r -> type != NULL && r -> body != NULL && r->length > 0) {;
        dprintf(sockfd
            ,"HTTP/1.0 %s \r\nContent-Length: %ld\r\nContent-Type: %s\r\n\r\n"
            , r->status, r->length, r->type);
        send(sockfd, r->body, r->length, 0);
    } else {
        dprintf(sockfd, "HTTP/1.0 %s\r\n" ,r->status);
    }
}

/*
* description:  Rejects a client
* param[in]: serverSocket - file descriptor to server socket.
*/
void rejectClient(int serverSocket) {

    struct pollfd fd;
    fd.fd = accept(serverSocket, NULL, NULL);
    char msg[] = "Server full; connection refused";
    send(fd.fd, msg, strlen(msg), 0);
    close(fd.fd);
    printf("Client rejected\r\n");
}

/*
* description:  Get number of digits that an integer contains. 
* param[in]: number - The integer (as a long).
* returns: The number of digits of that integer.
*/
int getNumberDigits(long number) {

    if (number < 0) number = number * -1;
    int numDigits = 0;
    long checker = 1;
    do {
        checker = checker * 10;
        number -= checker;
        if (number >= 0) numDigits++;
        number += checker;
    } while(number >= 0 && checker > 0);
    return numDigits;
}

/*
* description:  Creates a TCP socket
* param[in]: local - Port socket is bound to
* returns: The socket
*/
int create_socket(int local) {

    struct sockaddr_in in_addr;
    int insocket = socket(AF_INET, SOCK_STREAM, 0);

    if(insocket < 0) {
        perror("failed to create incoming socket");
        return -3;
    } else {
        in_addr.sin_family = AF_INET;
        in_addr.sin_port = htons(local);
        in_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    }

    if(bind(insocket, (struct sockaddr*)&in_addr, sizeof(in_addr))) {
        perror("Failed to bind socket");
        return -4;
    }

    return insocket;
}
