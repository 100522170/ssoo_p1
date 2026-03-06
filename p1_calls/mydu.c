#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>  // Required for directory operations: opendir, readdir, closedir
#include <fcntl.h>   // Required for file control constants: O_RDONLY, O_WRONLY, O_CREAT, O_APPEND
#include <unistd.h>  // Required for POSIX system calls: open, read, write, lseek, close
#include <limits.h>  // Required for PATH_MAX (maximum number of characters in a path)

/* * Recommended structure to write/read the history in the binary file.
 * We use 'int' for the size instead of 'long' to avoid cross-platform alignment 
 * and size issues (a 'long' might be 4 bytes on 32-bit and 8 bytes on 64-bit systems).
 * PATH_MAX guarantees we have enough space for any valid Linux path.
 */
typedef struct {
    int size;
    char path[PATH_MAX];
} DuHistory;

/* Name of the binary file to write/read the execution history */
const char *binary_file = "mydu.bin";

/**
 * Gets the logical size of a single file in bytes using the lseek system call.
 * * @param filepath The complete path of the file to measure.
 * @return The size of the file in bytes, or 0 if an error occurs.
 */
long get_file_size(const char *filepath) {
    // 1. Open the file in read-only mode using a system call
    int fd = open(filepath, O_RDONLY);
    if (fd == -1) {
        // If we can't open it (e.g., missing read permissions), we return 0 bytes to avoid crashing
        return 0; 
    }
    
    // 2. Use lseek to jump directly to the end of the file (SEEK_END)
    // The return value of this jump is the exact size of the file in bytes
    off_t size = lseek(fd, 0, SEEK_END);
    
    // 3. Always close the file descriptor to free system resources
    close(fd);
    
    // Return the size. If lseek failed, it returns -1, so we safely return 0 instead
    return (size == -1) ? 0 : (long)size;
}

/**
 * Recursively calculates the size of a directory and all its contents.
 * It also dynamically writes the calculated results to the binary history file.
 * * @param dir_path The path of the directory to process.
 * @param history_fd The open file descriptor for the binary history file.
 * @return The total size in bytes of this directory and its contents.
 */
long process_directory(const char *dir_path, int history_fd) {
    // Attempt to open the path as a directory
    DIR *dir = opendir(dir_path);
    if (dir == NULL) {
        // BASE CASE: If opendir fails, it means it is a regular file, not a directory.
        // So we just calculate and return the size of this single file.
        return get_file_size(dir_path);
    }

    struct dirent *entry;
    
    // Base size for the directory itself. In most Linux filesystems (ext4), 
    // a directory entry consumes a minimum of 4096 bytes (1 block).
    long total_bytes = 4096; 

    // Traverse all items (files and subdirectories) inside the current directory
    while ((entry = readdir(dir)) != NULL) {
        // Skip "." (current directory) and ".." (parent directory) 
        // to prevent infinite recursive loops.
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        // Safely construct the full path: "parent_dir/item_name"
        char full_path[PATH_MAX];
        snprintf(full_path, PATH_MAX, "%s/%s", dir_path, entry->d_name);

        // Check if the current item is a subdirectory by trying to open it
        DIR *sub_dir = opendir(full_path);
        if (sub_dir != NULL) {
            // It is a directory: close it and call this function recursively
            closedir(sub_dir);
            total_bytes += process_directory(full_path, history_fd);
        } else {
            // It is a regular file: add its size to the total
            total_bytes += get_file_size(full_path);
        }
    }
    
    // Finished reading the directory, so we close it
    closedir(dir);

    // Convert total bytes to Kilobytes. 
    // We explicitly cast to (int) to avoid the -Wconversion compiler error.
    int size_in_kb = (int)(total_bytes / 1024);
    
    // Requirement: Print the standard output correctly (size in KB and path)
    printf("%d\t%s\n", size_in_kb, dir_path);

    // Requirement: Write the result to the binary file using system calls
    if (history_fd != -1) {
        // Prepare the structure with the data to save
        DuHistory record;
        record.size = size_in_kb;
        
        // Copy the path safely and ensure it is null-terminated
        strncpy(record.path, dir_path, PATH_MAX - 1);
        record.path[PATH_MAX - 1] = '\0'; 
        
        // Write the exact memory block of the structure into the binary file
        if (write(history_fd, &record, sizeof(DuHistory)) == -1) {
            perror("Error writing to binary file");
        }
    }

    // Return the total bytes to be added to upper levels of recursion
    return total_bytes;
}

