#include <lib.h>
#include <fs.h>
int main(int argc, char **argv) {
    if(argc != 2) {
        user_panic("wrong number of args");
    } else {
        int fd = open(argv[1], O_RDONLY);
        if(fd >= 0) {
            user_panic("directory has already existed!");
        } else {
            int r = create(argv[1], FTYPE_DIR);
            if(r < 0) {
                user_panic("create directory named %s failed", argv[1]);
            }
            return 0;
        }
    }
    
}