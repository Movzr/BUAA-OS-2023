#include <lib.h>
int dirCnt;
int fileCnt;
void tree(char *path) {
    int r;
    struct Stat st;
    if ((r = stat(path, &st)) < 0) {
		user_panic("get File status error %s: %d", path, r);
	}
    if(!st.st_isdir) {
        printf("you can't tree a file! %s [error opening dir]", path);
        printf("0 directories, 0 files");
        exit();
    }
    printf("%s\n", path);
    treeDir(path, 0);
    printf("\n");
    printf("%d directories, %d files\n", dirCnt, fileCnt);
}

void treeDir(char *path, int depth) {
    int fd, n;
    struct File f;
    if ((fd = open(path, O_RDONLY)) < 0) {
		user_panic("open %s: %d", path, fd);
	}
    while ((n = readn(fd, &f, sizeof f)) == sizeof f) {
		if (f.f_name[0]) {
			tree1(path, f.f_type == FTYPE_DIR, f.f_name, depth + 1);
		}
	}
}

void tree1(char *path, int isDir, char *name, int depth) {
    if(depth >= 2) {
        for(int i = 0; i < depth - 1; i++) {
            printf("  ");
        }
    }
    printf("|-- ");
    printf("%s\n", name);
    if(isDir) {
        dirCnt += 1;
        char newPath[MAXPATHLEN];
        int oldPathLen = strlen(path);
        strcpy(newPath, path);
        if(path[oldPathLen-1] != '/') {
            newPath[oldPathLen] = '/';
            strcpy(newPath + oldPathLen + 1, name);
        } else {
            strcpy(newPath + oldPathLen, name);
        }
        treeDir(newPath,depth);
    } else {
        fileCnt += 1;
    }
}

int main(int argc, char **argv) {
    if(argc > 2) {
        printf("wrong tree order!");
        exit();
    } else {
        if(argc == 1) {
            tree("./");
        } else {
            tree(argv[1]);
        }
    }
    return 0;
}