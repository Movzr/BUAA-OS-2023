Semaphore Smutex=1;
Semaphore Se_De=1;
Semaphore In_In=1;
Semaphore In_De=1;
Semaphore De_De=1;
int searchCnt=0;

Search:
    while(true){
        P(Smutex);
        if(search==0){
            P(Se_De);
        }
        searchCnt++;
        V(Smutex);
        search();
        P(Smutex);
        searchCnt--;
        if(search==0){
            V(Se_De);
        }
        V(Smutex);
    }

Insert:
    while(true){
        P(In_In);
        P(In_De);
        insert();
        P(In_De);
        V(In_In);
    }

Delete:
    while(true){
        P(Se_De);
        P(In_De);
        P(De_De);
        delete();
        V(De_De);
        V(In_De);
        V(Se_De);
    }