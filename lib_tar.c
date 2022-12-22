#include "lib_tar.h"

#define PATH_SIZE 1000 

/**
 * Private method for devlopment purpose
 * Prints the header
 */
void print_header(tar_header_t *head)
{
    printf("Printing header ...\n");
    printf("Name: %s\nMode: %s\nUID: %s\nGID: %s\nSize: %s\nMtime: %s\nChksum: %s\nTypeflag: %c\nLinkname: %s\nMagic: %s\nVersion: %s\nUname: %s\nGname: %s\nDevMajor: %s\nDevMinor: %s\nPrefix: %s\nPadding: %s\n", head->name, head->mode, head->uid, head->gid, head->size, head->mtime, head->chksum, head->typeflag, head->linkname, head->magic, head->version, head->uname, head->gname, head->devmajor, head->devminor, head->prefix, head->padding);
    printf("Ending printing header ...\n\n");
}

/**
 * Checks whether the archive is valid.
 *
 * Each non-null header of a valid archive has:
 *  - a magic value of "ustar" and a null,
 *  - a version value of "00" and no null,
 *  - a correct checksum
 *
 * @param tar_fd A file descriptor pointing to the start of a file supposed to contain a tar archive.
 *
 * @return a zero or positive value if the archive is valid, representing the number of non-null headers in the archive,
 *         -1 if the archive contains a header with an invalid magic value,
 *         -2 if the archive contains a header with an invalid version value,
 *         -3 if the archive contains a header with an invalid checksum value
 */
int check_archive(int tar_fd)
{
    int to_return = 0;
    tar_header_t *head = malloc(sizeof(tar_header_t));
    // Set head at start of tar file
    lseek(tar_fd, 0, SEEK_SET);
    while (read(tar_fd, head, sizeof(tar_header_t)) > 0)
    {
        // Check if header not null
        if (strcmp((char *)head, "\0"))
        {
            char *temp = (char *)head;
            long counter = 0;
            for (int i = 0; i < sizeof(tar_header_t); i++)
            {
                if (i >= 148 && i < 156)
                    counter += 32;
                else
                    counter += temp[i];
            }
            if (strcmp(head->magic, "ustar") != 0 && strcmp(head->magic, "\0") != 0)
                return -1;
            char version[3];
            version[2] = '\0';
            if (strcmp(strncpy(version, head->version, 2), "00") != 0)
                return -2;
            if (strtol(head->chksum, NULL, 8) != counter)
                return -3;
            to_return++;
            int offset = strtol(head->size, NULL, 8);
            int size = sizeof(tar_header_t);
            lseek(tar_fd, offset % size ? (floor(offset / size) + 1) * size : offset, SEEK_CUR);
        }
    }
    return to_return;
}

/**
 * Checks whether an entry exists in the archive.
 *
 * @param tar_fd A file descriptor pointing to the start of a valid tar archive file.
 * @param path A path to an entry in the archive.
 *
 * @return zero if no entry at the given path exists in the archive,
 *         any other value otherwise.
 */
int exists(int tar_fd, char *path)
{
    tar_header_t *head = malloc(sizeof(tar_header_t));
    lseek(tar_fd, 0, SEEK_SET);
    while (read(tar_fd, head, sizeof(tar_header_t)) > 0)
    {
        if (strcmp((char *)head, "\0"))
        {
            if (strcmp(head->name, path) == 0)
                return 1;
            int offset = strtol(head->size, NULL, 8);
            int size = sizeof(tar_header_t);
            lseek(tar_fd, offset % size ? (floor(offset / size) + 1) * size : offset, SEEK_CUR);
        }
    }
    return 0;
}

/**
 * Checks whether an entry exists in the archive and is a directory.
 *
 * @param tar_fd A file descriptor pointing to the start of a valid tar archive file.
 * @param path A path to an entry in the archive.
 *
 * @return zero if no entry at the given path exists in the archive or the entry is not a directory,
 *         any other value otherwise.
 */
int is_dir(int tar_fd, char *path)
{
    if (exists(tar_fd, path) == 0)
        return 0;
    tar_header_t *head = malloc(sizeof(tar_header_t));
    lseek(tar_fd, 0, SEEK_SET);
    while (read(tar_fd, head, sizeof(tar_header_t)) > 0)
    {
        if (strcmp((char *)head, "\0"))
        {
            if (strcmp(head->name, path) == 0)
            {
                return head->typeflag == '5';
            }
            int offset = strtol(head->size, NULL, 8);
            int size = sizeof(tar_header_t);
            lseek(tar_fd, offset % size ? (floor(offset / size) + 1) * size : offset, SEEK_CUR);
        }
    }
    return 0;
}

