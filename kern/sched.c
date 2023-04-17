#include <env.h>
#include <pmap.h>
#include <printk.h>

/* Overview:
 *   Implement a round-robin scheduling to select a runnable env and schedule it using 'env_run'.
 *
 * Post-Condition:
 *   If 'yield' is set (non-zero), 'curenv' should not be scheduled again unless it is the only
 *   runnable env.
 *
 * Hints:
 *   1. The variable 'count' used for counting slices should be defined as 'static'.
 *   2. Use variable 'env_sched_list', which contains and only contains all runnable envs.
 *   3. You shouldn't use any 'return' statement because this function is 'noreturn'.
 */
void schedule(int yield) {
	static int count = 0; // remaining time slices of current env
	struct Env *e = curenv;
	static int user_time[5];
	struct Env *i;
	int mark[5]={0,0,0,0,0};
	int target=0;
	int mintime=1000;
	TAILQ_FOREACH(i, &env_sched_list, env_sched_link){
		if(i->env_status==ENV_RUNNABLE){
			mark[i->env_user]+=1;
		}
	}
	if(yield||count==0||e==NULL||e->env_status!=ENV_RUNNABLE){
		if(e!=NULL&&e->env_status==ENV_RUNNABLE){
			TAILQ_REMOVE(&env_sched_list,e,env_sched_link);
			TAILQ_INSERT_TAIL(&env_sched_list,e,env_sched_link);
			user_time[e->env_user]+=e->env_pri;
		}
		panic_on(TAILQ_EMPTY(&env_sched_list));
		for(int j=0;j<5;j++){
			if(mark[j]>0&&(user_time[j]<mintime)){
				target=j;
				mintime=user_time[j];
			}
		}
		TAILQ_FOREACH(i, &env_sched_list, env_sched_link){
			if(i->env_user==target){
				e=i;
				break;
			}
		}
		count = e->env_pri;
	}
	count--;
	env_run(e);
}
