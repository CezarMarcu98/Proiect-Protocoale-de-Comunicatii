Marcu-Nicolescu Cezar-George
335CB

	
			Tema 3 - HTTP

Am inceput rezolvarea temei prin a deschide un loop coontinuu in care fac citiri de la tastatura in functie de comenzile clientului . Acest loop se inchide in momentul in care clientul tasteaza "exit" . Citesc fiecare comanda data de la tastatura pentru a putea crea cererile clientului . In momentul in care primeste o comanda cunoscuta , deschid conexiunea cu serverul si incep procesarea de date pentru a crea pachetul pe care i-l trimit. In momentul in care se doreste inregistrarea , clientul tasteaza "register", salvez username-ul si parola dorite de utilizator si formez un mesaj json pentru a-l trimite serverului . Respect formatul mesajelor json pentru a ma asigura ca interpretarea mesajului este inteleasa bine de server. Am hardcodat ip-ul serverului din host si l-am concatenat cu port-ul pentru a avea ruta catre server. Am testat pe postman si , dupa ceva chin cu intelegerea functionalitatilor aplicatiei , am vazut ca functioneaza asa . Trimit cererea serverului sub forma de post la adresa formata din ip si port , cu calea catre zona in care se desfasoara inregistrarea si metoda de formatare "aplication/json". Astept raspunsul serverului pentru a interpreta mesajul si a vizualiza daca cererea a fost creata.(functia jsonResponse se ocupa de asta, imi afiseaza erorile sau mesajele de succes)

Pentru login , salvez datele introduse de client pentru user si password si creez un mesaj in format json pentru a-i trimite serverului . Dupa ce il trimit astept ca acesta sa-mi raspunda si sa preiau cookie-ul din mesajul lui pe care il salvez in cookie_login. Interpretez raspunsul serverului si verific existenta de erori sau daca logarea s-a efectuat cu succes. Mesajul pentru server este creat tot sub forma de post , singura diferenta intre cel de la register cu acesta fiind path-ul.

La enter_library creez o cerere get in care am urmatorii parametrii : adresa serverului(ip:port) / host-ul , calea de acces pentru rezolvarea mesajului , NULL -> fara alti parametrii url, cookie-ul pe care l-am preluat de la login, si NULL pentru ca abia acum urmeaza sa primesc token-ul de autorizatie.
Raspunsul serverului imi intoarce un token pe care il retin in token_auth, folosindul la urmatoarele prelucrari in biblioteca sau la afisari ale unei carti / bibliotecii.
Verific mesajul dupa erori in functia jsonResponse si printez daca acesta are un cod returnat de OK.

Cand citeste clinetul get_books de la tastatura , mesajul pe care i-l trimit serverului este format din host, path-ul prelucrarii , NULL-> no other url parameters, cookie-ul de login si token-ul de autorizare. Validarea token-ului este cruciala pentru a putea primi de la server un mesaj ce contine lista de carti. Verific dupa erori ce vor aparea in cazul in care clientul nu este logat -- Forbiden, sau nu are acces la biblioteca - Unauthorized.
In caz de succes printez un mesaj cu ce-mi intoarce serverul in lista de carti. 

Pentru get_book , mai intai salvez id-ul cartii pe care il inserez in path-ul la care este trimis mesajul . Mesajul catre server arata in felul urmator : adresa, "path/id_carte", NULL-> no other url parameters, cookie , token.
Mesajul este trimis , iar cel intors ca raspuns este parsat dupa erori , iar , in cazul in care nu contine , afisez detaliile cartii. Erorile ce pot aparea : 404 - not found , nu exista o carte cu id-ul acesta , 401 - unauthorized , token invalid, 403 - forbidden -> cookie-ul de login lipseste (la logout le fac pe ambele NULL , o sa imi intoarca mesajul cu unauthorized oricum).

In cazul citirii de la tastatura a "delete_book"-ului e nevoie de o cerere noua , am construit-o in request.c si contine host-ul, url-ul, cookie-ul si authorization-ul.
In aceasta cerere formatarea mesajului principal cel in care se specifica exact cererea pt server este "Delete".
In client , la delete_book , preiau id-ul, il concatenez la path si formez mesajul in care inserez cookie-ul si token-ul pentru a nu exista mesaje de eroare de forma bad_request.
Dupa trimiterea mesajului , astept confirmarea serverului si parsez mesajul dupa erori , iar in cazul in care nu exista printez faptul ca mesajul s-a realizat cu succes si actiunea este executata. Erori posibile : 404 - no book was found, 401 - unauthorized , token-ul invalid / lipseste.

La add_book, mai intai citesc datele cartii pe care vreau s-o inserez , apoi formez un string sub forma aplication/json ce va fi inserat in mesajul trimis catre server.
Ii trimit mesajul serverului cu stringul format , token-ul de autorizare , la path-ul specific pentru prelucrarile pe biblioteca(cele ce nu contin id). Astept confirmarea de la server . Ca metoda de precautie pentru ca am vazut ca se face flood serverului , am introdus si mesajul de eroare cu "Too many request " pentru a putea fi evidenta de ce nu a functionat adaugarea . Erori posibile : 401 - unauthorized, token invalid/lipseste si cea cu 429->too many requests, iar in caz de succes afiseaza confirmarea serverului.

Mi-am creat o functie pentru extragerea cookie-ului numita take_cookie care imi cauta in mesajul de la server "Cookie: " . Daca gaseste acest substring imi aloca un string de dimensiunea 1000 . In substringul cautat mai devreme caut \n-ul si ii retin pozitia in alt string . Fac diferentele de lungimi pentru a stii cat copiez in stringul de retur, cel alocat, si copiez aceasta din substringul principal totul pana la \n. Iar apoi , am grija sa nu copiez si \n-ul . punand pe ultima penultima pozitie a stringului de retur terminatorul de sir \0.

Functia de take_token este asemanatoare , copiez ce se afla intre ghilimele de la pozitia "token":. Am grija sa nu copiez ghilimele , la fel , cu un substring ce este pozitionat pe ultima ghilimea de la sfarsitul caracterelor din token.

Functia jsonResponse are 2 parametrii , mesajul si un flag ce imi indica daca mesajul este de tip add/delete/logout. Pentru a sti ce afisez. Mai intai cauta tipul de raspuns de pe prima linia a mesajului . Iar in functie de valoarea codului de retur a raspunsului de la server printez eroare (iau doar ce se afla intre paranteze erorii).

Am o alta functie de afisare la get_book/get_books ce imi afiseaza continutul mesajului . Imi face strstr pana la inceputul listei de carti (daca exista).

Rulare : ./client
