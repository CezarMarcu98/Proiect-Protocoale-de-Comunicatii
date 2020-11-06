#include <stdio.h>      /* printf, sprintf */
#include <stdlib.h>     /* exit, atoi, malloc, free */
#include <unistd.h>     /* read, write, close */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <arpa/inet.h>
#include "helpers.h"
#include "requests.h"
#include <stdbool.h>

#define IP "3.8.116.10"
#define PORT 8080

char *take_cookie(char *msg){
    char *poz = strstr(msg, "Cookie: ");
    if (poz == NULL)
        return NULL;
    char *cookie = calloc (1000, sizeof(char));
    poz = poz + strlen("Cookie: ");
    char *subpoz = strstr(poz, "\n");
    int len = strlen(poz) - strlen(subpoz);
    strncpy(cookie, poz, len);
    cookie[len - 1] = '\0';
    return cookie;
}

char* take_tok(char *msg){
    char *poz = strstr(msg, "token");
    if (poz == NULL)
        return NULL;

    poz = poz + strlen("token") + 3;
    char *subpoz = strstr(poz, "\"");

    char *token = calloc(1000, sizeof(char));
    int len = strlen(poz) - strlen(subpoz);
    strncpy(token, poz, len);
    return token;
}

void jsonResponse(char *msg, int flag){
    char *delim = "\n\r";
    char *tok2;
    char *tipMesaj;

    char *err;
    char *prt_err; 
    if ((err = strstr(msg, "error")) != NULL){
            err += strlen("error\":\"");
            int len1 = strlen(err);
            len1 -= 3;
            prt_err = calloc(len1, 1);
            strncpy(prt_err, err, len1);
            printf("%s\n", prt_err);
            return;
        }
    else if (strstr(msg, "token") || (strstr(msg, "Cookie"))){
        tok2 = strtok(msg, delim);
        if ((tipMesaj = strstr(tok2, "200")) != NULL){
        printf("Cerere acceptata -- OK\n\n");
        }
        if ((tipMesaj = strstr(tok2, "201")) != NULL){
            printf("Cererea a fost creata\n\n");
        }

    }else{

        char *dup = calloc(strlen(msg), 1);
        strcpy(dup, msg);
        tok2 = strtok(dup, delim);
        if (flag == 1){
            if ((tipMesaj = strstr(tok2, "200")) != NULL){
                printf("Cerere acceptata -- OK\n\n");
            }
            if ((tipMesaj = strstr(tok2, "429")) != NULL){
                printf("Too many requests -- retry later\n\n");
                return;
            }
        }
        if ((tipMesaj = strstr(tok2, "201")) != NULL){
            printf("Cererea a fost creata\n\n");
        }
        if (dup)
            free(dup);

    }
    
}
void printBooks(char *msg){
    char *dup2 = calloc(strlen(msg), 1);
    strcpy(dup2, msg);
    jsonResponse(dup2, 0);
    char *poz = strstr(msg, "[");
    if (poz != NULL)
        printf("%s\n\n", poz);
}


