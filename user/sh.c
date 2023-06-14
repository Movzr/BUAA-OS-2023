#include <args.h>
#include <lib.h>

#define WHITESPACE " \t\r\n"
#define SYMBOLS "<|>&;()"

/* Overview:
 *   Parse the next token from the string at s.
 *
 * Post-Condition:
 *   Set '*p1' to the beginning of the token and '*p2' to just past the token.
 *   Return:
 *     - 0 if the end of string is reached.
 *     - '<' for < (stdin redirection).
 *     - '>' for > (stdout redirection).
 *     - '|' for | (pipe).
 *     - 'w' for a word (command, argument, or file name).
 *
 *   The buffer is modified to turn the spaces after words into zero bytes ('\0'), so that the
 *   returned token is a null-terminated string.
 */
int _gettoken(char *s, char **p1, char **p2) {
	*p1 = 0;
	*p2 = 0;
	if (s == 0) {
		return 0;
	}

	while (strchr(WHITESPACE, *s)) {
		*s++ = 0;
	}
	if (*s == 0) {
		return 0;
	}

	if (strchr(SYMBOLS, *s)) {
		int t = *s;
		*p1 = s;
		*s++ = 0;
		*p2 = s;
		return t;
	}

	if (*s == '"') {
		*p1 = ++s;
		while(*s && *s != '"') {
			s++;
		}
		*s++ = 0;
		*p2 = s;
		return 'w';
	}

	*p1 = s;
	while (*s && !strchr(WHITESPACE SYMBOLS, *s)) {
		s++;
	}
	*p2 = s;
	return 'w';
}

int gettoken(char *s, char **p1) {
	static int c, nc;
	static char *np1, *np2;

	if (s) {
		nc = _gettoken(s, &np1, &np2);
		return 0;
	}
	c = nc;
	*p1 = np1;
	nc = _gettoken(np2, &np1, &np2);
	return c;
}

#define MAXARGS 128

int parsecmd(char **argv, int *rightpipe) {
	int argc = 0;
	while (1) {
		char *t;
		int fd, r;
		int c = gettoken(0, &t);
		switch (c) {
		case 0:
			return argc;
		case 'w':
			if (argc >= MAXARGS) {
				debugf("too many arguments\n");
				exit();
			}
			argv[argc++] = t;
			break;
		case '<':
			if (gettoken(0, &t) != 'w') {
				debugf("syntax error: < not followed by word\n");
				exit();
			}
			// Open 't' for reading, dup it onto fd 0, and then close the original fd.
			/* Exercise 6.5: Your code here. (1/3) */
			fd = open(t,O_RDONLY);
			dup(fd,0);
			close(fd);
			//user_panic("< redirection not implemented");

			break;
		case '>':
			if (gettoken(0, &t) != 'w') {
				debugf("syntax error: > not followed by word\n");
				exit();
			}
			// Open 't' for writing, dup it onto fd 1, and then close the original fd.
			/* Exercise 6.5: Your code here. (2/3) */
			if((fd = open(t,O_WRONLY)) < 0) {
				if((r = create(t, FTYPE_REG)) < 0) {
					debugf("open and create failed in >");
					exit();
				}
			}
			fd = open(t,O_WRONLY);
			dup(fd,1);
			close(fd);
			//user_panic("> redirection not implemented");

			break;
		case '|':;
			/*
			 * First, allocate a pipe.
			 * Then fork, set '*rightpipe' to the returned child envid or zero.
			 * The child runs the right side of the pipe:
			 * - dup the read end of the pipe onto 0
			 * - close the read end of the pipe
			 * - close the write end of the pipe
			 * - and 'return parsecmd(argv, rightpipe)' again, to parse the rest of the
			 *   command line.
			 * The parent runs the left side of the pipe:
			 * - dup the write end of the pipe onto 1
			 * - close the write end of the pipe
			 * - close the read end of the pipe
			 * - and 'return argc', to execute the left of the pipeline.
			 */
			int p[2];
			/* Exercise 6.5: Your code here. (3/3) */
			pipe(p);
			*rightpipe = fork();
			if(*rightpipe < 0){
				user_panic("fork error in sh.c");
			}
			if(*rightpipe == 0){
				dup(p[0],0);
				close(p[0]);
				close(p[1]);
				return parsecmd(argv, rightpipe);
			}else{
				dup(p[1],1);
				close(p[1]);
				close(p[0]);
				return argc;
			}
			//user_panic("| not implemented");

			break;
		case ';':
			*rightpipe = fork();
			if(*rightpipe < 0){
				user_panic("fork error in sh.c");
			}
			if(*rightpipe == 0) {
				return argc;
			}else {
				wait(*rightpipe);
				return parsecmd(argv, rightpipe);
			}
			break;

		case '&':
			*rightpipe = fork();
			if(*rightpipe < 0){
				user_panic("fork error in sh.c");
			}
			if(*rightpipe == 0) {
				return argc;
			}else {
				return parsecmd(argv, rightpipe);
			}
			break;
		}
	}

	return argc;
}

