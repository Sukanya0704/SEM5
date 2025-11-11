#include <iostream>
#include <algorithm>
#include <chrono>
#include <thread>
using namespace std;

// ---------- NTP Clock Synchronization (Simplified) ----------
void ntp_sync(double server_time, double client_send, double server_recv, double server_send, double client_recv) {
    // Formula:
    // Offset θ = ((server_recv - client_send) + (server_send - client_recv)) / 2
    // Delay  δ = (client_recv - client_send) - (server_send - server_recv)
    double offset = ((server_recv - client_send) + (server_send - client_recv)) / 2.0;
    double delay  = (client_recv - client_send) - (server_send - server_recv);

    cout << "\n--- NTP Synchronization ---\n";
    cout << "Offset (θ): " << offset << " sec\n";
    cout << "Delay  (δ): " << delay  << " sec\n";
    cout << "Adjusted client time: " << server_time + offset << " sec\n";
}

// ---------- Lamport Clock Synchronization ----------
class LamportClock {
public:
    int time;
    LamportClock() { time = 0; }

    void event(string name) {
        time++;
        cout << name << " | Local event | Clock: " << time << endl;
    }

    void send(string name) {
        time++;
        cout << name << " | Send message | Clock: " << time << endl;
    }

    void receive(int received_time, string name) {
        time = max(time, received_time) + 1;
        cout << name << " | Receive message | Clock: " << time << endl;
    }
};

// ---------- Main ----------
int main() {
    cout << "============================\n";
    cout << "Simple NTP + Lamport Demo\n";
    cout << "============================\n";

    // --- NTP Example (times in seconds for demo) ---
    double client_send = 1.0;
    double server_recv = 2.0;
    double server_send = 2.1;
    double client_recv = 3.1;
    double server_time = 5.0; // hypothetical server clock
    ntp_sync(server_time, client_send, server_recv, server_send, client_recv);

    // --- Lamport Example ---
    cout << "\n--- Lamport Clock Synchronization ---\n";
    LamportClock P1, P2;

    P1.event("P1");              // event on P1
    P1.send("P1 -> P2");         // P1 sends message
    int msg_time = P1.time;

    P2.receive(msg_time, "P2");  // P2 receives message
    P2.event("P2");              // local event on P2

    P1.event("P1");              // another event on P1
    cout << "\nFinal Clocks:\nP1: " << P1.time << "\nP2: " << P2.time << endl;

    return 0;
}