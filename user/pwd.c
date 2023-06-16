#include<lib.h>
int main(int argc, char **argv) {
    int r;
    char buf[1024];
    if(argc != 1) {
        printf("wrong pwd order!\n");
        exit();
    }
    getcwd(buf);
    printf("%s\n",buf);
    return 0;
}