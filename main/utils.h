#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdlib.h>

/**
 * @brief Skips whitespace characters in a string.
 *
 * @param str Pointer to the string.
 */
void skip_whitespace(const char **str);

/**
 * @brief Function to convert a string to an array of integers
 *
 * @param str The string to convert (e.g. "1,2,3,4")
 * @param delimiter The delimiter to use (e.g. ',')
 * @param arr The array to store the integers
 * @param arr_size The size of the array
 * @return int The number of integers converted
 */
int str_to_int_array(const char *str, char delimiter, int *arr, int arr_size);

/**
 * @brief Function to generate a random number in a given range
 *
 * @param min The minimum value
 * @param max The maximum value
 * @return int The random number
 */
int randinrange(int min, int max);

#endif // UTILS_H
