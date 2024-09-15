#include <stdlib.h>     /* exit, atoi, malloc, free */
#include <stdio.h>
#include <unistd.h>     /* read, write, close */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <arpa/inet.h>
#include "helpers.h"
#include "buffer.h"
#include "parson.h"

#define HEADER_TERMINATOR "\r\n\r\n"
#define HEADER_TERMINATOR_SIZE (sizeof(HEADER_TERMINATOR) - 1)
#define CONTENT_LENGTH "Content-Length: "
#define CONTENT_LENGTH_SIZE (sizeof(CONTENT_LENGTH) - 1)
#define CHARLEN 30
#define DELETE_ROUTE "/api/v1/tema/library/books/"
#define BOOK_ROUTE "/api/v1/tema/library/books/"

void error(const char *msg)
{
    perror(msg);
    exit(0);
}

void compute_message(char *message, const char *line)
{
    strcat(message, line);
    strcat(message, "\r\n");
}

int open_connection(char *host_ip, int portno, int ip_type, 
                    int socket_type, int flag)
{
    struct sockaddr_in serv_addr;
    int sockfd = socket(ip_type, socket_type, flag);
    if (sockfd < 0)
        error("ERROR opening socket");

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = ip_type;
    serv_addr.sin_port = htons(portno);
    inet_aton(host_ip, &serv_addr.sin_addr);

    if (connect(sockfd, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) < 0)
        error("ERROR connecting");

    return sockfd;
}

void close_connection(int sockfd)
{
    close(sockfd);
}

void send_to_server(int sockfd, char *message)
{
    int bytes, sent = 0;
    int total = strlen(message);

    do
    {
        bytes = write(sockfd, message + sent, total - sent);
        if (bytes < 0) {
            error("ERROR writing message to socket");
        }

        if (bytes == 0) {
            break;
        }

        sent += bytes;
    } while (sent < total);
}

char *receive_from_server(int sockfd)
{
    char response[BUFLEN];
    buffer buffer = buffer_init();
    int header_end = 0;
    int content_length = 0;

    do {
        int bytes = read(sockfd, response, BUFLEN);

        if (bytes < 0){
            error("ERROR reading response from socket");
        }

        if (bytes == 0) {
            break;
        }

        buffer_add(&buffer, response, (size_t) bytes);
        
        header_end = buffer_find(&buffer, HEADER_TERMINATOR,
                                    HEADER_TERMINATOR_SIZE);

        if (header_end >= 0) {
            header_end += HEADER_TERMINATOR_SIZE;
            
            int content_length_start = buffer_find_insensitive(&buffer,
                                        CONTENT_LENGTH, CONTENT_LENGTH_SIZE);
            
            if (content_length_start < 0) {
                continue;           
            }

            content_length_start += CONTENT_LENGTH_SIZE;
            content_length = strtol(buffer.data + content_length_start,
                                    NULL, 10);
            break;
        }
    } while (1);
    size_t total = content_length + (size_t) header_end;
    
    while (buffer.size < total) {
        int bytes = read(sockfd, response, BUFLEN);

        if (bytes < 0) {
            error("ERROR reading response from socket");
        }

        if (bytes == 0) {
            break;
        }

        buffer_add(&buffer, response, (size_t) bytes);
    }
    buffer_add(&buffer, "", 1);
    return buffer.data;
}

char *basic_extract_json_response(char *str)
{
    return strstr(str, "{\"");
}

char *json_login_info(char *username, char *password) {
	JSON_Value *root_value = json_value_init_object();
	JSON_Object *root_object = json_value_get_object(root_value);
	json_object_set_string(root_object, "username", username);
	json_object_set_string(root_object, "password", password);
	char *serialzized_message = json_serialize_to_string(root_value);
	json_value_free(root_value);
	return serialzized_message;
}

void usr_and_pass_read(char *username, char *password)
{
	printf("username=");
    fgets(username, CHARLEN, stdin);
    username[strlen(username) - 1] = 0;
    
	printf("password=");
    fgets(password, CHARLEN, stdin);
    password[strlen(password) - 1] = 0;
}

char *get_the_token(char *response) {
    JSON_Value *root_value;
    root_value = json_parse_string(basic_extract_json_response(response));
    JSON_Object *root_object = json_value_get_object(root_value);
    const char *token = json_object_get_string(root_object, "token");
    free(root_value);
    return (char*)token;
}

char *make_authorization(char *token) {
    char *str = calloc(LINELEN, sizeof(char));
    memset(str, 0, LINELEN * sizeof(char));
    strcat(str, "Bearer ");
    strncat(str, token, strlen(token));
	return str;
}

char *get_books(char *ptr) {
    return strstr(ptr, "[");
}

void show_books(char* books) {
    JSON_Value *json_value;
    JSON_Array *json_array;
    JSON_Object *json_object;

    json_value = json_parse_string(books);
    json_array = json_value_get_array(json_value);

    if (json_array_get_count(json_array) == 0) {
        printf("Libraria nu contine nicio carte.\n");
        json_value_free(json_value);
        return;
    }
    for (int i = 0; i < json_array_get_count(json_array); i++) {
        json_object = json_array_get_object(json_array, i);
        printf("ID = %d and Title = %s\n",
                (int)(json_object_get_number(json_object, "id")),
                json_object_get_string(json_object, "title"));
    }

    json_value_free(json_value);
}

void book_info_read(char *title, char *author, char *genre, char *publisher,
                    char *page_count)
{
    printf("title=");
    fgets(title, CHARLEN, stdin);
    title[strlen(title) - 1] = 0;

    printf("author=");
    fgets(author, CHARLEN, stdin);
    author[strlen(author) - 1] = 0;

    printf("genre=");
    fgets(genre, CHARLEN, stdin);
    genre[strlen(genre) - 1] = 0;

    printf("publisher=");
    fgets(publisher, CHARLEN, stdin);
    publisher[strlen(publisher) - 1] = 0;

    printf("page_count=");
    fgets(page_count, CHARLEN, stdin);
    page_count[strlen(page_count) - 1] = 0;
}

char *json_book_info(char *title, char *author, char *genre, char *publisher,
                    int page_count) {
	JSON_Value *root_value = json_value_init_object();
	JSON_Object *root_object = json_value_get_object(root_value);
	json_object_set_string(root_object, "title", title);
	json_object_set_string(root_object, "author", author);
    json_object_set_string(root_object, "genre", genre);
	json_object_set_string(root_object, "publisher", publisher);
    json_object_set_number(root_object, "page_count", page_count);
	char *serialzized_message = json_serialize_to_string(root_value);
	json_value_free(root_value);
	return serialzized_message;
}

void show_book(char* book) {
    JSON_Value *json_value;  
    JSON_Object *json_object;

    json_value = json_parse_string(book);
    json_object = json_value_get_object(json_value);

    printf("Titlu: %s\n", json_object_get_string(json_object, "title"));
    printf("Autor: %s\n", json_object_get_string(json_object, "author"));
    printf("Gen: %s\n", json_object_get_string(json_object, "genre"));
    printf("Editura: %s\n", json_object_get_string(json_object, "publisher"));
    printf("Nr pagini: %d\n", 
            (int) json_object_get_number(json_object, "page_count"));

    json_value_free(json_value);
}

char *make_path(int route, char id[]) {
    char *path = calloc(LINELEN, sizeof(char));
    if (route == 1) { 
        strcpy(path, BOOK_ROUTE);
        strcat(path, id);
    } else {
        strcpy(path, DELETE_ROUTE);
        strcat(path, id);
    }
    return path;
}