int main(int argc, char *argv[])
{
    char *message;
    char *response;
    int sockfd;
    char *ipAndPort = calloc(31, sizeof(char));
    char *buffer = calloc(100, sizeof(buffer));
    char *cookie_login;
    char *token_auth;

    sprintf(ipAndPort, "%s:%d", IP, PORT);

    while (true){
        scanf("%s", buffer);
        if (strcmp(buffer, "register") == 0){
            sockfd = open_connection(IP, PORT, AF_INET, SOCK_STREAM, 0);
            
            char *username = calloc(100, sizeof(char));
            char *password = calloc(100, sizeof(char));
            printf("username = ");
            scanf("%s", username);
            printf("password = ");
            scanf("%s", password);

            char *data = calloc(200, sizeof(char));
            sprintf(data, "{\n\"username\":\"%s\",\n\"password\":\"%s\"\n}", username, password);

            free(username);
            free(password);
            
            message = compute_post_request(ipAndPort, "/api/v1/tema/auth/register", "application/json" , data, NULL, NULL);
            send_to_server(sockfd, message);
            response = receive_from_server(sockfd);
            jsonResponse(response, 0);
            close_connection(sockfd);
        }
        if (strcmp(buffer, "login") == 0){
            sockfd = open_connection(IP, PORT, AF_INET, SOCK_STREAM, 0);

            char *username = calloc(100, sizeof(char));
            char *password = calloc(100, sizeof(char));
            printf("username = ");
            scanf("%s", username);
            printf("password = ");
            scanf("%s", password);

            char *data = calloc(200, sizeof(char));
            sprintf(data, "{\n\"username\":\"%s\",\n\"password\":\"%s\"\n}", username, password);

            free(username);
            free(password);
            

            message = compute_post_request(ipAndPort, "/api/v1/tema/auth/login", "application/json" , data, NULL, NULL);
            send_to_server(sockfd, message);
            response = receive_from_server(sockfd);
            char *dup = calloc(strlen(response), 1);
            strcpy(dup, response);
            jsonResponse(dup, 0);
            if (dup)
                free(dup);
            cookie_login = take_cookie(response);
            
            close_connection(sockfd);
            

        }
        if (strcmp(buffer, "enter_library") == 0){
            sockfd = open_connection(IP, PORT, AF_INET, SOCK_STREAM, 0);

            message = compute_get_request(ipAndPort, "/api/v1/tema/library/access", NULL, cookie_login, NULL);
            send_to_server(sockfd, message);
            response = receive_from_server(sockfd);
            char *dup = calloc(strlen(response), 1);
            strcpy(dup, response);
            jsonResponse(dup, 0);
            if (dup)
                free(dup);
            token_auth = take_tok(response);
            close_connection(sockfd);
        }
        if (strcmp(buffer, "get_books") == 0){
            sockfd = open_connection(IP, PORT, AF_INET, SOCK_STREAM, 0);

            message = compute_get_request(ipAndPort, "/api/v1/tema/library/books", NULL, cookie_login, token_auth);
            send_to_server(sockfd, message);
            response = receive_from_server(sockfd);
            printBooks(response);
            close_connection(sockfd);
        }
        if (strcmp(buffer, "get_book") == 0){
            sockfd = open_connection(IP, PORT, AF_INET, SOCK_STREAM, 0);

            int id;
            printf("id = ");
            scanf("%d", &id);
            char *url = calloc (100, sizeof(char));
            sprintf(url, "%s/%d", "/api/v1/tema/library/books", id);
            url[strlen(url)] = '\0';

            message = compute_get_request(ipAndPort, url, NULL, cookie_login, token_auth);
            send_to_server(sockfd, message);
            response = receive_from_server(sockfd);
            printBooks(response);
            if (url)
                free(url);
            close_connection(sockfd);
        }
        if (strcmp(buffer, "delete_book") == 0){
            sockfd = open_connection(IP, PORT, AF_INET, SOCK_STREAM, 0);

            int id;
            printf("id = ");
            scanf("%d", &id);
            char *url = calloc (100, sizeof(char));
            sprintf(url, "%s/%d", "/api/v1/tema/library/books", id);
            url[strlen(url)] = '\0';

            message = compute_delete_request(ipAndPort, url, cookie_login, token_auth);

            send_to_server(sockfd, message);
            response = receive_from_server(sockfd);
            jsonResponse(response, 1);
            free(url);
            close_connection(sockfd);
        }
        if (strcmp(buffer, "add_book") == 0){
            sockfd = open_connection(IP, PORT, AF_INET, SOCK_STREAM, 0);

            char *data = calloc(1000, 1);
            char *title = calloc(100, 1);
            char *author = calloc(100, 1);
            char *genre = calloc(100, 1);
            char *publisher = calloc(100, 1);
            int page;
            
            printf("title = ");
            scanf("%s", title);
            printf("author = ");
            scanf("%s", author);
            printf("genre = ");
            scanf("%s", genre);
            printf("page_count = ");
            scanf("%d", &page);
            printf("publisher = ");
            scanf("%s", publisher);

            char *pagini = calloc(3, 1);
            sprintf(pagini, "%d", page);

            strcat(data, "{\n");
            strcat(data, "\"title\":\"");
            strcat(data, title);
            strcat(data, "\",\n\"author\":\"");
            strcat(data, author);
            strcat(data, "\",\n\"genre\":\"");
            strcat(data, genre);
            strcat(data, "\",\n\"page_count\":\"");
            strcat(data, pagini);
            strcat(data, "\",\n\"publisher\":\"");
            strcat(data, publisher);
            strcat(data, "\"\n}");
            
            message = compute_post_request(ipAndPort, "/api/v1/tema/library/books", "application/json" , data, NULL, token_auth);
            send_to_server(sockfd, message);
            response = receive_from_server(sockfd);
            jsonResponse(response, 1);

            free(title);
            free(author);
            free(genre);
            free(publisher);
            free(pagini);
            close_connection(sockfd);
        }
        if (strcmp(buffer, "logout") == 0){
            sockfd = open_connection(IP, PORT, AF_INET, SOCK_STREAM, 0);

            message = compute_get_request(ipAndPort, "/api/v1/tema/auth/logout", NULL, cookie_login, NULL);
            cookie_login = NULL;
            token_auth = NULL;
            
            send_to_server(sockfd, message);
            response = receive_from_server(sockfd);

            jsonResponse(response, 1);
            close_connection(sockfd);
        }
        if (strcmp(buffer, "exit") == 0){
            break;
        }
    }
    
	shutdown(sockfd, 2);
	close_connection(sockfd);
    free(ipAndPort);
    free(buffer);

    return 0;
}