/**
 * Mode 3 execution (-b): Reads the history from the binary file and prints it.
 * * @return 0 on success, -1 on any error.
 */
int read_history() {
    // Open the binary file in Read-Only mode (O_RDONLY) using a system call
    int fd = open(binary_file, O_RDONLY);
    if (fd == -1) {
        perror("Error opening mydu.bin");
        return -1; // Requirement: Return -1 on error
    }

    // Print the exact header required by the laboratory statement
    printf("Contents of binary file\n");
    
    DuHistory record;
    ssize_t bytes_read;
    
    // Sequentially read blocks of data equal to the size of our structure.
    // read() returns the number of bytes read. If it returns 0, it means EOF (End of File).
    while ((bytes_read = read(fd, &record, sizeof(DuHistory))) > 0) {
        
        // Safety check: ensure we read a complete structure, not a fragmented one
        if (bytes_read != sizeof(DuHistory)) {
            fprintf(stderr, "Error: Incomplete read or corrupted file.\n");
            close(fd);
            return -1;
        }
        
        // Print the read structure to standard output
        printf("%d\t%s\n", record.size, record.path);
    }

    // Check if the read() loop terminated due to a system error (-1)
    if (bytes_read == -1) {
        perror("Error reading history file");
        close(fd);
        return -1;
    }

    // Close the file descriptor and exit successfully
    close(fd);
    return 0;
}


int main(int argc, char *argv[]) {
    // ------------------------------------------------------------------------
    // MODE 3: Print binary file contents if the argument "-b" is provided
    // ------------------------------------------------------------------------
    if (argc == 2 && strcmp(argv[1], "-b") == 0) {
        return read_history();
    }

    // ------------------------------------------------------------------------
    // MODES 1 & 2: Calculate directory size (specific or current directory)
    // ------------------------------------------------------------------------
    
    // If an argument is provided, use it as target (Mode 1).
    // Otherwise, use "." which represents the current directory (Mode 2).
    const char *target = (argc > 1) ? argv[1] : ".";

    // Requirement: Regular files are NOT valid arguments.
    // We check this by attempting to open the target as a directory.
    DIR *check_dir = opendir(target);
    if (check_dir == NULL) {
        // If opendir fails, it's a file or invalid. Print error to standard error.
        fprintf(stderr, "%s: It is not a directory\n", target);
        return -1; // Requirement: return -1 on error
    }
    // If it is a valid directory, close it (we will reopen it inside the recursive function)
    closedir(check_dir);

    // Open the binary file to save the history using a system call.
    // Flags:
    // O_WRONLY -> Write only
    // O_CREAT  -> Create the file if it does not exist
    // O_APPEND -> Append new data at the end of the file, preserving older history
    // Permissions: 0644 (Read/Write for owner, Read for others)
    int history_fd = open(binary_file, O_WRONLY | O_CREAT | O_APPEND, 0644);
    if (history_fd == -1) {
        perror("Error opening binary file for writing");
        return -1;
    }

    // Start the recursive calculation and writing process
    process_directory(target, history_fd);

    // Cleanup: close the binary file descriptor
    if (close(history_fd) == -1) {
        perror("Error closing binary file");
        return -1;
    }

    // Requirement: The program must return 0 if everything worked correctly.
    return 0;
}
