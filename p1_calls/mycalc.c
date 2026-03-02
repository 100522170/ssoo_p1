#include <fcntl.h>  /* To use open, read, write, close */
#include <stdlib.h> // important to transform variable types
#include <string.h> // important  for strlen
#include <sys/types.h>
#include <unistd.h> /*To use write*/

const char *log_file = "mycalc.log";

int valid_number(char *str, char *end) {
  /*
  this function checks if a str contains only numbers
  */

  if (str == end || *end != '\0') {
    return 0; // false
  }
  return 1; // true
}

int print_result(long n) {
  /*
  Function that prints the result
  */
  const char *msg = (n < 0) ? "result = -" : "result";
  size_t len = strlen(msg);
  if (write(1, msg, len) != (ssize_t)len) {
    return -1;
  }
  return 0;
}

int main(int argc, char *argv[]) {
  /*
  This function  does...(NO ESTA COMPLETO)
  */

  // normal error message for almost all errors
  char *arg_error_msg =
      "Error: \n Usage (1): ./mycalc <num1> <operation (+|-|x|/)> <num2> \n "
      "Usage (2): ./mycalc -b <num operation>";

  char *a = "Error: \n miau \n";

  // we check if there are between 2 and 3 characters
  if (argc < 2 || argc > 4) {
    if (write(2, a, strlen(a)) <
        0) { // write error msg and return-1 if write fails
      return -1;
    }
    return -1;
  }

  /*
  argc== 4 means maybe we are in the c_mode Usage (1): ./mycalc <num1>
  <operation (+|-|x|/)>
   */
  if (argc == 4) {

    /*
    Now we will check if both numbers are ok
    */
    // first number
    char *end1;
    // Second  number
    char *end2;
    // we change argv[1] (first number) from string to long
    long n1 = strtol(argv[1], &end1, 10);
    // we change argv[3] (second number) from string to long
    long n2 = strtol(argv[3], &end2, 10);

    // if any of the numbers is not correct, we return error
    if (!valid_number(argv[1], end1) || !valid_number(argv[3], end2)) {
      if (write(2, arg_error_msg, strlen(arg_error_msg)) < 0) {
        return -1;
      }
      return -1;
    }

    // now we do the calculation
    long result;
    if (strcmp(argv[2], "+") == 0) {
      result = n1 + n2;
    } else if (strcmp(argv[2], "-") == 0) {
      result = n1 - n2;
    } else if (strcmp(argv[2], "x") == 0) {
      result = n1 * n2;
    } else if (strcmp(argv[2], "/") == 0) {

      if (n2 == 0) {

        char *div_error = "Error; Division by zero \n";
        if (write(2, div_error, strlen(div_error)) < 0) {
          return -1;
        }
        return -1;
      }
      result = n1 / n2;

    } else { // the symbol was not correct so we return error
      if (write(2, arg_error_msg, strlen(arg_error_msg)) <
          0) { // write error msg and return-1 if this also fails
        return -1;
      }
      return -1;
    }
    print_result(result);
    // we print and the result goes to the log

    return 0; // everythign perfect
  }
  /*
  Now we start with the history mode
  */
  if (argc == 3) {

    if (strcmp(argv[1], "-b") == 0) {
    }
  }
}