#include <fcntl.h>  /* To use open, read, write, close */
#include <stdlib.h> // important to transform variable types
#include <string.h> // important  for strlen
#include <unistd.h> /*To use write*/

const char *log_file = "mycalc.log";

int main(int argc, char *argv[]) {
  /*
  This function ...
  */

  (void)argv;

  char *arg_error_msg =
      "Error: \n Usage (1): ./mycalc <num1> <operation (+|-|x|/)> <num2> \n "
      "Usage (2): ./mycalc -b <num operation>";

  // we check if there are between 2 and characters
  if (argc < 3 || argc > 4) {
    if (write(2, arg_error_msg, strlen(arg_error_msg)) <
        0) { // write error msg and return-1 if write fails
      return -1;
    }

    return -1;
  }

  char *history_mode = "History mode:";
  char *calc_mode = "Calculator mode";

  /* argc== 4 means maybe Usage (1): ./mycalc <num1> <operation (+|-|x|/)>
   * <num2>
   */
  if (argc == 4) {

    // comprobamos si el simbolo es valido o no, tiene que ser +|-|x|/)
    if (strcmp(argv[2], "+") == 0 || strcmp(argv[2], "-") == 0 ||
        strcmp(argv[2], "x") == 0 || strcmp(argv[2], "/") == 0) {

      char *end;
      long n1 = strtol(argv[1], &end, 10);

      if (argv[1] == end || *end != '\0') {
        if (write(2, arg_error_msg, strlen(arg_error_msg) < 0)) {
          return -1;
        }
        return -1;
      }

    } else {
      if (write(2, arg_error_msg, strlen(arg_error_msg)) <
          0) { // write error msg and return-1 if write fails
        return -1;
      }
      return -1;
    }
  }

  if (argc == 3) {

    if (strcmp(argv[1], "-b") == 0) {

      if (write(1, history_mode, strlen(history_mode)) < 0) {
        return -1;
      }
    }
  }

  return 0; // everythign perfect
}
