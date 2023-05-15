// User-level IPC library routines

#include <env.h>
#include <lib.h>
#include <mmu.h>

// Send val to whom.  This function keeps trying until
// it succeeds.  It should panic() on any error other than
// -E_IPC_NOT_RECV.
//
// Hint: use syscall_yield() to be CPU-friendly.
void ipc_send(u_int whom, u_int val, const void *srcva, u_int perm) {
	int r;
	while ((r = syscall_ipc_try_send(whom, val, srcva, perm)) == -E_IPC_NOT_RECV) {
		syscall_yield();
	}
	user_assert(r == 0);
}

// Receive a value.  Return the value and store the caller's envid
// in *whom.
//
// Hint: use env to discover the value and who sent it.
u_int ipc_recv(u_int *whom, void *dstva, u_int *perm) {
	int r = syscall_ipc_recv(dstva);
	if (r != 0) {
		user_panic("syscall_ipc_recv err: %d", r);
	}

	if (whom) {
		*whom = env->env_ipc_from;
	}

	if (perm) {
		*perm = env->env_ipc_perm;
	}

	return env->env_ipc_value;
}

u_int get_time(u_int *us) {
	u_int temp = 1;
	panic_on(syscall_write_dev((void *)&temp,0x15000000,4));
	u_int s;
	u_int result;
	panic_on(syscall_read_dev((void *)&s,0x15000000+0x0010,4));
	panic_on(syscall_read_dev((void *)&result,0x15000000+0x0020,4));
	*us = result;
	return s;
}

void usleep(u_int us) {
	u_int inUs;
	u_int inS;
	inS = get_time(&inUs);
	while(1){
		u_int nowS;
		u_int nowUs;
		nowS = get_time(&nowUs);
		long long result= (long long)us + (long long)inS * (long long) 1000 + (long long) inUs;
		long long check = (long long) nowS * (long long) 1000+ (long long) nowUs;
		if(check >= result){
			return ;
		}else{
			syscall_yield();
		}
	}
}
