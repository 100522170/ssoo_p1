#include <fcntl.h> /* To use open, read, write, close */
#include <string.h>
#include <unistd.h> /*To use write*/

const char *log_file = "mycalc.log";
int main(int argc, char *argv[]) {
  (void)argv;
  char *error_msg = "Error: the arguments must be between 2 and 3\n";

  /*First we check if the user used from 2 to 3 arguments, more or less will
  raise an error because is not calculator mode neither history mode*/

  if (argc < 3 || argc > 4) {

    if (write(2, error_msg, strlen(error_msg)) < 0) {
      return -1;
    }

    return -1;
  }

  char *history_mode = "Entrando en modo historial";
  char *calc_mode = "Entrando en modo historial";
  if (argc == 3) {

    if (strcmp(argv[1], "-b") == 0) {

      if (write(1, history_mode, strlen(history_mode)) < 0) {
        return 1;
      }
    }
  }

  if (argc == 4) {

    if (write(1, calc_mode, strlen(calc_mode)) < 0) {
      return 0;
    }
  }
  /* Complete */

  return 0;
}