/**
 * Checks whether an entry exists in the archive and is a file.
 *
 * @param tar_fd A file descriptor pointing to the start of a valid tar archive file.
 * @param path A path to an entry in the archive.
 *
 * @return zero if no entry at the given path exists in the archive or the entry is not a file,
 *         any other value otherwise.
 */
int is_file(int tar_fd, char *path)
{
    if (exists(tar_fd, path) == 0)
        return 0;
    tar_header_t *head = malloc(sizeof(tar_header_t));
    lseek(tar_fd, 0, SEEK_SET);
    while (read(tar_fd, head, sizeof(tar_header_t)) > 0)
    {
        if (strcmp((char *)head, "\0"))
        {
            if (strcmp(head->name, path) == 0)
            {
                return head->typeflag == '0' || head->typeflag == '\0';
            }
            int offset = strtol(head->size, NULL, 8);
            int size = sizeof(tar_header_t);
            lseek(tar_fd, offset % size ? (floor(offset / size) + 1) * size : offset, SEEK_CUR);
        }
    }
    return 0;
}

/**
 * Checks whether an entry exists in the archive and is a symlink.
 *
 * @param tar_fd A file descriptor pointing to the start of a valid tar archive file.
 * @param path A path to an entry in the archive.
 * @return zero if no entry at the given path exists in the archive or the entry is not symlink,
 *         any other value otherwise.
 */
int is_symlink(int tar_fd, char *path)
{
    if (exists(tar_fd, path) == 0)
        return 0;
    tar_header_t *head = malloc(sizeof(tar_header_t));
    lseek(tar_fd, 0, SEEK_SET);
    while (read(tar_fd, head, sizeof(tar_header_t)) > 0)
    {
        if (strcmp((char *)head, "\0"))
        {
            if (strcmp(head->name, path) == 0)
            {
                return head->typeflag == '2';
            }
            int offset = strtol(head->size, NULL, 8);
            int size = sizeof(tar_header_t);
            lseek(tar_fd, offset % size ? (floor(offset / size) + 1) * size : offset, SEEK_CUR);
        }
    }
    return 0;
}

/**
 * Lists the entries at a given path in the archive.
 * list() does not recurse into the directories listed at the given path.
 *
 * Example:
 *  dir/          list(..., "dir/", ...) lists "dir/a", "dir/b", "dir/c/" and "dir/e/"
 *   ├── a
 *   ├── b
 *   ├── c/
 *   │   └── d
 *   └── e/
 *
 * @param tar_fd A file descriptor pointing to the start of a valid tar archive file.
 * @param path A path to an entry in the archive. If the entry is a symlink, it must be resolved to its linked-to entry.
 * @param entries An array of char arrays, each one is long enough to contain a tar entry path.
 * @param no_entries An in-out argument.
 *                   The caller set it to the number of entries in `entries`.
 *                   The callee set it to the number of entries listed.
 *
 * @return zero if no directory at the given path exists in the archive,
 *         any other value otherwise.
 */
int nombre_d_occurence(char * string, char character)
{
    int counter = 0;
    for(int i=0; i<strlen(string); i++)
    {
        if(string[i] == character) counter++;
    }
    return counter;
}

