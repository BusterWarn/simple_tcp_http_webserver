/**
 * str.c
 * 
 * Description: Some help functions for string parsing.
 * 
 * Author:      Buster Hultgren Wärn - busterw@cs.umu.se
 * Date:        2019-09-10
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "str.h"

/*
* description: Creates and allocates memory for a string.
* param[in]: buffer - The string content.
* returns: The string.
*/
char *strCreate(char *buffer) {
    int len = strlen(buffer);
    char *str = malloc((sizeof(char) * len) + 1);
    strncpy(str, buffer, len);
    str[len] = '\0';
    return str;
}

/*
* description: Checks if a strings beginning compares true to another string.
* param[in]: str - The string.
* param[in]: compStr - The comparision string.
* returns: 1 if true; 0 if false.
*/
int strStartsWith(char *str, char *compStr) {
    
    int i = 0;
    while(compStr[i] != '\0') {
        if (compStr[i] == '\0') return 1;
        if (str[i] != compStr[i]) return 0;
        i++;
    }
    return 1;
}

/*
* description: Checks if a strings beginning compares true to another string.
* param[in]: str - The string.
* param[in]: compStr - The comparision string.
* returns: 1 if true; 0 if false.
*/
int strEndsWith(char *str, char *compStr) {

    int n = strlen(str) - 1;
    int m = strlen(compStr) - 1;

    while(str[n] == '\r' || str [n] == '\n') {
        n--;
    }

    if (m > n) return 0;
    while(m >= 0) {
        if (str[n] != compStr[m]) return 0;
        n--;
        m--;
    }
    return 1;
}

/*
* description: From a word surrounded by whitespace, gets that word. Allocates
               memory from that word.
* param[in]: str - The string containing the word.
* returns: The word.
*/
char *strGetWordFromString(char *str) {

    char *word;
    int begin = -1, end = -1, i = 0;
    while (str[i] != '\0' && end < 0) {
        if (   str[i] == 9  // Horizontal tab
            || str[i] == 10 // New line
            || str[i] == 11 // Vertical tab
            || str[i] == 12 // New page
            || str[i] == 13 // Carriage return
            || str[i] == 32 // Space
        ) {
            
            if (begin >= 0) end = i;
        } else  if (begin < 0) begin = i;
        i++;
    }
    if (begin < 0) {
        word = strCreate(str);
    } else if (end < 0) {
        word = strCreate(str + begin);
    } else {
        int len = end - begin;
        word = malloc(sizeof(char) * (len + 1));
        strncpy(word, str + begin, len);
        word[len] = '\0';
    }
    return word;
}
