//fct care creeaza un nou tab in lista de taburi
void new_tab (browser *br) {
    //creez un tab nou
    tabsList tab_nou;
    tab_nou = (tabsList) malloc(sizeof(struct tabsCel));

    // aloc mem tabului ca sa-i pot atribui inf despre: id, pg curenta (care e cea implicita) si cele backward si forward care sunt nule
    tab_nou->tabInf = (struct tab *) malloc(sizeof(struct tab));
    //ii dau tabului nou un id urmator ca valoare ultimului din lista de taburi
    tab_nou->tabInf->id = br->current->id + 1;
    tab_nou->tabInf->currentPage = (page *) malloc(sizeof(page));
    //setez ca pg curenta pe cea implicita cu id 0
    tab_nou->tabInf->currentPage = init_pg_implicita();
    tab_nou->tabInf->backwardStack = NULL;
    tab_nou->tabInf->forwardStack = NULL;

    //leg noul tab de lista
    //lista fiind circulara dupa acest ultim tab adaugat trb sa urmeze primul tab
    tab_nou->next = br->list;
    tab_nou->prev = br->list->prev;
    br->list->prev->next = tab_nou;
    br->list->prev = tab_nou;

    //setez ca tab curent pe cel nou
    br->current = tab_nou;
    //return br->list;  //???
}

//fct care deschide o pg deja existenta dupa un id specificat
//fct care sterge tabul curent
void close_tab(browser *br) {
    if(br->current == 0) {
        printf("403 Forbidden\n");
        return;
    }

    tabsList tab_sters;
    tab_sters = br->list->next;
    //trecem prin fiecare tab din lista pana il gasim pe cel pe care vrem sa-l stergem (cel curent)
    while(tab_sters != br->list && tab_sters->tabInf != br->current) {
        tab_sters = tab_sters->next;
    }
    tab_sters->prev->next = tab_sters->next;
    tab_sters->next->prev = tab_sters->prev;

    //eliberez memoria ocupata de tabul sters
    free(tab_sters->tabInf->currentPage->description);
    free(tab_sters->tabInf->currentPage);
    free(tab_sters->tabInf);
    free(tab_sters);
    //return br->list;  //???
}

void open_tab(browser *br, int id){
    //verific daca exista tabul cu id-ul dat
    if(id > br->list->prev->tabInf->id) {
        printf("403 Forbidden\n");
        return;
    }

    tabsList tab_deschis;
    tab_deschis = br->list->next;
    //trece prin fiecare tab din lista pana il gasesc pe cel pe care vreau sa-l deschid
    while(tab_deschis != br->list && tab_deschis->tabInf->id != id) {
        tab_deschis = tab_deschis->next;
    }
    //verific daca am ajuns la santinela fara sa gasesc tab-ul
    if(tab_deschis == br->list) {
        printf("403 Forbidden\n");
        return;
    }
    br->current = tab_deschis->tabInf;
    //return br->list;  //???
}

//fct care deschide urmatorul tab dupa cel curent
void next_tab(browser *br) {
    tabsList tab_crt;
    tab_crt = br->list->next;
    //trece prin fiecare tab din lista pana il gasesc pe cel curent
    while(tab_crt != br->list && tab_crt->tabInf != br->current) {
        tab_crt = tab_crt->next;
    }
    //daca next-ul lui current este santinela atunci trecem la primul tab
    if(tab_crt->next == br->list) {
        br->current = br->list->next->tabInf;
    } else {
        br->current = tab_crt->next->tabInf;
    }
}

//fct care deschide tab-ul anterior celui curent
void prev_tab(browser *br) {
    tabsList tab_crt;
    tab_crt = br->list->next;
    //trece prin fiecare tab din lista pana il gasesc pe cel curent
    while(tab_crt != br->list && tab_crt->tabInf != br->current) {
        tab_crt = tab_crt->next;
    }
    //daca prev-ul lui current este santinela atunci trecem la ultimul tab
    if(tab_crt->prev == br->list) {
        br->current = br->list->prev->tabInf;
    } else {
        br->current = tab_crt->prev->tabInf;
    }
}

//fct care deschide pg cu id-ul specificat din tabul curent
void open_page(browser *br, int id, page *pagini, int nr_pg) {
    page *pg_noua = NULL;
    int i, ok = 0;
    //verific daca exista vreo pg intre cele primite in fis de input al carei id sa se potriveasca cu id-ul cautat
    for(i = 0; i < nr_pg && ok == 0; i++) {
        if(pagini[i].id == id) {
            pg_noua = &pagini[i];
            ok = 1;
        }
    }

    //daca nu am gasit pagina cu id-ul dat atunci returnez eroare
    if(pg_noua == NULL) {
        printf("403 Forbidden\n");
        return;
    }

    //salvez pagina curenta in stiva backward
    push(&br->current->backwardStack, br->current->currentPage);

    //golesc stiva forward
    free_stiva(&br->current->forwardStack);

    //pun pg_noua ca pagina currenta
    br->current->currentPage = pg_noua;
}


// //fct care acceseaza ultima pg din backwardStack a tabului curent
// void backward(browser *br) {
//     //verific daca exista vreo pagina in stiva backward a tabului curent
//     if (br->current->backwardStack == NULL) {
//         printf("403 Forbidden\n");
//         return;
//     }

//     //pun pagina curenta in varful stivei forward a tabului curent
//     page *temporar = br->current->forwardStack;
//     br->current->forwardStack = br->current->currentPage;
//     //legam noul varf de stiva de vechiul varf de stiva
//     br->current->forwardStack->next = temporar;


//     //setez ca pagina curenta pe cea din stiva backward
//     br->current->currentPage = br->current->backwardStack;
//     br->current->backwardStack = br->current->backwardStack->next;
// }

// void forward(browser *br) {
//     //verific daca am vreo pagina in stiva forward a tabului curent
//     if (br->current->forwardStack == NULL) {
//         printf("403 Forbidden\n");
//         return;
//     }

//     //pun pagina curenta in varful stivei backward a tabului curent
//     page *temporar = br->current->backwardStack;
//     br->current->backwardStack = br->current->currentPage;
//     //legam noul varf de stiva backward de vechiul varf de stiva
//     br->current->backwardStack->next = temporar;

//     //setez ca pagina curenta pe cea din varful stivei forward
//     br->current->currentPage = br->current->forwardStack;
//     br->current->forwardStack = br->current->forwardStack->next;
// }