Semaphore mutex=1; //保护waiting,eating,flag.
Semaphore block=0; 
int eating=0;
int waiting=0;
int flag=0;        //标记是否已经满了五个人
while(true){
    P(mutex);
    if(flag==1){
        waiting++;
        V(mutex);
        P(block);
    }else{
        eating+=1;
        if(eating==5){
            flag=1;
        }else{
            flag=0;
        }
        V(mutex);
    }
    //do some eating
    P(mutex);
    eating-=1;
    if(eating==0){
        int next=min(waiting,5);
        waiting-=n;
        eating+=n;
        if(eating==5){
            flag=1;
        }else{
            flag=0;
        }
        while(n--){
            V(block);
        }
    }
    V(mutex);

}