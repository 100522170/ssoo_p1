#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>

/* Recommended structure*/
struct record {
    long size;
    char path[1024];
};

const char *binary_file = "mydu.bin";

/**
 * Saves a result to the binary file using system calls.
 */
void save_to_history(long size, const char *path) {
    int fd = open(binary_file, O_WRONLY | O_CREAT | O_APPEND, 0644);
    if (fd < 0) return;

    struct record r;
    r.size = size;
    strncpy(r.path, path, sizeof(r.path) - 1);
    r.path[sizeof(r.path) - 1] = '\0';

    if (write(fd, &r, sizeof(struct record)) < 0) {
        perror("Error writing to history");
    }
    close(fd);
}

/**
 * Reads the history from the binary file (Mode -b).
 */
int read_history() {
    int fd = open(binary_file, O_RDONLY);
    if (fd < 0) {
        perror("Error opening history file");
        return -1;
    }

    printf("Contents of binary file\n");
    struct record r;
    while (read(fd, &r, sizeof(struct record)) > 0) {
        printf("%ld\t%s\n", r.size, r.path);
    }
    close(fd);
    return 0;
}

/**
 * Gets the logical size of a file in bytes using lseek.
 */
long get_size_with_lseek(const char *path) {
    int fd = open(path, O_RDONLY);
    if (fd < 0) return 0;

    /* Move pointer to the end to get the size in bytes */
    off_t size = lseek(fd, 0, SEEK_END);
    close(fd);
    
    return (size >= 0) ? (long)size : 0;
}

/**
 * Recursively calculates directory
 */
long calculate_size_recursive(const char *path) {
    DIR *dir = opendir(path);
    if (!dir) {
        return get_size_with_lseek(path);
    }

    struct dirent *entry;
    long total_bytes = 0;

    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        char full_path[2048];
        snprintf(full_path, sizeof(full_path), "%s/%s", path, entry->d_name);

        DIR *sub = opendir(full_path);
        if (sub) {
            closedir(sub);
            total_bytes += calculate_size_recursive(full_path);
        } else {
            total_bytes += get_size_with_lseek(full_path);
        }
    }
    closedir(dir);

    long size_kb = total_bytes / 1024;
    printf("%ld\t%s\n", size_kb, path);
    save_to_history(size_kb, path);

    return total_bytes;
}

int main(int argc, char *argv[]) {
    if (argc > 1 && strcmp(argv[1], "-b") == 0) {
        return read_history();
    }

    const char *target = (argc > 1) ? argv[1] : ".";

    DIR *check = opendir(target);
    if (!check) {
        fprintf(stderr, "%s: It is not a directory\n", target);
        return -1;
    }
    closedir(check);

    if (calculate_size_recursive(target) < 0) {
        return -1;
    }

    return 0;
}