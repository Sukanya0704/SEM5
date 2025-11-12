#include<iostream>
#include<string>
#include<chrono>
#include<thread>
using namespace std;

void ntp_sync(double server_time, double client_send, double server_recv, double server_send, double client_recv){
    double offset = ((server_recv-client_send)+(server_send-client_recv))/2;
    double delay = (client_recv-client_send)-(server_send-server_recv);

    cout << "\n--- NTP Synchronization ---\n";
    cout << "Offset (θ): " << offset << " sec\n";
    cout << "Delay  (δ): " << delay  << " sec\n";
    cout << "Adjusted client time: " << server_time + offset << " sec\n";
}

class Lamport_Clock{
public:
    int time;
    Lamport_Clock(){
        time = 0;
    }

    void event(string name){
        time++;
        cout<<name<<"|Local event|Clock: "<<time<<endl;
    }

    void send(string name){
        time++;
        cout<<name<<"|Send Message|Clock: "<<time<<endl;
    }

    void receive_msg(int received_time, string name){
        time = max(time, received_time)+1;
        cout<<name<<"|Receive msg|Clock: "<<time<<endl;
    }
};

int main(){
    cout<<"NTP and Lamport Clock Synchronization"<<endl;
    double client_send = 1.0;
    double server_recv = 2.0;
    double server_send = 2.1;
    double client_recv = 3.1;
    double server_time = 5.0; 
    ntp_sync(server_time, client_send, server_recv, server_send, client_recv);

    cout << "\n--- Lamport Clock Synchronization ---\n";
    Lamport_Clock P1, P2;

    P1.event("P1");              
    P1.send("P1 -> P2");         
    int msg_time = P1.time;

    P2.receive_msg(msg_time, "P2");  
    P2.event("P2");             

    P1.event("P1");              
    cout << "\nFinal Clocks:\nP1: " << P1.time << "\nP2: " << P2.time << endl;
    return 0;
}