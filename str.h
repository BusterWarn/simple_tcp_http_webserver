/**
 * str.h
 * 
 * Description: Some help functions for string parsing.
 * 
 * Author:      Buster Hultgren Wärn - busterw@cs.umu.se
 * Date:        2019-09-10
*/

/*
* description: Creates and allocates memory for a string.
* param[in]: buffer - The string content.
* returns: The string.
*/
char *strCreate(char *buffer);

/*
* description: Checks if a strings beginning compares true to another string.
* param[in]: str - The string.
* param[in]: compStr - The comparision string.
* returns: 1 if true; 0 if false.
*/
int strStartsWith(char *str, char *compStr);

/*
* description: Checks if a strings beginning compares true to another string.
* param[in]: str - The string.
* param[in]: compStr - The comparision string.
* returns: 1 if true; 0 if false.
*/
int strEndsWith(char *str, char *compStr);

/*
* description: From a word surrounded by whitespace, gets that word. Allocates
               memory from that word.
* param[in]: str - The string containing the word.
* returns: The word.
*/
char *strGetWordFromString(char *str);
