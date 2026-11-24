
#include <dirent.h>
#include <stdio.h>
#include<string.h>

int main(int argc, char* argv[]) {
    printf("argc = %d\n", argc);
    for (int i = 0; i < argc; i++) {
        printf("argv[%d] = %s\n", i, argv[i]);
    }
    // 打开当前目录
    DIR* dir = opendir(".");
    // 判断是否打开成功
    if (dir == NULL) {
        perror("opendir");
        return 1;
    }
    // 用于保存读取到的目录项
    struct dirent* entry;
    int show_all = 0;
    int show_long = 0;
    int recursive = 0;
    int sort_time = 0;
    int reverse = 0;
    int show_inode = 0;
    int show_blocks = 0;

    for (int i = 1; i < argc; i++) {
        if (argv[i][0] == '-') {
            for (int j = 1; argv[i][j] != '\0'; j++) {
                if (argv[i][j] == 'a') {
                    show_all = 1;
                } else if (argv[i][j] == 'l') {
                    show_long = 1;
                } else if (argv[i][j] == 'R') {
                    recursive = 1;
                } else if (argv[i][j] == 't') {
                    sort_time = 1;
                } else if (argv[i][j] == 'r') {
                    reverse = 1;
                } else if (argv[i][j] == 'i') {
                    show_inode = 1;
                } else if (argv[i][j] == 's') {
                    show_blocks = 1;
                } else {
                    printf("未知参数: -%c\n", argv[i][j]);
                }
            }
        }
    }
    printf("a = %d\n", show_all);
    printf("l = %d\n", show_long);
    printf("R = %d\n", recursive);
    printf("t = %d\n", sort_time);
    printf("r = %d\n", reverse);
    printf("i = %d\n", show_inode);
    printf("s = %d\n", show_blocks);
    // 不断读取目录中的内容
    while ((entry = readdir(dir)) != NULL) {
        if (show_all == 0) {
            if (entry->d_name[0] == '.') {
                continue;
            }
        }

        printf("%s\n", entry->d_name);
    }
    // 关闭目录
    closedir(dir);
    return 0;
}
