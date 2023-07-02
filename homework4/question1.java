Semaphore RWBlock;
Semaphore Wpri;
Semaphore ReadBlock;
Semaphore Rmutex;
Semaphore Wmutex;
int readnum=0;
int writenum=0;
Read:
    while(true){
        P(Wpri);
        P(Rmutex);
        if(readnum==0){
            P(ReadBlock);
            P(RWBlock);
        }
        readnum++;
        V(Rmutex);
        V(Wpri);
        read();
        P(Rmutex);
        readnum--;
        if(readnum==0){
            V(ReadBlock);
            V(RWBlock);
        }
        V(Rmutex);
    }

Writer:
    while(true){
       P(Wmutex);
       if(writenum==0){
            P(Wpri);
            P(ReadBlock);
       }
       writenum++;
       V(Wmutex);
       P(RWBlock);
       write();
       V(RWBlock);
       P(Wmutex);
       writenum--;
       if(writenum==0){
            V(ReadBlock);
       }
       V(Wmutex);
       V(Wpri); 
    }