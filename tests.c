#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "lib_tar.h"

/**
 * You are free to use this file to write tests for your implementation
 */

void debug_dump(const uint8_t *bytes, size_t len) {
    for (int i = 0; i < len;) {
        printf("%04x:  ", (int) i);

        for (int j = 0; j < 16 && i + j < len; j++) {
            printf("%02x ", bytes[i + j]);
        }
        printf("\t");
        for (int j = 0; j < 16 && i < len; j++, i++) {
            printf("%c ", bytes[i]);
        }
        printf("\n");
    }
}

int main(int argc, char **argv) {
    if (argc < 2) {
        printf("Usage: %s tar_file\n", argv[0]);
        return -1;
    }

    int fd = open(argv[1] , O_RDONLY);
    if (fd == -1) {
        perror("open(tar_file)");
        return -1;
    }

    printf("\nDescribe: check_archive\n");
    int check = check_archive(fd);
    printf("It should return 0 : ");
    printf("returned %d\n", check);

    printf("\nDescribe: exists\n");
    int exist = exists(fd, "lib_tar.w");
    printf("It should return 0 : ");
    printf("returned %d\n", exist);
    exist = exists(fd, "lib_tar.c");
    printf("It should return 1 : ");
    printf("returned %d\n", exist);

    printf("\nDescribe: is_dir\n");
    int dir = is_dir(fd, "tests.c");
    printf("It should return 0 : ");
    printf("returned %d\n", dir);
    dir = is_dir(fd, "test/");
    printf("It should return 1 : ");
    printf("returned %d\n", dir);

    printf("\nDescribe: is_file\n");
    int file = is_file(fd, "test/");
    printf("It should return 0 : ");
    printf("returned %d\n", file);
    file = is_file(fd, "tests.c");
    printf("It shoud return 1 : ");
    printf("returned %d\n", file);
    
    printf("\nDescribe: is_symlink\n");
    int link = is_symlink(fd, "tests.c");
    printf("It should return 0 : ");
    printf("returned %d\n", link);

    return 0;
}