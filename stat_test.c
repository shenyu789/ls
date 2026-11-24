#include <stdio.h>
#include <sys/stat.h>

int main() {
    struct stat file_stat;

    if (lstat("ls.c", &file_stat) == -1) {
        perror("stat");
        return 1;
    }

    printf("inode: %ld\n", (long)file_stat.st_ino);
    printf("size: %ld bytes\n", (long)file_stat.st_size);
    printf("blocks: %ld\n", (long)file_stat.st_blocks);

    return 0;
}