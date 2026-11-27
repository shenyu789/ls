
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <locale.h>
#include <stdlib.h>

struct FileInfo {
    char name[256];
    struct stat file_stat;
};
int compare_name(const void* a, const void* b) {
    const struct FileInfo* file_a = a;
    const struct FileInfo* file_b = b;

    return strcmp(file_a->name, file_b->name);
}
int compare_time(const void* a, const void* b) {
    const struct FileInfo* file_a = a;
    const struct FileInfo* file_b = b;

    if (file_a->file_stat.st_mtime > file_b->file_stat.st_mtime) {
        return -1;
    } else if (file_a->file_stat.st_mtime < file_b->file_stat.st_mtime) {
        return 1;
    } else {
        return 0;
    }
}
void list_directory(const char* path,
                    int show_all,
                    int show_long,
                    int recursive,
                    int sort_time,
                    int reverse,
                    int show_inode,
                    int show_blocks);
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
    setlocale(LC_TIME, "");

    printf("argc = %d\n", argc);

    for (int i = 0; i < argc; i++) {
        printf("argv[%d] = %s\n", i, argv[i]);
    }

    int show_all = 0;
    int show_long = 0;
    int recursive = 0;
    int sort_time = 0;
    int reverse = 0;
    int show_inode = 0;
    int show_blocks = 0;

    // 参数解析
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

    // 调用目录处理函数
    list_directory(".", show_all, show_long, recursive, sort_time, reverse,
                   show_inode, show_blocks);

    return 0;
}
void list_directory(const char* path,
                    int show_all,
                    int show_long,
                    int recursive,
                    int sort_time,
                    int reverse,
                    int show_inode,
                    int show_blocks) {
    // 第一阶段：打开指定目录
    DIR* dir = opendir(path);

    if (dir == NULL) {
        perror(path);
        return;
    }

    struct FileInfo files[1024];
    int count = 0;
    struct dirent* entry;

    // 第二阶段：收集目录中的文件
    while ((entry = readdir(dir)) != NULL) {
        if (show_all == 0 && entry->d_name[0] == '.') {
            continue;
        }

        if (count >= 1024) {
            printf("目录项数量超过数组容量！\n");
            break;
        }

        // 拼接完整路径
        char full_path[1024];

        snprintf(full_path, sizeof(full_path), "%s/%s", path, entry->d_name);

        // 获取文件属性
        struct stat file_stat;

        if (lstat(full_path, &file_stat) == -1) {
            perror(full_path);
            continue;
        }

        // 保存文件名和属性
        strcpy(files[count].name, entry->d_name);
        files[count].file_stat = file_stat;

        count++;
    }

    printf("一共读取了 %d 个文件\n", count);

    // 第三阶段：排序
    if (sort_time == 1) {
        qsort(files, count, sizeof(struct FileInfo), compare_time);
    } else {
        qsort(files, count, sizeof(struct FileInfo), compare_name);
    }
    // 第四阶段：打印
    if (recursive == 1) {
        printf("%s:\n", path);
    }
    for (int n = 0; n < count; n++) {
        // 根据 reverse 决定实际访问哪个下标
        int i;

        if (reverse == 1) {
            i = count - 1 - n;
        } else {
            i = n;
        }

        struct stat file_stat = files[i].file_stat;

        // -i：显示 inode
        if (show_inode == 1) {
            printf("%lu ", (unsigned long)file_stat.st_ino);
        }

        // -s：显示磁盘块数量
        if (show_blocks == 1) {
            printf("%lld ", (long long)((file_stat.st_blocks + 1) / 2));
        }

        // -l：详细输出
        if (show_long == 1) {
            char permissions[10];
            char time_str[64];

            char type = get_file_type(file_stat.st_mode);
            get_permissions(file_stat.st_mode, permissions);

            struct passwd* user = getpwuid(file_stat.st_uid);
            struct group* group = getgrgid(file_stat.st_gid);

            const char* username = user ? user->pw_name : "unknown";
            const char* groupname = group ? group->gr_name : "unknown";

            struct tm* time_info = localtime(&file_stat.st_mtime);

            if (time_info == NULL) {
                strcpy(time_str, "unknown");
            } else {
                strftime(time_str, sizeof(time_str), "%b %d %H:%M", time_info);
            }

            printf("%c%s %lu %s %s %lld %s %s\n", type, permissions,
                   (unsigned long)file_stat.st_nlink, username, groupname,
                   (long long)file_stat.st_size, time_str, files[i].name);

        } else {
            printf("%s\n", files[i].name);
        }
    }
    // 第五阶段：递归遍历子目录
    if (recursive == 1) {
        for (int i = 0; i < count; i++) {
            // 只处理真正的目录
            if (S_ISDIR(files[i].file_stat.st_mode) &&
                strcmp(files[i].name, ".") != 0 &&
                strcmp(files[i].name, "..") != 0) {
                // 拼接子目录路径
                char child_path[1024];

                snprintf(child_path, sizeof(child_path), "%s/%s", path,
                         files[i].name);

                printf("\n");

                // 递归调用
                list_directory(child_path, show_all, show_long, recursive,
                               sort_time, reverse, show_inode, show_blocks);
            }
        }
    }
    // 关闭目录
    closedir(dir);
}