int list(int tar_fd, char *path, char **entries, size_t *no_entries)
{
    if (is_dir(tar_fd, path) == 0 && is_symlink(tar_fd, path) == 0)
        return 0;
    tar_header_t *head = malloc(sizeof(tar_header_t));
    lseek(tar_fd, 0, SEEK_SET);
    char * directory = malloc(sizeof(char)*PATH_SIZE);
    strcpy(directory, path);
    int sub_directory = nombre_d_occurence(directory, '/');
    while (read(tar_fd, head, sizeof(tar_header_t)) > 0)
    {
        if (strcmp((char *)head, "\0"))
        {
            if (strcmp(head->name, directory) == 0 && is_symlink(tar_fd, head->name)) {
                int length = 0;
                if(nombre_d_occurence(head->linkname, '/') == 0) {
                    length = strlen(head->linkname)+1;
                } else {
                    length = strlen(head->linkname);
                }
                char new_directory[length];
                strcpy(new_directory, head->linkname);
                if(nombre_d_occurence(head->linkname, '/') == 0) {
                    new_directory[length-1] = '/';
                }
                new_directory[length] = '\0';
                if(is_dir(tar_fd, new_directory) == 0 && is_symlink(tar_fd, head->linkname) == 0) 
                    return 0;
                strcpy(directory, new_directory);
                sub_directory = nombre_d_occurence(directory, '/');
                lseek(tar_fd, 0, SEEK_SET);
            } else {
                if (strcmp(head->name, directory) == 0)
                {
                    int counter = 0;
                    // Checks if no_entries not exceeded, if characters can still be read and if we are still in the right folder
                    while (counter < *no_entries && read(tar_fd, head, sizeof(tar_header_t)) > 0)
                    {
                        // Checks if in the good [sub]directory
                        if((nombre_d_occurence(head->name, '/') == sub_directory || (nombre_d_occurence(head->name, '/') == sub_directory+1 && is_dir(tar_fd, head->name))) && strstr(head->name, directory) != NULL && strcmp(directory, head->name) != 0)
                        {
                            strcpy(*(entries + counter), head->name);
                            counter++;
                        }
                        int offset = strtol(head->size, NULL, 8);
                        int size = sizeof(tar_header_t);
                        lseek(tar_fd, offset % size ? (floor(offset / size) + 1) * size : offset, SEEK_CUR);
                    }
                    *no_entries = counter;
                    return 1;
                }
                int offset = strtol(head->size, NULL, 8);
                int size = sizeof(tar_header_t);
                lseek(tar_fd, offset % size ? (floor(offset / size) + 1) * size : offset, SEEK_CUR);
            }
        }
    }
    return 0;
}

/**
 * Reads a file at a given path in the archive.
 *
 * @param tar_fd A file descriptor pointing to the start of a valid tar archive file.
 * @param path A path to an entry in the archive to read from.  If the entry is a symlink, it must be resolved to its linked-to entry.
 * @param offset An offset in the file from which to start reading from, zero indicates the start of the file.
 * @param dest A destination buffer to read the given file into.
 * @param len An in-out argument.
 *            The caller set it to the size of dest.
 *            The callee set it to the number of bytes written to dest.
 *
 * @return -1 if no entry at the given path exists in the archive or the entry is not a file,
 *         -2 if the offset is outside the file total length,
 *         zero if the file was read in its entirety into the destination buffer,
 *         a positive value if the file was partially read, representing the remaining bytes left to be read to reach
 *         the end of the file.
 *
 */
ssize_t read_file(int tar_fd, char *path, size_t offset, uint8_t *dest, size_t *len)
{
    // Checks if the given file exists and if it's a file or a symlink
    if (is_file(tar_fd, path) == 0 && is_symlink(tar_fd, path) == 0)
        return -1;
    tar_header_t *head = malloc(sizeof(tar_header_t));
    lseek(tar_fd, 0, SEEK_SET);
    char * file = malloc(sizeof(char)*PATH_SIZE);
    strcpy(file, path);
    while (read(tar_fd, head, sizeof(tar_header_t)) > 0)
    {
        // Checks if header not null
        if (strcmp((char *)head, "\0"))
        {
            // Checks if head name correspond to given file and if it's a symlink
            if (strcmp(head->name, file) == 0 && is_symlink(tar_fd, head->name)) {
                // Checks if real file is a file or a symlink
                if(is_file(tar_fd, head->linkname)==0 && is_symlink(tar_fd, head->linkname) == 0)
                    return -1;
                strcpy(file, head->linkname);
                lseek(tar_fd, 0, SEEK_SET); // Get back to the beginning of the tar file to check with the real file this time
            } else {
                // Checks if head name correspond to given file
                if (strcmp(head->name, file) == 0) {
                    int file_size = strtol(head->size, NULL, 8);
                    if(offset > file_size) return -2;
                    size_t readable = *len > file_size ? file_size - offset : *len;
                    lseek(tar_fd, offset, SEEK_CUR);
                    *len = read(tar_fd, dest, readable);
                    if(file_size > *len) return file_size - *len - offset;
                    return 0;
                }
                int offset_heer = strtol(head->size, NULL, 8);
                int size = sizeof(tar_header_t);
                lseek(tar_fd, offset_heer % size ? (floor(offset_heer / size) + 1) * size : offset_heer, SEEK_CUR);
            }
        }
    }
    return 0;
}