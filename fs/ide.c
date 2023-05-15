/*
 * operations on IDE disk.
 */

#include "serv.h"
#include <drivers/dev_disk.h>
#include <lib.h>
#include <mmu.h>

int excel[31];
int map[31];
int flush[31];
int allZero[512];

int alloc_pblock(){
	int min=99999999;
	int target=0;
	for(int i=0;i<32;i++){
		if(map[i]==0){
			if(flush[i]<min){
				min=flush[i];
				target=i;
			}
		}
	}
	if(min>=5){
		char buf[1024];
		int m=99999999;
		int t=0;
		for(int i=0;i<32;i++){
			if(map[i]==1){
				if(flush[i]<m){
					m=flush[i];
					t=i;
				}
			}
		}
		ide_read(0,t,(void *)buf,1);
		ide_write(0,target,(void *)buf,1);
		map[target]=1;
		int logic;
		for(int i=0;i<32;i++){
			if(excel[i]==t){
				logic=i;
				break;
			}
		}
		excel[logic]=target;
		ide_write(0,t,(void *)allZero,1);
		map[t]=0;
		flush[t]+=1;
		return t;
	}
	else{
		return target;
	}
}

void ssd_init(){
	for(int i=0;i<32;i++){
		excel[i]=-1;
		map[i]=0;
		flush[i]=0;
	}
	for(int i=0;i<512;i++){
		allZero[i]=0;
	}
}
int ssd_read(u_int logic_no, void *dst){
	int result=excel[logic_no];
	if(result<0){
		return -1;
	}
	ide_read(0,result,dst,1);
	return 0;
}

void ssd_write(u_int logic_no, void *src){
	int r = excel[logic_no];
        if(r<0) {
                r= alloc_pblock();
		excel[logic_no] = r;
		map[r]=1;
		ide_write(0,r,src,1);
        }else{
		ssd_erase(logic_no);
		r= alloc_pblock();
                excel[logic_no] = r;
                map[r]=1;
             	ide_write(0,r,src,1);
	}
}
void ssd_erase(u_int logic_no) {
	int r = excel[logic_no];
	if(r<0) {
		return ;
	}
	ide_write(0,r,(void *)allZero,1);
	map[r]=0;
	flush[r]+=1;
	excel[logic_no]=-1;
}
/*
int alloc_pblock(){
	int min=99999999;
	int target=0;
	for(int i=0;i<32;i++){
		if(map[i]==0){
			if(flush[i]<min){
				min=flush[i];
				target=i;
			}
		}
	}
	if(min>=5){
		char buf[1024];
		int m=99999999;
		int t=0;
		for(int i=0;i<32;i++){
			if(map[i]==1){
				if(flush[i]<m){
					m=flush[i];
					t=i;
				}
			}
		}
		ide_read(0,t,(void *)buf,1);
		ide_write(0,target,(void *)buf,1);
		map[target]=1;
		int logic;
		for(int i=0;i<32;i++){
			if(excel[i]==t){
				logic=i;
				break;
			}
		}
		excel[logic]=target;
		ide_write(0,t,(void *)allZero,1);
		map[t]=0;
		flush[t]+=1;
		return t;
	}
	else{
		return target;
	}
}
*/
// Overview:
//  read data from IDE disk. First issue a read request through
//  disk register and then copy data from disk buffer
//  (512 bytes, a sector) to destination array.
//
// Parameters:
//  diskno: disk number.
//  secno: start sector number.
//  dst: destination for data read from IDE disk.
//  nsecs: the number of sectors to read.
//
// Post-Condition:
//  Panic if any error occurs. (you may want to use 'panic_on')
//
// Hint: Use syscalls to access device registers and buffers.
// Hint: Use the physical address and offsets defined in 'include/drivers/dev_disk.h':
//  'DEV_DISK_ADDRESS', 'DEV_DISK_ID', 'DEV_DISK_OFFSET', 'DEV_DISK_OPERATION_READ',
//  'DEV_DISK_START_OPERATION', 'DEV_DISK_STATUS', 'DEV_DISK_BUFFER'
void ide_read(u_int diskno, u_int secno, void *dst, u_int nsecs) {
	u_int begin = secno * BY2SECT;
	u_int end = begin + nsecs * BY2SECT;

	for (u_int off = 0; begin + off < end; off += BY2SECT) {
		uint32_t temp = diskno;
		/* Exercise 5.3: Your code here. (1/2) */
		panic_on(syscall_write_dev((void *)&temp,DEV_DISK_ADDRESS+DEV_DISK_ID,4));
		temp=begin+off;
		panic_on(syscall_write_dev((void *)&temp,DEV_DISK_ADDRESS+DEV_DISK_OFFSET,4));
		temp=DEV_DISK_OPERATION_READ;
		panic_on(syscall_write_dev((void *)&temp,DEV_DISK_ADDRESS+DEV_DISK_START_OPERATION,4));
		int resultOfRead;
		panic_on(syscall_read_dev((void *)&resultOfRead,DEV_DISK_ADDRESS+DEV_DISK_STATUS,4));
		panic_on(resultOfRead==0);
		panic_on(syscall_read_dev((void *)dst+off,DEV_DISK_ADDRESS+DEV_DISK_BUFFER,DEV_DISK_BUFFER_LEN));

	}
}

// Overview:
//  write data to IDE disk.
//
// Parameters:
//  diskno: disk number.
//  secno: start sector number.
//  src: the source data to write into IDE disk.
//  nsecs: the number of sectors to write.
//
// Post-Condition:
//  Panic if any error occurs.
//
// Hint: Use syscalls to access device registers and buffers.
// Hint: Use the physical address and offsets defined in 'include/drivers/dev_disk.h':
//  'DEV_DISK_ADDRESS', 'DEV_DISK_ID', 'DEV_DISK_OFFSET', 'DEV_DISK_BUFFER',
//  'DEV_DISK_OPERATION_WRITE', 'DEV_DISK_START_OPERATION', 'DEV_DISK_STATUS'
void ide_write(u_int diskno, u_int secno, void *src, u_int nsecs) {
	u_int begin = secno * BY2SECT;
	u_int end = begin + nsecs * BY2SECT;

	for (u_int off = 0; begin + off < end; off += BY2SECT) {
		uint32_t temp = diskno;
		/* Exercise 5.3: Your code here. (2/2) */
		panic_on(syscall_write_dev((void *)src+off,DEV_DISK_ADDRESS+DEV_DISK_BUFFER,DEV_DISK_BUFFER_LEN));
		panic_on(syscall_write_dev((void *)&temp,DEV_DISK_ADDRESS+DEV_DISK_ID,4));
		temp=begin+off;
		panic_on(syscall_write_dev((void *)&temp,DEV_DISK_ADDRESS+DEV_DISK_OFFSET,4));
		temp=DEV_DISK_OPERATION_WRITE;
		panic_on(syscall_write_dev((void *)&temp,DEV_DISK_ADDRESS+DEV_DISK_START_OPERATION,4));
		int resultOfWrite;
		panic_on(syscall_read_dev((void *)&resultOfWrite,DEV_DISK_ADDRESS+DEV_DISK_STATUS,4));
		panic_on(resultOfWrite==0);
	}
}
