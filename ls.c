
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>

void get_permissions(mode_t mode, char permissions[]) {
    permissions[0] = (mode & S_IRUSR) ? 'r' : '-';
    permissions[1] = (mode & S_IWUSR) ? 'w' : '-';
    permissions[2] = (mode & S_IXUSR) ? 'x' : '-';

    permissions[3] = (mode & S_IRGRP) ? 'r' : '-';
    permissions[4] = (mode & S_IWGRP) ? 'w' : '-';
    permissions[5] = (mode & S_IXGRP) ? 'x' : '-';

    permissions[6] = (mode & S_IROTH) ? 'r' : '-';
    permissions[7] = (mode & S_IWOTH) ? 'w' : '-';
    permissions[8] = (mode & S_IXOTH) ? 'x' : '-';

    permissions[9] = '\0';
}
char get_file_type(mode_t mode) {
    if (S_ISREG(mode)) {
        return '-';
    } else if (S_ISDIR(mode)) {
        return 'd';
    } else if (S_ISLNK(mode)) {
        return 'l';
    }
    return '?';
}
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
        // 第一层：判断是否显示隐藏文件
        if (show_all == 0 && entry->d_name[0] == '.') {
            continue;
        }

        // 第二层：判断是否使用详细输出
        if (show_long == 0) {
            printf("%s\n", entry->d_name);
        } else {
            struct stat file_stat;
            char permissions[10];

            if (lstat(entry->d_name, &file_stat) == -1) {
                perror(entry->d_name);
                printf("%s\n", entry->d_name);
                continue;
            }
            char type = get_file_type(file_stat.st_mode);
            get_permissions(file_stat.st_mode, permissions);

            printf("%c%s %lu %s\n", type, permissions,
                   (unsigned long)file_stat.st_nlink, entry->d_name);
            struct passwd* user = getpwuid(file_stat.st_uid);
            struct group* group = getgrgid(file_stat.st_gid);

            if (user != NULL && group != NULL) {
                printf("用户名：%s\n", user->pw_name);
                printf("组名：%s\n", group->gr_name);
            }
        }
    }
    // 关闭目录
    closedir(dir);
    return 0;
}
