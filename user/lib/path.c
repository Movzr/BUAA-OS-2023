#include<lib.h>

int chdir(u_int envid, char *buf) {
    return syscall_set_cur_path(envid, buf);
}

void getcwd(char *buf) {
    return syscall_get_cur_path(buf);
}

void link_paths(char *prePath, char *posPath) {
    int preLen = strlen(prePath);
    if(preLen != 1) {
        prePath[preLen++] = '/';
    }
    if(posPath[0] == '.' && posPath[1] == '/') {
        posPath += 2;               //跳过./
    }
    int poslen = strlen(posPath);
    for(int i = 0; i < poslen; i++) {
        prePath[preLen + i] = posPath[i];
    }
    prePath[preLen + poslen] = 0;
}