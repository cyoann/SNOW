#include "utils.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void skip_whitespace(const char **str) {
  while (**str != '\0' && isspace((unsigned char)**str)) {
    (*str)++;
  }
}

int str_to_int_array(const char *str, char delimiter, int *arr, int arr_size) {
  // If no delimiter is found, assume the entire string is a single number
  if (strchr(str, delimiter) == NULL) {
    int num = atoi(str);
    if (num != 0 || *str == '0') {
      arr[0] = num;
      return 1;
    }
    return -1;
  }

  int count = 0;
  const char *start = str;
  while (*str != '\0') {
    if (*str == delimiter) {
      if (count < arr_size) {
        int num = atoi(start);
        if (num != 0 || *start == '0') {
          arr[count] = num;
          count++;
        }
        start = str + 1;
      }
    }
    str++;
  }

  if (count < arr_size) {
    int num = atoi(start);
    if (num != 0 || *start == '0') {
      arr[count] = num;
      count++;
    }
  }

  return count;
}

int randinrange(int min, int max) { return (rand() % (max - min + 1)) + min; }