static int cmdCnt;
static int cmdOffset[256];
void getHistoryCommand(int target, char *result) {
	int r, fd, len;
	char temp[4096];
	r = open(".history", O_RDONLY);
	if(r < 0) {
		printf("open history failed or have no history command\n");
		exit();
	}
	fd = r;
	if(target == 0) {
		r = read(fd, result, cmdOffset[target]);
		if(r < 0) {
			printf("read history failed!\n");
			exit();
		}
		result[cmdOffset[target] - 1] = 0;
	} else {
		if((r = read(fd, temp, cmdOffset[target - 1])) != cmdOffset[target - 1]) {
			printf("read history failed!\n");
			exit();
		}
		len = cmdOffset[target] - cmdOffset[target - 1];
		if((r = read(fd, result, len)) != len) {
			printf("read history failed!\n");
			exit();
		}
		result[len - 1] = 0;
	}
}

void savecmd(char *cmd) {
	int r;
	r = open(".history", O_WRONLY | O_APPEND);
	if(r < 0) {
		r = create(".history", FTYPE_REG);
		if(r < 0) {
			user_panic("create .history failed!");
		}
		r = open(".history", O_WRONLY | O_APPEND);
	}
	write(r, cmd, strlen(cmd));
	write(r, "\n", 1);
	if(cmdCnt == 0) {
		cmdOffset[cmdCnt++] = strlen(cmd) + 1; //with \n
	} else {
		cmdOffset[cmdCnt] = cmdOffset[cmdCnt-1] + strlen(cmd) + 1;
		cmdCnt++;
	}
	close(r);
}

void runcmd(char *s) {
	gettoken(s, 0);
	char *argv[MAXARGS];
	int rightpipe = 0;
	int argc = parsecmd(argv, &rightpipe);
	if (argc == 0) {
		return;
	}
	argv[argc] = 0;

	int child = spawn(argv[0], argv);
	close_all();
	if (child >= 0) {
		wait(child);
	} else {
		debugf("spawn %s: %d\n", argv[0], child);
	}
	if (rightpipe) {
		wait(rightpipe);
	}
	exit();
}

char after[1024];

int checkDirKey(){
	char temp1;
	char temp2;
	int r;
	if ((r = read(0, &temp1, 1)) != 1) {
		if (r < 0) {
			debugf("read error: %d\n", r);
		}
		exit();
	}
	if ((r = read(0, &temp2, 1)) != 1) {
		if (r < 0) {
			debugf("read error: %d\n", r);
		}
		exit();
	}
	if(temp1 == '[') {
		if(temp2 == 'A') return 1;
		if(temp2 == 'B') return 2;
		if(temp2 == 'D') return 3;
		if(temp2 == 'C') return 4;
	}
	return 0;
}

