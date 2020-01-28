/*
Jouni Saari 2519078
Sanan maksimipituus 100.
*/

#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>

#define ARRAY_SIZE 50021                    // HASH-taulukon koko

unsigned char hashArray[ARRAY_SIZE][100];   // HASH-taulukko

int tila;                                   // 1: Tallentaa ensimmäistä tiedostoa, 2: Etsii erilaisia sanoja
int same_word_count;
int word_count;

unsigned char wordlist[50][100];            // Taulukko johon 50 erilaista sanaa tallennetaan


unsigned long hashstring(unsigned char *str);
int hash_save(unsigned char* data);
int hash_search(unsigned char* data);
int hashIndex(int key);
int read_hasher(char* file);
int wordlister(unsigned char* word);


/* Ohjelmaa pyörittävä funktio */
int main() {
    int y = 0;
    char file[20];
    char file2[20];
    int read_hasher_return;
    

    printf("Ensimmainen tiedosto tallennetaan ja toisesta tiedostosta\n");
    printf("etsitaan 50 ensimmaista erilaista sanaa, joita ei esiinny\n");
    printf("ensimmaisessa tiedostossa.\n");
    
    /* Ensimmäisen tiedoston tallennus */
    tila = 1;
    while (1){
        printf("Anna ensimmaisen tiedoston nimi:\n");
        scanf("%s", file);
        read_hasher_return = read_hasher(file);
        if(read_hasher_return > 0){
            printf("Tiedostoa ei voitu avata, tarkista oikeinkirjoitus.\n");
        } else if(read_hasher_return == -1){    // Jos tosi, lopputulostaulukko on täynnä
            return 0;
        } else {
            break;
        }
    }
    
    /* Etsitään toisesta tiedostosta 50 sanaa jotka eivät esiinny ensimmäisessä tiedostossa */
    tila = 2;
    while (1){
        printf("Anna toisen tiedoston nimi:\n");
        scanf("%s", file2);
        if(read_hasher(file2) < 0){
            printf("Tiedostoa ei voitu avata, tarkista oikeinkirjoitus.\n");
        } else {
            break;
        }
    }
    
    /* Tulosten tulostus */
    printf("Sanojen maara (%s): %i\n", file, word_count);
    int different_word_count = word_count - same_word_count;
    printf("Erilaisten sanojen maara (%s): %i\n", file, different_word_count);
    for(y = 0; y < 50; y++){
        if(strcmp(wordlist[y], "") == 0){
            if(y == 0){
                printf("Kaikki sanat esiintyivat ensimmaisessa tiedostossa");
            }
            break;
        }
        printf("%i: %s\n", y + 1, wordlist[y]);
    }
    return 0;
}

/* Lukee tiedososta sanoja ja tilasta riippuen kutsuu tallennus- tai etsintäfunktiota */
int read_hasher(char* file)
{
    FILE *fp;
    
    char line[500];
    unsigned char word[100];
    int len;
    int i = 0;
    int p = 0;
    int sana = 0;
    
    int hash_search_return;
    int wordlister_return;
    int hash_save_return;
    
    fp = fopen(file, "r");
    if (fp == NULL){
        return 1;
    }
    while (fgets(line, 500, fp) != NULL) {
        len = strlen(line);
        if (len == 1) {
            continue;
        }
        /* Käydään rivi läpi merkki merkiltä */
        for (i = 0; i < len; i++) {
            if(isalpha(line[i])){
                if(isupper(line[i])){
                    line[i] = tolower(line[i]);
                }
                word[p] = line[i];
                p++;
            } else if(isspace(line[i])) {
                if(p != 0) {
                    sana = 1;
                }
            } else /*(ispunct(line[i]))*/ {
                if(line[i] == 39) {
                    word[p] = line[i];
                    p++;
                } else {
                    if(p > 1){
                        sana = 1;
                    }
                }
            }
            
            /* Jos sana = 1 niin kutsutaan tallennus- tai etsintäfunktiota tilasta riippuen */
            if(sana == 1){
                if(tila == 1){
                    hash_save_return = hash_save(word);
                    if(hash_save_return == -1){
                        return -1;
                    }
                    word_count++;
                } else if(tila == 2){
                    hash_search_return = hash_search(word);
                    if(hash_search_return < 0){
                        wordlister_return = wordlister(word);
                        if(wordlister_return == 1){
                            return 1;
                        }
                    }
                }
                p = 0;
                memset(word, 0, 100);
                sana = 0;
            }
        }
    }
    return 0;
}

/* Antaa sille annetulle sanalle hash-indeksin (djb2 - http://www.cse.yorku.ca/~oz/hash.html) */
unsigned long hashstring(unsigned char *str)
{
    unsigned long hash = 5381;
    int c;


    while ( (c = *str++) ){
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
    }
    hash %= ARRAY_SIZE;
    return hash;
}

/* Tallentaa sille annetun sanan hash-taulukkoon. */
int hash_save(unsigned char* data){
    unsigned long hashIndex;
    unsigned char word[100];
    unsigned long if_full;
    
    strcpy(word, data);
    
    hashIndex = hashstring(data);
    
    if_full = hashIndex - 1;

    while(strcmp(hashArray[hashIndex], "") != 0){
        if(strcmp(hashArray[hashIndex], word) == 0){
            same_word_count++;
            break;
        } else {
            hashIndex++;
            if(hashIndex > ARRAY_SIZE){
                hashIndex = 0;
            }
            if(hashIndex == if_full){                       // Tarkistaa onko HASH-taulukko jo täysi
                printf("hashArray is full!");
                return -1;
            }
        }
    }
    strcpy(hashArray[hashIndex], word);
    return 0;
}

/* Etsii sille annettua sanaa hash-taulukosta ja palauttaa -1 jos sanaa ei löydy */
int hash_search(unsigned char* data){
    unsigned long hashIndex;
    unsigned char* word;
    
    strcpy(word, data);
    
    hashIndex = hashstring(data);
    
    while(strcmp(hashArray[hashIndex], "") != 0){
        if(strcmp(hashArray[hashIndex], word) == 0){
            return 1;
        } else {
            hashIndex++;
            if(hashIndex > ARRAY_SIZE){
                hashIndex = 0;
            }
        }
    }
    return -1;
}

/* Lisää sille annetun sanan tuloslistaan */
int wordlister(unsigned char* data){
    int resultIndex = 0;
    unsigned char* word;
    strcpy(word, data);
    if(strcmp(wordlist[0], "") != 0){
        while((strcmp(wordlist[resultIndex], "") != 0) && (resultIndex < 51)){
            if(strcmp(wordlist[resultIndex], word) == 0){
                return -1;
            } else {
                resultIndex++;
                if(resultIndex > 50){
                    return 1;
                }
            }
        }
    }
    strcpy(wordlist[resultIndex], word);
    return 0;
}


