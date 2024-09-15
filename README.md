*******************************************************************************
                    Gheorghisor Ileana-Teodora
                            333 CA
                        Tema HTTP - PCom
*******************************************************************************
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Introducere ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    Aceasta este o tema ce simuleaza o aplicatie web pentru intelegerea
protocolului HTTP. Proiectul reprezinta clientul care poate sa isi creeze cont,
sa se autentifice si sa interactioneze cu o biblioteca.
    Lista de interacituni:
* register -> creare de cont
* login -> autentificare
* enter_library -> accesare biblioteca
* get_books -> afisarea listei de carti
* add_book -> adaugarea unei carti in biblioteca
* delete_book -> stergerea unei carti din biblioteca
* get_book -> afisarea unei carti
* logout -> iesirea din cont
* exit -> incheierea programului

~~~~~~~~~~~~~~~~~~~~~~~~~~ Explicarea Codului ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    Am plecat de la scheletul laboratorului 9. Nu voi explica ceea ce a fost
deja implementat acolo. De asemenea, am cautat cum sa integrez libraria parson
si cum sa lucrez cu JSON-uri.
    Am scris comentariile si printurile in limba romana, insa nu am sters nici
modificat comentariile din scheletul initial. Sper sa nu reprezinte acest fapt
o problema. Avand comentarii in cod, voi explica mai sumar aici implementarea.

-> Register
    Aici, am citit datele de la tastatura, am compus un mesaj cu ele si le-am
    trimis serverului sub forma de un post request. Daca exista deja cont,
    nu se mai poate crea altul la fel.

-> Login
    Trimit datele cu un post request si obtin in schimb un cookie pe care il
    folosesc sa ma asigur ca sunt autentificata pentru urmatoarele comenzi.
    De asemenea am o variabila locala care verifica acest lucru pentru usurinta
    utilizarii.
    Daca datele introduse sunt gresite, nu se poate face autentificarea.

-> Logout
    Numai daca sunt autentificata, ma pot deloga. Reprezinta un 

-> Enter_library
    Aici, verific folosind cookieul daca sunt logata si in momentul in care
    accesez biblioteca, primesc un token pe care il voi foloi ulterior
    sa demonstrez ca am autorizatia de a interactiona cu biblioteca.
-> Get_books si Get_book
    Afisez lista de carti, respectiv detaliile unei carti cerute. Verific ca
    id-ul dat ca parametru sa fie valid.
-> Delete
    Sterg cartea. Verific ca id-ul sa fie valid. Pentru delete am facut un
    request now specific.
-> Exit
    Ies din program.

~~~~~~~~~~~~~~~~~~~~~~~ Concluzii si precizari ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    Mi-a placut mult sa lucrez la aceasta tema, desi am intampinat multe
dificultati pe parcurs. De la erori de neinteles, ce s-au dovedit a fi doar
uitari de \ sau alte caractere, pana la a intelege unde trebuia sa fac
modificari in scheletul de la laborator pentru a integra alte functionalitati.
(de exemplu, la headerul de autorizatie).
    Am incercat sa tratez toate erorile la care m-am putut gandi pe moment, dar
si ceea ce a fost indicat in enunt. Sper mult sa nu se puna problema de a fi
considerate extra sau de a fi depunctata pe felul in care am tratat erorile.
Am incercat, pana la urma, sa ii dau un aer personal. La fel si cu printurile.

    Am incercat sa fac tema cat de bine am putut. :D


*******************************************************************************
