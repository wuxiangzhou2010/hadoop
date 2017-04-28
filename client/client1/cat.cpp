#include <stdio.h>
#include <stdlib.h>
#include <sys/dir.h>
#include <sys/stat.h>
#include <string.h>
#include <unistd.h>

// 判断是否为目录

int IS_DIR(const char * path)
{
    struct stat st;

    lstat(path, &st);
    return S_ISDIR(st.st_mode);
}

// 遍历文件夹de递归函数
void List_Files_Core(const char * path, int recursive)
{
    DIR * pdir;
    struct dirent * pdirent;
    char temp[256], COMMAND[5000], NUM[3];

    pdir = opendir(path);

    if (pdir) {
        int i = 1;

        while ((pdirent = readdir(pdir)) ) {
            // 跳过"."和".."
            if (strcmp(pdirent->d_name, ".") == 0 || strcmp(pdirent->d_name, "..") == 0)
                continue;

            sprintf(temp, "%s/%s", path, pdirent->d_name);

            // 当temp为目录并且recursive为1的时候递归处理子目录
            if (IS_DIR(temp) && recursive) {
                List_Files_Core(temp, recursive);
            } else {
                int len = strlen(temp);

                if (temp[len - 2] == '+' && temp[len - 1] == '1') {
                    strcpy(COMMAND, "cat ");
                    int j     = 1;
                    bool flag = true;

                    while (flag) {
                        sprintf(NUM, "%d", j);
                        temp[len - 1] = '\0';
                        strcat(temp, NUM);

                        if (access(temp, F_OK) ) {
                            flag = false;
                        } else {
                            strcat(COMMAND, " ");
                            strcat(COMMAND, temp);
                            j++;
                        }

                        // printf("%s__%d\n",temp,access(temp,F_OK));
                    }

                    if (j > 2) {
                        strcat(COMMAND, " >> ");
                        temp[len - 2] = '\0';
                        strcat(COMMAND, temp);
                        printf("正在拼接%s\n", COMMAND);
                        system(COMMAND);
                    }
                }
            }
        }
    } else {
        printf("opendir error:%s\n", path);
    }

    closedir(pdir);
} // List_Files_Core

// 遍历文件夹的驱动函数

void List_Files(const char * path, int recursive)
{
    int len;
    char temp[256];

    // 去掉末尾的'/'
    len = strlen(path);
    strcpy(temp, path);

    if (temp[len - 1] == '/')
        temp[len - 1] = '\0';

    if (IS_DIR(temp) ) {
        // 处理目录
        List_Files_Core(temp, recursive);
    }
}

int main(int argc, char ** argv)
{
    if (argc != 2) {
        printf("Usage: ./program absolutePath\n");
        exit(0);
    }

    List_Files(argv[1], 1);
    return 0;
}
