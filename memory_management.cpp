#include<iostream>
#include<string>
#include<vector>
using namespace std;

class MemoryAllocator{
private:
    vector<int> BlockSizes;
    vector<int> ProcessSizes;
    int nb;
    int np;
    vector<int> allocations;
    vector<int> tempBlockSizes;

    void reset(){
        tempBlockSizes = BlockSizes;
        allocations.assign(np,-1);
    }

    void printAllocations(const string& processName){
        cout<<"\n---"<<processName<<"---\n";
        for(int i=0; i<np; i++){
            if(allocations[i]!=-1){
                cout<<"Process"<<i+1<<"--> Block"<<allocations[i]+1<<endl;
            }
            else{
                cout<<"Process"<<i+1<<"--> Not allocated"<<endl;
            }
        }
    }

    void firstFit(){
        reset();
        for(int i=0; i<np; i++){
            for(int j=0; j<nb; j++){
                if(tempBlockSizes[j]>=ProcessSizes[i]){
                    allocations[i] = j;
                    tempBlockSizes[j] -= ProcessSizes[i];
                    break;
                }
            }
        }
        printAllocations("First Fit");
    }

    void NextFit() {
    reset();
    int pos = 0;
    for (int i = 0; i < np; i++) {
        bool allocated_memory = false;
        int start = pos;        // remember where we started

        do {
            if (tempBlockSizes[pos] >= ProcessSizes[i]) {
                allocations[i] = pos;
                tempBlockSizes[pos] -= ProcessSizes[i];
                allocated_memory = true;
                break;
            }
            pos = (pos + 1) % nb;   // move to next block (wrap around)
        } while (pos != start);      // stop when we’ve checked all blocks

        if (!allocated_memory) {
            allocations[i] = -1;     // not allocated
        }
    }

    printAllocations("Next Fit");
    }


    void bestFit(){
        reset();
        for(int i=0; i<np; i++){
            int bstidx = -1;
            for(int j=0; j<nb; j++){
                if(tempBlockSizes[j]>=ProcessSizes[i]){
                    if(bstidx==-1||tempBlockSizes[j]<tempBlockSizes[bstidx]){
                        bstidx = j;
                    }
                }
            }
            if(bstidx!=-1){
                allocations[i] = bstidx;
                tempBlockSizes[bstidx] -= ProcessSizes[i];
                
            }
        }
        printAllocations("Best Fit");
    }

    void worstFit(){
        reset();
        for(int i=0; i<np; i++){
            int worstidx = -1;
            for(int j=0; j<nb; j++){
                if(tempBlockSizes[j]>=ProcessSizes[i]){
                    if(worstidx==-1||tempBlockSizes[j]>tempBlockSizes[worstidx]){
                        worstidx = j;
                    }
                }
            }
            if(worstidx!=-1){
                allocations[i] = worstidx;
                tempBlockSizes[worstidx] -= ProcessSizes[i];
                
            }
        }
        printAllocations("Worst Fit");
    }

public:
    MemoryAllocator(){
        nb =0;
        np = 0;
    }

    void readInput() {
        cout << "Enter number of memory blocks: ";
        cin >> nb;
        cout << "Enter size of each block: ";
        BlockSizes.resize(nb);
        for (int i = 0; i < nb; i++) {
            cin >> BlockSizes[i];
        }

        cout << "\nEnter number of processes: ";
        cin >> np;
        cout << "Enter memory required by each process: ";
        ProcessSizes.resize(np); 
        for (int i = 0; i < np; i++) {
            cin >> ProcessSizes[i];
        }
    }

    void runAllocator(){
        firstFit();
        NextFit();
        bestFit();
        worstFit();
    }

};

int main(){
    MemoryAllocator mem;
    mem.readInput();
    mem.runAllocator();
    return 0;
}