char currentCmd[1024];
void readline(char *buf, u_int n) {
	int r;
	int pos=0;		//当前光标位置的指针
	char c;
	int draftLen = 0;
	int rowNow = cmdCnt;
	while (pos < n) {
		if ((r = read(0, &c, 1)) != 1) {
			if (r < 0) {
				debugf("read error: %d\n", r);
			}
			exit();
		}
		if(c == '\b' || c == 0x7f) {
			if(pos != 0) {
				if(pos == draftLen) {
					buf[--draftLen] = 0;
					pos--;
					printf("\b \b");
				} else {
					strcpy(after, buf + pos);
					pos--;
					strcpy(buf + pos, after);
					buf[--draftLen] = 0;
					printf("\b");
					printf("\x1b[K");
					printf("%s", after);
					for(int i = 0; i < strlen(after); i++) {
						printf("\b");
					}
					memset(after,0,1024);
				}
			}
		} else if(c == 27) {
			switch (checkDirKey()) {
				case 1:
					printf("\x1b[1B");
					if(cmdCnt == 0) {
						break;
					} else {
						for(int i = 0; i < pos; i++) {
							printf("\b");
						}
						printf("\x1b[K");
						pos = 0;
						if(rowNow == cmdCnt) {
							strcpy(currentCmd, buf);
						}
						if(rowNow > 0) {
							rowNow--;
						}
						getHistoryCommand(rowNow, buf);
						printf("%s",buf);
						pos = strlen(buf);
						draftLen = strlen(buf);
						break;
					}
				case 2:
					if(rowNow == cmdCnt) {
						break;
					} else if(rowNow == cmdCnt - 1) {
						for(int i = 0; i < pos; i++) {
							printf("\b");
						}
						printf("\x1b[K");
						rowNow++;
						strcpy(buf, currentCmd);
						if(draftLen == 0) {
							printf(" \b");
						} else {
							printf("%s",buf);
							pos = strlen(buf);
							draftLen = strlen(buf);
						}
						break;
					} else {
						for(int i = 0; i < pos; i++) {
							printf("\b");
						}
						printf("\x1b[K");
						rowNow++;
						getHistoryCommand(rowNow, buf);
						printf("%s",buf);
						pos = strlen(buf);
						draftLen = strlen(buf);
						break;
					}
				case 3:
					if(pos > 0) {
						pos--;
					} else {
						printf("\x1b[1C");		//超出左移范围输出光标右移
					}
					break;
				case 4:
					if(pos < draftLen) {
						pos++;
					} else {
						printf("\x1b[1D");
					}
					break;
				default:
					break;
			}
		} else if(c == '\r' || c == '\n') {
			memset(after,0,1024);
			return ;
		} else {
			if(pos == draftLen) {
				buf[draftLen] = c;
				buf[++draftLen] = 0;
				pos++;
			} else {
				strcpy(after, buf + pos);
				buf[pos] = c;
				printf("\x1b[K");	//清除掉当前光标到末尾
				pos++;
				strcpy(buf + pos, after);
				buf[++draftLen] = 0;
				printf("%s",after);
				for(int i = 0; i < strlen(after); i++) {
					printf("\b");
				}
				memset(after,0,1024);
			}
		}
		if(draftLen > n) {
			break;
		}

	}
	debugf("line too long\n");
	while ((r = read(0, buf, 1)) == 1 && buf[0] != '\r' && buf[0] != '\n') {
		;
	}
	buf[0] = 0;
}

char buf[1024];

void usage(void) {
	debugf("usage: sh [-dix] [command-file]\n");
	exit();
}

int main(int argc, char **argv) {
	int r;
	int interactive = iscons(0);
	int echocmds = 0;
	debugf("\n:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::\n");
	debugf("::                                                         ::\n");
	debugf("::                     MOS Shell 2023                      ::\n");
	debugf("::                                                         ::\n");
	debugf(":::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::\n");
	ARGBEGIN {
	case 'i':
		interactive = 1;
		break;
	case 'x':
		echocmds = 1;
		break;
	default:
		usage();
	}
	ARGEND

	if (argc > 1) {
		usage();
	}
	if (argc == 1) {
		close(0);
		if ((r = open(argv[1], O_RDONLY)) < 0) {
			user_panic("open %s: %d", argv[1], r);
		}
		user_assert(r == 0);
	}
	for (;;) {
		if (interactive) {
			printf("\n$ ");
		}
		readline(buf, sizeof buf);
		savecmd(buf);
		if (buf[0] == '#') {
			continue;
		}
		if (echocmds) {
			printf("# %s\n", buf);
		}
		if ((r = fork()) < 0) {
			user_panic("fork: %d", r);
		}
		if (r == 0) {
			runcmd(buf);
			exit();
		} else {
			wait(r);
		}
	}
	return 0;
}
