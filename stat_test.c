#include <stdio.h>
#include <sys/stat.h>

int main() {
    struct stat file_stat;

    if (lstat("shortcut", &file_stat) == -1) {
        perror("stat");
        return 1;
    }

    printf("inode: %ld\n", (long)file_stat.st_ino);
    printf("size: %ld bytes\n", (long)file_stat.st_size);
    printf("blocks: %ld\n", (long)file_stat.st_blocks);
    if (S_ISREG(file_stat.st_mode)) {
        printf("type: regular file\n");
    } else if (S_ISDIR(file_stat.st_mode)) {
        printf("type: directory\n");
    } else if (S_ISLNK(file_stat.st_mode)) {
        printf("type: symbolic link\n");
    }
    return 0;
}