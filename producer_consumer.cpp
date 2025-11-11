#include<iostream>
#include<queue>
using namespace std;

class ProducerConsumer{
    public:

    queue <int> buffer;
    int s=1;//semaphore
    int e=10;//initially the buffer will be empty
    int f=0;

    void wait(int &s)
    {
        while(s<=0);
        s--;
    }

    void signal(int &s)
    {
        s++;
    }

    void producer()
    {
        int x=rand();//producer produces random data
        wait(s);
        wait(e);
        buffer.push(x);//producer now has access to share resource that is buffer 
        cout<<"Producer has produced "<<x<<" data"<<endl;
        signal(f);
        signal(s);
    }

    void consumer()
    {
        wait(s);
        wait(f);
        //now consumer has access to critical section so it will consume data from buffer
        int y=buffer.front();
        cout<<"Consumer has consumed "<<y<<" data from buffer"<<endl;
        buffer.pop();
        signal(e);
        signal(s);
    }

};

int main()
{
    ProducerConsumer p1;
    p1.producer();
    p1.producer();
    p1.consumer();
    return 0;
}