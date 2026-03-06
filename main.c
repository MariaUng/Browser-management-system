#include <stdio.h>
#include <stdlib.h>
#include <string.h>


typedef struct Page {
    int id;
    char url[50];
    char *description;
    //page *next;
} page;

typedef struct Stack {
    page *pg;
    struct Stack *next;
} StackCell, *stiva;

//structura care tine datele unui singur tab
struct tab {
    int id;
    page *currentPage;
    stiva backwardStack;
    stiva forwardStack;
};

//struct repr un nod din lista dublu inlantuita care contine un tab si leg catre alte noduri
typedef struct tabsCel {
    struct tab *tabInf;
    struct tabsCel *prev;
    struct tabsCel *next;
} *tabsList;

typedef struct Browser {
    struct tab *current;
    //list este pointer către nodul-santinela al unei liste circulare
    tabsList list;
} browser;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

tabsList initL (tabsList head) {
    //aloc spatiu pentru intreaga struct  
    head = (tabsList) malloc(sizeof(struct tabsCel));
    head->next = head;
    head->prev = head;
    return head;
}


page* init_pg_implicita () {
    page *pg = (page *) malloc(sizeof(page));
    pg->id = 0;
    strcpy(pg->url, "https://acs.pub.ro/");
    pg->description = (char *) malloc(50 * sizeof(char));
    strcpy(pg->description, "Computer Science");
    return pg;
}

browser* init_browser () {
    browser *br = (browser *) malloc(sizeof(browser));
    br->list = initL(br->list);

    //crearea primului tab cu id 0
    tabsList primul_tab;
    primul_tab = (tabsList) malloc(sizeof(struct tabsCel));
    primul_tab->tabInf = (struct tab *) malloc(sizeof(struct tab));
    primul_tab->tabInf->id = 0;
    //creez pagina implicita
    primul_tab->tabInf->currentPage = init_pg_implicita();
    primul_tab->tabInf->backwardStack = NULL;
    primul_tab->tabInf->forwardStack = NULL;

    //legare in lista
    primul_tab->next = br->list;
    primul_tab->prev = br->list;
    br->list->prev = primul_tab;
    br->list->next = primul_tab;
    //setarea tabului curent
    br->current = primul_tab->tabInf;
    return br;
}


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 



//adauga o pagina in varful stivei (pg este pointer la pagina pe care vrem sa o adaugam)
void push(stiva *s, page *pg) {
    stiva pg_noua = (stiva) malloc(sizeof(StackCell));
    pg_noua->pg = pg;
    //legam noul varf de stiva de vechiul varf
    pg_noua->next = *s;
    //noua pagina devine prima in stiva
    *s = pg_noua;
}

//fct care scoate si returneaza pagina din varful stivei
page* pop(stiva *s) {
    if (*s == NULL) {
        return NULL;
    }
    //retin varful stivei
    page *pg;
    pg = (*s)->pg;
    //varf este pointer la primul elem din stiva
    stiva varf = *s;
    //avansam varful stivei
    *s = (*s)->next;
    free(varf);
    return pg;
}

//fct care elibereaza stiva
void free_stiva(stiva *s) {
    while (*s != NULL) {
        stiva elem_scos = *s;
        *s = (*s)->next;
        free(elem_scos);
    }
}


//- - - - - - - - - - - - - - - - - -  - - - - - - - - - - - 

void citire(char *fname, int *nr_pg, int id_pagini[], char url_pgini[][50], char descriere_pg[][50], int *nr_comenzi, char comenzi[][50]) {
    FILE *f = fopen(fname, "r");
    int i;

    fscanf(f, "%d", nr_pg);
    for (i = 0; i < *nr_pg; i++) {
        fscanf(f, "%d", &id_pagini[i]);
        fscanf(f, "%s", url_pgini[i]);
        // citeste \n ramas in buffer
        fgetc(f);
        fgets(descriere_pg[i], 50, f);
        //elimin \n
        descriere_pg[i][strlen(descriere_pg[i]) - 1] = '\0';
    }
    fscanf(f, "%d", nr_comenzi);
    fgetc(f);
    for (i = 0; i < (*nr_comenzi)-1; i++) {
        fgets(comenzi[i], 50, f);
        //elimin \n
        comenzi[i][strlen(comenzi[i]) - 1] = '\0';
    }
    fgets(comenzi[i], 50, f);
    fclose(f);
}

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
    br->current = tab_nou->tabInf;
}

//se afiseaza toate id-urile taburilor deschise
//+ descrierea paginii curente
void print_br(char *fname, browser *br) {
    FILE *f = fopen(fname, "w");
    tabsList tab;
    tab->tabInf = br->current;
    while (tab != br->list) {
        fprintf(f, "%d ", tab->tabInf->id);
        tab = tab->next;
    }
    fprintf(f, "\n");
    fprintf(f, "%s\n", br->current->currentPage->description);
    fclose(f);
}

int main() {
    int i;
    // initializare browser
    browser *br = (browser *)malloc(sizeof(browser));
    // initializare browser
    br = init_browser();
    
    int nr_pg, nr_comenzi;
    int id_pagini[50];
    char url_pgini[50][50];
    char descriere_pg[50][50];
    char comenzi[50][50];
    
    citire("tema1.in", &nr_pg, id_pagini, url_pgini, descriere_pg, &nr_comenzi, comenzi);

    for(i=0; i<nr_comenzi; i++) {
        if(strstr(comenzi[i], "NEW_TAB") != NULL) {
            new_tab(br);
        } else if(strstr(comenzi[i], "PRINT") != NULL) {
            print_br("tema1.out", br);
        }
    }
    return 0;
}