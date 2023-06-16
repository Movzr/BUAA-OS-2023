#include<lib.h>
int main(int argc, char **argv) {
    char c;
    int fd;
    if(argc != 1) {
        printf("wrong history order! \n");
        exit();
    }
    fd = open("/.history", O_RDONLY);
    if(fd < 0) {
        printf("error in opening history or there is no previous command \n");
        exit();
    }
    int r;
    int cnt = 1;
    int flag = 1;
    while((r = read(fd, &c, 1)) == 1) {
        if(flag) {
            printf("%-5d ", cnt);
            flag = 0;
        }
        printf("%c", c);
        if(c == '\n') {
            flag = 1;
            cnt++;
        } 
    }
}