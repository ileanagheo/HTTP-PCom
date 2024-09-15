// Gheorghisor Ileana-Teodora 333 CA
#include "helpers.h"
#include "requests.h"
#include <arpa/inet.h>
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <stdio.h>      /* printf, sprintf */
#include <stdlib.h>     /* exit, atoi, malloc, free */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <unistd.h>     /* read, write, close */
#include "parson.h"

#define HOST "34.254.242.81"
#define PORT 8080
#define CHARLEN 30
#define HOSTPORT "34.254.242.81:8080"
#define REGISTER_ROUTE "/api/v1/tema/auth/register"
#define LOGIN_ROUTE "/api/v1/tema/auth/login"
#define PAYLOAD "application/json"
#define LOGOUT_ROUTE "/api/v1/tema/auth/logout"
#define ACCESS_ROUTE "/api/v1/tema/library/access"
#define VISUALIZE_ROUTE "/api/v1/tema/library/books"
#define INSERT_ROUTE "/api/v1/tema/library/books"
#define DELETE_ROUTE "/api/v1/tema/library/books/"
#define BOOK_ROUTE "/api/v1/tema/library/books/"

int main(int argc, char *argv[])
{
	char *request, *response;
	int sockfd;
	char command[LINELEN];
	char *token  = NULL;
	char **cookies = NULL;
	int logged_in = 0; // Verific mereu daca sunt logata sau nu
	char *authorization = NULL;

	// Cat timp programul ruleaza
	while (1) {
		// Citesc o comanda
		fgets(command, 30, stdin);

		// Register
		if (strcmp(command, "register\n") == 0) {
			// Deschid conexiunea la server
			sockfd = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);
			if (sockfd == -1) {
				printf("Eroare la conexiunea cu serverul.\n");
				return -1;
			}

			// Obtin userul si parola de la tastatura
			char username[LINELEN], password[LINELEN];
			usr_and_pass_read(username, password);

			// Creez mesaj JSON cu datele
			char **message = calloc(1, sizeof(char*));
			message[0] = json_login_info(username, password);

			// Creez requestul catre server
			request = compute_post_request(HOSTPORT, REGISTER_ROUTE, PAYLOAD,
											message, 1, NULL, NULL, 0);
			if (request == NULL) {
                printf("Nu s-a putut crea mesajul.\n");
                return -1;
            }

			// Trimit mesajul la server
			send_to_server(sockfd, request);
			response = receive_from_server(sockfd);

			// Verific raspunsul de la server
			if (response) {
                if (strstr(response, "error")) {
                printf("Numele de utilizator %s deja exista!\n", username);
				} else if (strstr(response, "201 Created")) {
					printf("200 - OK - Utilizator înregistrat cu succes!\n");
				} else {
					printf("Nu ai putut crea cont!\n");
				}
            } else {
				printf("Eroare la raspunsul de la server.\n");
			}

			// Eliberez memoria si inchid conexiunea
			json_free_serialized_string(message[0]);
			free(message);
			free(request);
			free(response);
			close_connection(sockfd);
		}

		// Login
		else if (strcmp(command, "login\n") == 0) {
			// Verific daca nu sunt deja logata
			if (logged_in) {
				printf("Esti deja autentificat! Nu te poti deloga.\n");
			} else {
				// Deschid conexiunea la server
				sockfd = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);
				if (sockfd == -1) {
					printf("Eroare la conexiunea cu serverul.\n");
					return -1;
				}

				// Obtin userul si parola de la tastatura
				char username[LINELEN], password[LINELEN];
				usr_and_pass_read(username, password);

				// Creez mesaj JSON cu datele introduse
				char **message = calloc(1, sizeof(char*));
				message[0] = json_login_info(username, password);

				// Creez requestul catre server
				request = compute_post_request(HOSTPORT, LOGIN_ROUTE, PAYLOAD,
												message, 1, NULL, NULL, 0);
				if (request == NULL) {
					printf("Nu s-a putut crea mesajul.\n");
					return -1;
				}

				// Trimit mesajul la server
				send_to_server(sockfd, request);
				response = receive_from_server(sockfd);

				// Verific raspunsul
				if (response) {
					if (strstr(response, "200 OK")) {
						// Ma asigur ca sunt logata
						logged_in = 1;
						// Atunci, salvez cookie-ul
						cookies = calloc(1, sizeof(char*));
						cookies[0] = calloc(LINELEN, sizeof(char));

						char *ptr = strstr(response, "connect.sid=");
						ptr = strtok(ptr, ";");
						strcpy(cookies[0], ptr);

						printf("200 - OK - Autentificare reusita!\n");
					} else if (strstr(response, "No account with this username!")) {
						printf("Nu exista cont cu acest nume de utilizator\n");
					} else if (strstr(response, "Credentials are not good!")) {
						printf("Datele introduse nu sunt corecte!\n");
					}
				} else {
					printf("Eroare la raspunsul de la server.\n");
				}

				// Eliberez memoria si inchid conexiunea
				json_free_serialized_string(message[0]);
				free(message);
				free(request);
				free(response);
				close_connection(sockfd);
			}
		}

		// Exit
		else if (strcmp(command, "exit\n") == 0) {
            break;
        }

		// Logout
		else if (strcmp(command, "logout\n") == 0) {
			if (!logged_in) {
				printf("Nu esti inca autentificat! Nu te poti deloga.\n");
			} else {
				// Deschid conexiunea la server
				sockfd = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);
				if (sockfd == -1) {
					printf("Eroare la conexiunea cu serverul.\n");
					return -1;
				}
				// Verific sa fiu conectat la server
				if (cookies != NULL) {
					// Creez requestul catre server
					request = compute_get_request(HOST, LOGOUT_ROUTE, NULL,
												NULL, cookies, 1);
					if (request == NULL) {
						printf("Nu s-a putut crea mesajul.\n");
						return -1;
					}

					// Trimit mesajul la server
					send_to_server(sockfd, request);
					response = receive_from_server(sockfd);

					// Verific raspunsul
					if (response) {
						if (strstr(response, "200 OK")) {
							logged_in = 0;
							free(cookies[0]);
							free(cookies);
							if (token != NULL) {
								free(token);
							}
							if (authorization != NULL) {
								free(authorization);
							}
							printf("200 - OK - Te-ai delogat cu succes!\n");
						} else {
							printf("400 Bad Request\n");
						}
					} else {
						printf("Eroare la raspunsul de la server.\n");
					}

					// Eliberez memoria
					free(request);
					free(response);
					
				} else {
					printf("400 Bad Request.\n");
				}
				
				// Inchid conexiunea
				close_connection(sockfd);
			}
		}

		// enter_library
		else if (strcmp(command, "enter_library\n") == 0) {
			if (!logged_in) {
				printf("Nu esti inca autentificat!\n");
			} else {
				// Deschid conexiunea la server
				sockfd = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);
				if (sockfd == -1) {
					printf("Eroare la conexiunea cu serverul.\n");
					return -1;
				}
				// Verific sa fiu conectat la server
				if (cookies != NULL) {
					// Creez requestul catre server
					request = compute_get_request(HOST, ACCESS_ROUTE, NULL,
												NULL, cookies, 1);
					if (request == NULL) {
						printf("Nu s-a putut crea mesajul.\n");
						return -1;
					}

					// Trimit mesajul la server
					send_to_server(sockfd, request);
					response = receive_from_server(sockfd);

					// Verific raspunsul
					if (response) {
						// Extrag jetonul
						token = get_the_token(response);

						if (token != NULL) {
							// Creez headerul Authorization folosind jetonul
							authorization = make_authorization(token);
							printf("200 - OK - Ai intrat in biblioteca!\n");
						} else {
							printf("Nu s-a putut accesa libraria.\n");
						}
					} else {
						printf("Eroare la raspunsul de la server.\n");
					}

					// Eliberez memoria
					free(request);
					free(response);
				} else {
					printf("400 Bad Request\n");
				}
				
				// Inchid conexiunea
				close_connection(sockfd);
			}
		}

		// get_books
		else if (strcmp(command, "get_books\n") == 0) {
			if (!logged_in) {
				printf("Nu esti inca autentificat!\n");
			} else {
				// Deschid conexiunea la server
				sockfd = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);
				if (sockfd == -1) {
					printf("Eroare la conexiunea cu serverul.\n");
					return -1;
				}
				// Verific sa fiu conectat la biblioteca
				if (token != NULL) {
					// Creez requestul catre server
					request = compute_get_request(HOST, VISUALIZE_ROUTE, NULL,
													authorization, cookies, 1);
					if (request == NULL) {
						printf("Nu s-a putut crea mesajul.\n");
						return -1;
					}
					// Trimit mesajul la server
					send_to_server(sockfd, request);
					response = receive_from_server(sockfd);

					// Verific raspunsul
					if (response) {
						// Iau cartile din formatul json si le printez
						if (strstr(response, "200 OK")) {
							printf("Acestea sunt cartile:\n");
							char* book_list = get_books(response);
							show_books(book_list);
						} else {
							printf("400 Bad Request\n");
						}
					} else {
						printf("Eroare la raspunsul de la server.\n");
					}
					
					// Eliberez memoria
					free(request);
					free(response);

				} else {
					printf("Nu ai acces inca la biblioteca!\n");
				}

				// Inchid conexiunea
				close_connection(sockfd);
			}
		}
		// add_book
		if (strcmp(command, "add_book\n") == 0) {
			if (!logged_in) {
				printf("Nu esti inca autentificat!\n");
			} else {
				// Deschid conexiunea la server
				sockfd = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);
				if (sockfd == -1) {
					printf("Eroare la conexiunea cu serverul.\n");
					return -1;
				}
				// Verific sa fiu conectat la librarie
				if (token != NULL) {

					// Obtin datele cartii de la tastatura
					char title[CHARLEN], author[CHARLEN], genre[CHARLEN];
					char publisher[CHARLEN], pg_count[CHARLEN];
					int page_count;

					book_info_read(title, author, genre, publisher, pg_count);
					// Verific ca nr de pagini sa fie numar
    				if(!atoi(pg_count)) {
        				printf("Nu s-a introdus un numar pentru pagini!\n");
    				} else if (strlen(title) <= 0 || strlen(author) <=0
								|| strlen(genre) <= 0 || strlen(publisher) <= 0) {
						printf("Nu s-au introdus date cum trebuie!\n");
					} else {
						// Salvez nr de pagini ca numar
						page_count = atoi(pg_count);

						// Creez mesaj JSON cu datele
						char **message = calloc(1, sizeof(char*));
						message[0] = json_book_info(title, author, genre,
													publisher, page_count);

						// Creez requestul catre server
						request = compute_post_request(HOSTPORT, INSERT_ROUTE,
									PAYLOAD, message, 1, authorization, NULL, 0);
						if (request == NULL) {
							printf("Nu s-a putut crea mesajul.\n");
							return -1;
						}

						// Trimit mesajul la server
						send_to_server(sockfd, request);
						response = receive_from_server(sockfd);

						// Verific raspunsul de la server
						if (response) {
							if (strstr(response, "200 OK")) {
								printf("200 - OK - Cartea a fost adaugata cu succes!\n");
							} else if (strstr(response, "page_count")) {
								printf("Lipseste numarul de pagini!\n");
							} else if (strstr(response, "publisher")) {
								printf("Lipseste editura!\n");
							} else if (strstr(response, "genre")) {
								printf("Lipseste genul!\n");
							} else if (strstr(response, "author")) {
								printf("Lipseste autorul!\n");
							} else if (strstr(response, "title")) {
								printf("Lipseste titlul!\n");
							} else {
								printf("Nu s-a putut adauga cartea!\n");
							}
						} else {
							printf("Eroare la raspunsul de la server.\n");
						}
						// Eliberez memoria
						json_free_serialized_string(message[0]);
						free(message);
						free(request);
						free(response);
					}

				} else {
					printf("Nu ai acces inca la biblioteca!\n");
				}

				// Inchid conexiunea
				close_connection(sockfd);
			}
		}
		// delete_book
		else if (strcmp(command, "delete_book\n") == 0) {
			if (!logged_in) {
				printf("Nu esti inca autentificat!\n");
			} else {
				// Deschid conexiunea la server
				sockfd = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);
				if (sockfd == -1) {
					printf("Eroare la conexiunea cu serverul.\n");
					return -1;
				}
				// Verific sa fiu conectat la librarie
				if (token != NULL) {
					// Completez path-ul pentru cererea de delete
					char id[CHARLEN];
    				printf("id=");
    				fgets(id, CHARLEN, stdin);
    				id[strlen(id) - 1] = 0;
					if (!atoi(id)) {
						printf("Introduceti un numar valid!\n");
					} else  {
						char *path;
						int route = 0;
						path = make_path(route, id);

						// Creez requestul catre server
						request = compute_delete_request(HOST, path, authorization,
														cookies[0]);
						if (request == NULL) {
							printf("Nu s-a putut crea mesajul.\n");
							return -1;
						}

						// Trimit mesajul la server
						send_to_server(sockfd, request);
						response = receive_from_server(sockfd);

						// Verific raspunsul de la server
						if (response) {
							if (strstr(response, "404")) {
								printf("Nu exista aceasta carte.\n");
							} else if (strstr(response, "200 OK")) {
								printf("Cartea a fost stearsa cu succes!\n");
							} else {
								printf("Cartea nu a putut fi stearsa!\n");
							}
						} else {
							printf("Eroare la raspunsul de la server.\n");
						}

						// Eliberez memoria
						free(path);
						free(request);
						free(response);
					}

				} else {
					printf("Nu ai acces inca la biblioteca!\n");
				}

				// Inchid conexiunea
				close_connection(sockfd);
			}
		}
		// get_book
		else if (strcmp(command, "get_book\n") == 0) {
			if (!logged_in) {
				printf("Nu esti inca autentificat!\n");
			} else {
				// Deschid conexiunea la server
				sockfd = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);
				if (sockfd == -1) {
					printf("Eroare la conexiunea cu serverul.\n");
					return -1;
				}
				// Verific sa fiu conectat la librarie
				if (token != NULL) {
					// Completez path-ul pentru cererea de vizualizarea  cartii
					char id[CHARLEN];
    				printf("id=");
    				fgets(id, CHARLEN, stdin);
    				id[strlen(id) - 1] = 0;

					if(!atoi(id)) {
        				printf("Nu s-a introdus un numar valid!\n");
    				} else {
						// Creez calea
						char *path;
						int route = 1;
						path = make_path(route, id);
						// Creez requestul catre server
						request = compute_get_request(HOST, path, NULL, authorization,
													cookies, 1);
						if (request == NULL) {
							printf("Nu s-a putut crea mesajul.\n");
							return -1;
						}

						// Trimit mesajul la server
						send_to_server(sockfd, request);
						response = receive_from_server(sockfd);

						// Verific raspunsul de la server
						if (response) {
							if (strstr(response, "200 OK")) {
								show_book(basic_extract_json_response(response));
							} else if(strstr(response, "404 Not Found")) {
								printf("Nu exista cartea cu acest id.\n");
							}
						} else {
							printf("Eroare la raspunsul de la server.\n");
						}

						// Eliberez memoria
						free(path);
						free(request);
						free(response);
					}
				} else {
					printf("Nu ai acces inca la biblioteca!\n");
				}
				// Inchid conexiunea
				close_connection(sockfd);
			}
		}
	}
	if (token != NULL) {
		free(token);
	}
	if (authorization != NULL) {
		free(authorization);
	}
	if (cookies != NULL) {
		free(*cookies);
		free(cookies);
	}
	return 0;
}
