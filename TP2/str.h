#ifndef STR_H
#define STR_H

#include "macros.h"
#include "utils.h"

/**
 * @brief This function returns a newly allocated string, with a left and right trim of the characters
 * passed as argument in set. Example: string: "312Nuno231" set: "123" return: "Nuno"
 * @param s1 The string to be trimmed
 * @param set The inventory of characters to remove
 * @return A new allocated string, NULL otherwise
 */
char	*strtrim(char const *s1, char const *set);

/**
 * @brief Reads a line until the break line in encountered from a given file descriptor. It
 * returns a newly allocated string with the content of the line
 * @param fd The file descriptor to read from
 * @return A newly allocated string with the line content, NULL othwersie
 */
char	*get_line(int fd);

/**
 * @brief Given a string f, it applies the function f to the string s. It returns a newly 
 * allocated string with the new contents in it.
 * @param s The string to apply the function to
 * @param f The function to apply
 * @return A newly allocated string, NULL otherwise
 */
char	*strmap(char const *s, char (*f)(unsigned int, char));

/**
 * @brief Deletes the memory of an array of strings
 * @param mat The matrix to delete the memory from
 */
void	delete_matrix(char **mat);

/**
 * @brief Given a string s and a separator c, this function splits the string on the 
 * delimiter positions. It returns a NULL terminated array of strings.
 * @param s The string to split
 * @param c The delimiter
 * @return A newly allocated array of strings, containing each word, NULL otherwise
 */
char	**split(char const *s, char c);

/**
 * @brief Given a string s, it returns a newly allocated string with a substring starting
 * on start with a length len.
 * @param s The string to retrieve the substring from
 * @param start The index of the first character
 * @param len The length of the substring
 * @return A newly allocated string, NULL otherwise
 */
char	*substr(char const *s, unsigned int start, size_t len);


#endif