#include <fcntl.h>  /* To use open, read, write, close */
#include <stdlib.h> // important to transform variable types strol
#include <string.h> // important  for strlen and strcmp
#include <unistd.h> /*To use write*/

#define LOG_FILE "mycalc.log"
#define array_SIZE 256

int write_str(int fd, char *s) { // write str and check if error
  if (write(fd, s, strlen(s)) < 0) {
    return -1; // error
  }
  return 0; // all perfect
}

int print_usage_error(void) { // print the usage str in case of error
  if (write_str(2, "Error: \n Usage (1): ./mycalc <num1> <operation (+|-|x|/)> "
                   "<num2> \n "
                   "Usage (2): ./mycalc -b <num operation>") <
      0) { // write error msg and return-1 if write fails
    return -1;
  }
  return 0;
}
void long_to_str(long val, char *array) {
  int i = 0;
  int start = 0;
  int neg = 0;

  if (val < 0) {
    neg = 1;
    val = -val;
  }

  /* Generate digits in reverse order */
  do {
    array[i++] = '0' + (char)(val % 10);
    val /= 10;
  } while (val > 0);

  if (neg) {
    array[i++] = '-';
  }

  array[i] = '\0';

  /* Reverse the string */
  start = 0;
  i--;
  while (start < i) {
    char tmp = array[start];
    array[start] = array[i];
    array[i] = tmp;
    start++;
    i--;
  }
}
int calc_mode(const char *num_1, char *operand,
              char *num_2) { // proces of the calculator mode

  long num1, num2;
  long result;
  char line[array_SIZE];
  char tmp[64];
  int fd;
  int len;

  char *end1;
  char *end2;

  // we change (first number) and (second number) from string to long
  num1 = strtol(num_1, &end1, 10);

  num2 = strtol(num_2, &end2, 10);

  // if any of the numbers is not correct, we return error
  if ((*end1 != '\0') || (*end2 != '\0')) {
    if (write_str(2, "Error: Operands must be numbers\n") < 0) {
      return -1;
    }
    return -1;
  }

  // checking if the operator is only one like + - x /
  if (operand[0] == '\0' || operand[1] != '\0') {
    if (write_str(2, "Error: Operant0r must be one sign only\n") < 0) {
      return -1;
    }
    return -1;
  }

  // now we do the calculation

  switch (operand[0]) { // managing each case of operand
  case '+':
    result = num1 + num2;
    break;
  case '-':
    result = num1 - num2;
    break;
  case 'x':
    result = num1 * num2;
    break;
  case '/':
    if (num2 == 0) {
      if (write_str(2, "Error: Division by zero\n") < 0)
        return -1;
      return -1;
    }
    result = num1 / num2;
    break;
  default:
    if (write_str(2, "Error: Invalid operation\n") < 0)
      return -1;
    return -1;
  }
  len = 0;
  /* Copy "Operation: " */
  {
    const char *prefix = "Operation: ";
    int plen = (int)strlen(prefix);
    int i;
    for (i = 0; i < plen; i++) {
      line[len++] = prefix[i];
    }
  }

  /* num1 */
  long_to_str(num1, tmp);
  {
    int i;
    for (i = 0; tmp[i] != '\0'; i++) {
      line[len++] = tmp[i];
    }
  }

  line[len++] = ' ';
  line[len++] = *operand;
  line[len++] = ' ';

  /* num2 */
  long_to_str(num2, tmp);
  {
    int i;
    for (i = 0; tmp[i] != '\0'; i++) {
      line[len++] = tmp[i];
    }
  }

  /* " = " */
  line[len++] = ' ';
  line[len++] = '=';
  line[len++] = ' ';

  /* result */
  long_to_str(result, tmp);
  {
    int i;
    for (i = 0; tmp[i] != '\0'; i++) {
      line[len++] = tmp[i];
    }
  }

  line[len++] = '\n';
  line[len] = '\0';

  /* Print to stdout */
  if (write(1, line, (size_t)len) < 0) {
    return -1;
  }

  /* Append to log file */
  fd = open(LOG_FILE, O_WRONLY | O_CREAT | O_APPEND, 0644);
  if (fd < 0) {
    if (write_str(2, "Error: Cannot open log file\n") < 0)
      return -1;
    return -1;
  }
  if (write(fd, line, (size_t)len) < 0) {
    close(fd);
    return -1;
  }
  if (close(fd) < 0) {
    return -1;
  }

  return 0;
}

int history_mode(char *line) {

  long line_num;
  int fd;
  int current_line;
  char ch;
  ssize_t bytes_read;
  char line_array[array_SIZE];
  int line_len;
  char tmp[64];

  /* Parse and validate line number */
  {
    char *endptr;
    line_num = strtol(line, &endptr, 10);
    if (*endptr != '\0' || line_num <= 0) {
      if (write_str(2, "Error: Invalid line number\n") < 0)
        return -1;
      return -1;
    }
  }

  /* Open log file for reading */
  fd = open(LOG_FILE, O_RDONLY);
  if (fd < 0) {
    if (write_str(2, "Error: Cannot open log file\n") < 0)
      return -1;
    return -1;
  }

  /* Find the requested line */
  current_line = 1;
  line_len = 0;

  while ((bytes_read = read(fd, &ch, 1)) > 0) {
    if (current_line == line_num) {
      if (ch == '\n') {
        /* Reached end of the desired line */
        line_array[line_len] = '\0';
        break;
      }
      if (line_len < array_SIZE - 1) {
        line_array[line_len++] = ch;
      }
    } else {
      if (ch == '\n') {
        current_line++;
      }
    }
  }

  close(fd);

  /* Check if the requested line was found */
  if (current_line != line_num ||
      (current_line == line_num && line_len == 0 && bytes_read <= 0)) {
    if (write_str(2, "Error: Invalid line number\n") < 0)
      return -1;
    return -1;
  }

  /* Print: "Line <N>: <content>\n" */
  if (write_str(1, "Line ") < 0)
    return -1;
  long_to_str(line_num, tmp);
  if (write_str(1, tmp) < 0)
    return -1;
  if (write_str(1, ": ") < 0)
    return -1;
  if (write(1, line_array, (size_t)line_len) < 0)
    return -1;
  if (write_str(1, "\n") < 0)
    return -1;

  return 0;
}

int main(int argc,
         char *argv[]) { // checks the arguments provided by the user and if
                         // it should return error, calc mode or history mode

  // we check if there are less than 3 characters
  if (argc < 3) {
    print_usage_error();
    return -1;
  }

  // history mode
  if (strcmp(argv[1], "-b") == 0) {
    if (argc != 3) {
      print_usage_error();
      return -1;
    }
    return history_mode(argv[2]);
  }

  // calc mode
  if (argc != 4) {
    print_usage_error();
    return -1;
  }

  return calc_mode((argv[1]), argv[2], argv[3]);
}