#ifndef _HELPERS_
#define _HELPERS_

#define BUFLEN 4096
#define LINELEN 1000

// shows the current error
void error(const char *msg);

// adds a line to a string message
void compute_message(char *message, const char *line);

// opens a connection with server host_ip on port portno, returns a socket
int open_connection(char *host_ip, int portno, int ip_type, int socket_type, int flag);

// closes a server connection on socket sockfd
void close_connection(int sockfd);

// send a message to a server
void send_to_server(int sockfd, char *message);

// receives and returns the message from a server
char *receive_from_server(int sockfd);

// extrage si returneaza JSON din raspunsul serverului
char *basic_extract_json_response(char *str);

// Formatare json pentru login credentials
char *json_login_info(char *username, char *password);

// Citire credentials
void usr_and_pass_read(char *username, char *password);

// Extragere token din formatul json
char *get_the_token(char *response);

// Formatare pentru autorizatie de vazut cartile
char *make_authorization(char *token);

// Ia lisat de carti
char *get_books(char *str);

// Arata lista de carti
void show_books(char* book_list);

// Citesc datele despre o carte
void book_info_read(char *title, char *author, char *genre, char *publisher,
                    char *page_count);

// Fac datele in format json
char *json_book_info(char *title, char *author, char *genre, char *publisher,
                    int page_count);

// Arat datele despre o carte
void show_book(char* book);

// Fac cale catre o carte
char *make_path(int route, char id[]);

#endif
