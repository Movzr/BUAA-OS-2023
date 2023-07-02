Semaphore mutex=1;        //对缓冲区的访问限制
Semaphore empty=N;        //缓冲区的个数限制
Semaphore odd=0;          //已经放入的奇数个数
Semaphore even=0;         //已经放入的偶数个数

P1:
    while(true){
        int number=produce();
        P(empty);
        P(mutex);
        put();
        P(mutex);
        if(number&1){
            V(odd);
        }else{
            V(even);
        }
    }

P2:
    while(true){
        P(odd);
        P(mutex);
        getodd();
        V(mutex);
        V(empty);
        countodd();
    }

P3:
    while(true){
        P(even);
        P(mutex);
        geteven();
        V(mutex);
        V(empty);
        counteven();
    }