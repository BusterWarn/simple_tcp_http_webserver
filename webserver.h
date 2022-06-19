/**
 * webserver.h
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

#ifndef SERVER
#define SERVER

typedef struct response response;
typedef struct requestBody requestBody;

const int MAX_CONNECTIONS = 10;

/*
* description:  Runs the server; handles client connection to server; handles 
*               clients requests
* param[in]: server - The server filed descriptor.
* param[in]: filePath - filePath to directory where files are stored.
*/
void runServer(int server, const char *filePath);

/*
* description: Creates an empty response struct.
* returns: The respose struct.
*/
struct response *responseEmpty(void);

/*
* description: Frees all memory allocated in a response struct.
* param[in]: r - The response struct.
*/
void responseKill(struct response* r);

/*
* description:  Parses a string containg an HTTP GET request. Handles the
                response with a response struct.
* param[in]: request - The request.
* param[in]: filePath - Path to the file to GET.
* returns: The response struct.
*/
struct response *parseHTTPRequest(char *request, const char *filePath);

/*
* description: From a file that can be opened, reads that file and parses
               the information to a response struct.
* param[in]: r - The response struct.
* param[in]: filename - The path to the file.
*/
void readFileToResponse(struct response *r, char *filename);


/*
* description: Sends a response, read from a response struct, to a socket.
* param[in]: sockfd - The socket file descriptor.
* param[in]: r - The response struct.
*/
void sendResponseToSocket(int sockfd, struct response *r);

/*
* description: Rejects a client
* param[in]: serverSocket - file descriptor to server socket.
*/
void rejectClient(int serverSocket);

/*
* description: Get number of digits that an integer contains. 
* param[in]: number - The integer (as a long).
* returns: The number of digits of that integer.
*/
int getNumberDigits(long number);

/*
* description: Creates a TCP socket
* param[in]: local - Port socket is bound to
* returns: The socket
*/
int create_socket(int local);

#endif // SERVER
