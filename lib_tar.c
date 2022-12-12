#include "lib_tar.h"

#include <stdio.h>
#include <string.h>
#include <math.h>

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
    tar_header_t *head = malloc(sizeof(tar_header_t));
    lseek(tar_fd, 0, SEEK_SET);
    while (read(tar_fd, head, sizeof(tar_header_t)) > 0)
    {
        if (strcmp((char *)head, "\0"))
        {
            if (strcmp(head->magic, "ustar") != 0 && strcmp(head->magic, "\0") != 0)
                return -1;
            char version[3];
            version[2] = '\0';
            if (strcmp(strncpy(version, head->version, 2), "00") != 0)
                return -2;
            if (head->chksum < 0)
                return -3;
            int offset = strtol(head->size, NULL, 8);
            int size = sizeof(tar_header_t);
            lseek(tar_fd, offset % size ? (floor(offset / size) + 1) * size : offset, SEEK_CUR);
        }
    }
    return 0;
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
    if(exists(tar_fd, path) == 0) return 0;
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
    if(exists(tar_fd, path) == 0) return 0;
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
    if(exists(tar_fd, path) == 0) return 0;
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
int list(int tar_fd, char *path, char **entries, size_t *no_entries)
{
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
    return 0;
}