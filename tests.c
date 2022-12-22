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

    /**
     * @brief check_archive UT
     */
    printf("\nDescribe: check_archive\n");

    int check = check_archive(fd);
    printf("It should return 11 : ");
    printf("returned %d\n", check);

    /**
     * @brief exists UT
     */
    printf("\nDescribe: exists\n");

    int exist = exists(fd, "lib_tar.w");
    printf("It should return 0 : ");
    printf("returned %d\n", exist);

    exist = exists(fd, "lib_tar.c");
    printf("It should return 1 : ");
    printf("returned %d\n", exist);

    /**
     * @brief is_dir UT
     */
    printf("\nDescribe: is_dir\n");

    int dir = is_dir(fd, "tests.c");
    printf("It should return 0 : ");
    printf("returned %d\n", dir);

    dir = is_dir(fd, "test/");
    printf("It should return 1 : ");
    printf("returned %d\n", dir);

    /**
     * @brief is_file UT
     */
    printf("\nDescribe: is_file\n");

    int file = is_file(fd, "test/");
    printf("It should return 0 : ");
    printf("returned %d\n", file);

    file = is_file(fd, "tests.c");
    printf("It shoud return 1 : ");
    printf("returned %d\n", file);
    
    /**
     * @brief is_symlink UT
     */
    printf("\nDescribe: is_symlink\n");

    int link = is_symlink(fd, "tests.c");
    printf("It should return 0 : ");
    printf("returned %d\n", link);
    link = is_symlink(fd, "test_link");
    printf("It should return 1 : ");
    printf("returned %d\n", link);

    /**
     * @brief list UT
     */
    printf("\nDescribe: list\n");

    // Preparing resources
    size_t * no_entries = (size_t*) malloc(sizeof(size_t));
    *no_entries = (size_t) 4;
    char ** entries = (char **) malloc(*no_entries*sizeof(char*));
    for(int i=0; i<*no_entries; i++)
    {
        entries[i] = (char*) malloc(sizeof(char*));
    }

    int listed = list(fd, "test/", entries, no_entries);
    printf("List : [");
    for(int i=0; i<*no_entries; i++)
    {
        printf(" %s ", entries[i]);
    }
    printf("]\n");
    printf("Size : %zu\n", *no_entries);
    printf("It should return 1 : ");
    printf("returned %d\n", listed);

    listed = list(fd, "test/test2/", entries, no_entries);
    printf("List : [");
    for(int i=0; i<*no_entries; i++)
    {
        printf(" %s ", entries[i]);
    }
    printf("]\n");
    printf("Size : %zu\n", *no_entries);
    printf("It should return 1 : ");
    printf("returned %d\n", listed);

    listed = list(fd, "test_dir", entries, no_entries);
    printf("List : [");
    for(int i=0; i<*no_entries; i++)
    {
        printf(" %s ", entries[i]);
    }
    printf("]\n");
    printf("Size : %zu\n", *no_entries);
    printf("This should return 1 : ");
    printf("returned %d\n", listed);

    listed = list(fd, "tests.c", entries, no_entries);
    printf("It should return 0 : ");
    printf("returned %d\n", listed);

    listed = list(fd, "test_link", entries, no_entries);
    printf("It should return 0 : ");
    printf("returned %d\n", listed);

    // Freeing resources
    free(entries);
    free(no_entries);

    /**
     * @brief read_file
     */
    printf("\nDescribe: read_file\n");

    // Preparing resources
    size_t * len = (size_t*) malloc(sizeof(size_t));
    *len = 5;
    uint8_t * dest = (uint8_t*) malloc(*len*sizeof(uint8_t));

    int readed = read_file(fd, "test/test.txt", 3, dest, len);
    printf("Content readed should return 'st' : ");
    printf("'%s'\n", (char*) dest);
    printf("Bytes readed should return 5 : ");
    printf("%zu\n", *len);
    printf("It should return 0 : ");
    printf("returned %d\n", readed);

    readed = read_file(fd, "test_link", 2, dest, len);
    printf("Content readed should return 'st' : ");
    printf("'%s'\n", (char*) dest);
    printf("Bytes readed should return 5 : ");
    printf("%zu\n", *len);
    printf("It should return 0 : ");
    printf("returned %d\n", readed);


    *len = 3;
    readed = read_file(fd, "test/test.txt", 2, dest, len);
    printf("Content readed should return 'st' : ");
    printf("'%s'\n", (char*) dest);
    printf("Bytes readed should return 3 : ");
    printf("%zu\n", *len);
    printf("It should return 1 : ");
    printf("returned %d\n", readed);

    *len = 2;
    readed = read_file(fd, "test/test.txt", 0, dest, len);
    printf("Content readed should return 'te' : ");
    printf("'%s'\n", (char*) dest);
    printf("Bytes readed should return 2 : ");
    printf("%zu\n", *len);
    printf("It should return 0 : ");
    printf("returned %d\n", readed);

    *len = 56;
    readed = read_file(fd, "test/test.txt", 1, dest, len);
    printf("Content readed should return 'te' : ");
    printf("'%s'\n", (char*) dest);
    printf("Bytes readed should return 36 : ");
    printf("%zu\n", *len);
    printf("It should return 20 : ");
    printf("returned %d\n", readed);

    readed = read_file(fd, "test/", 2, dest, len);
    printf("It should return -1 : ");
    printf("returned %d\n", readed);

    readed = read_file(fd, "test_dir", 2, dest, len);
    printf("It should return -1 : ");
    printf("returned %d\n", readed);

    readed = read_file(fd, "test/test.txt", 56, dest, len);
    printf("It should return -2 : ");
    printf("returned %d\n", readed);

    readed = read_file(fd, "test_link", 56, dest, len);
    printf("It should return -2 : ");
    printf("returned %d\n", readed);

    // Freeing resources
    free(len);
    free(dest);

    return 0;
}