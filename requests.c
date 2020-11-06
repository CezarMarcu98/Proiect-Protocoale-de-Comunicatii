#include <stdlib.h>     /* exit, atoi, malloc, free */
#include <stdio.h>
#include <unistd.h>     /* read, write, close */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <arpa/inet.h>
#include "helpers.h"
#include "requests.h"

char *compute_get_request(const char *host,
	                  const char *url,
	                  const char *url_params,
	                  const char *cookies,
	                  const char *authorization)
{
    char *message = calloc(4096, sizeof(char));
    char *line = calloc(1000, sizeof(char));

    if (url_params != NULL) {
        sprintf(line, "GET %s?%s HTTP/1.1", url, url_params);
    } else {
        sprintf(line, "GET %s HTTP/1.1", url);
    }
    compute_message(message, line);

    sprintf(line, "Host: %s", host);
    compute_message(message, line);

    if (authorization != NULL) {
        sprintf(line, "Authorization: Bearer %s", authorization);
        compute_message(message, line);
    }

    if (cookies != NULL) {
        sprintf(line, "Cookie: %s", cookies);
        compute_message(message, line);
    }

    compute_message(message, "");

    free(line);
    return message;
}
char *compute_delete_request(const char *host,
	                   const char *url,
	                   const char *cookies,
	                   const char *authorization)
{
    char *message = calloc(4096, sizeof(char));
    char *line = calloc(1000, sizeof(char));

    sprintf(line, "DELETE %s HTTP/1.1", url);
    compute_message(message, line);

    sprintf(line, "Host: %s", host);
    compute_message(message, line);

    if (authorization != NULL) {
        sprintf(line, "Authorization: Bearer %s", authorization);
        compute_message(message, line);
    }

    if (cookies != NULL) {
        sprintf(line, "Cookie: %s", cookies);
        compute_message(message, line);
    }

    compute_message(message, "");
    compute_message(message, "\n");

    free(line);
    return message;
}

char *compute_post_request(const char *host,
	                   const char *url,
	                   const char *method,
	                   const char *form_data,
	                   const char *cookies,
	                   const char *authorization)
{

    char *message = calloc(4096, sizeof(char));
    char *line = calloc(1000, sizeof(char));

    sprintf(line, "POST %s HTTP/1.1", url);
    compute_message(message, line);

    sprintf(line, "Host: %s", host);
    compute_message(message, line);

    sprintf(line, "Content-Type: %s", method);
    compute_message(message, line);

    sprintf(line, "Content-Length: %ld", strlen(form_data));
    compute_message(message, line);
    
    if (authorization != NULL) {
        sprintf(line, "Authorization: Bearer %s", authorization);
        compute_message(message, line);
    }

    if (cookies != NULL) {
        sprintf(line, "Cookie: %s", cookies);
        compute_message(message, line);
    }

    compute_message(message, "");

    sprintf(line, "%s", form_data);
    compute_message(message, line);

    free(line);
    return message;
}