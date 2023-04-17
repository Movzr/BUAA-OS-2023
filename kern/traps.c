#include <env.h>
#include <pmap.h>
#include <printk.h>
#include <trap.h>
#include <mmu.h>
extern void handle_int(void);
extern void handle_tlb(void);
extern void handle_sys(void);
extern void handle_mod(void);
extern void handle_reserved(void);
extern void handle_ov(void);

void (*exception_handlers[32])(void) = {
    [0 ... 31] = handle_reserved,
    [0] = handle_int,
    [2 ... 3] = handle_tlb,
    [12] = handle_ov,
#if !defined(LAB) || LAB >= 4
    [1] = handle_mod,
    [8] = handle_sys,
#endif
};

/* Overview:
 *   The fallback handler when an unknown exception code is encountered.
 *   'genex.S' wraps this function in 'handle_reserved'.
 */
void do_reserved(struct Trapframe *tf) {
	print_tf(tf);
	panic("Unknown ExcCode %2d", (tf->cp0_cause >> 2) & 0x1f);
}

void do_ov(struct Trapframe *tf) {
	u_long va;
	u_long pa;
 	Pte *pte;
	page_lookup(curenv->env_pgdir,tf->cp0_epc,&pte);
	pa=PTE_ADDR(*pte);
	va=KADDR(pa);
	u_long instr=*((u_int *)va);
	printk("0x%08x\n",instr);
	if(((instr&0x20)==0x20)&&((instr>>26)==0)){
		*((u_long *)va)=instr+1;
		curenv->env_ov_cnt++;
		printk("add ov handled\n");
		return;
	}
	if(((instr&0x22)==0x22)&&((instr>>26)==0)){
		*((u_long *)va)=instr+1;
		curenv->env_ov_cnt++;
		printk("sub ov handled\n");
		return;
	}
	if((instr>>26)==0x8){
		int t=(int)((instr>>16)&0x1f);
		int s=(int)((instr>>21)&0x1f);
		u_long imm=instr&0xffff;
		tf->regs[t]=(tf->regs[s]>>1)+(imm>>1);
		tf->cp0_epc+=4;
		curenv->env_ov_cnt++;
		printk("addi ov handled\n");
		return ;
	}
}	

