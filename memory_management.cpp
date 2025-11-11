#include <iostream>
#include <vector>    // Use vector for dynamic arrays (better than int[10])
#include <string>    // To pass strategy names
#include <numeric>   // For std::iota (though not strictly needed, good to know)

using namespace std;

// This class will hold all the data and the logic
class MemoryAllocator {
private:
    // --- 1. Member Variables (The Data) ---
    // These store the initial state
    vector<int> blockSize;
    vector<int> processSize;
    int nb; // number of blocks
    int np; // number of processes

    // These store the results of a single simulation
    vector<int> allocation;
    vector<int> tempBlockSize; // The "working copy" of block sizes

    // --- 2. Private Helper Methods (The Repetitive Logic) ---

    /**
     * @brief Resets the simulation state.
     * Copies original block sizes to the temp list.
     * Clears the allocation results (sets all to -1).
     */
    void reset() {
        // Copy the original blocks to our temporary "working" copy
        tempBlockSize = blockSize;
        
        // Resize the allocation vector and set all values to -1
        allocation.assign(np, -1);
    }

    /**
     * @brief Prints the results of the latest allocation simulation.
     * @param strategyName The name of the strategy to print (e.g., "FIRST FIT")
     */
    void printAllocation(const string& strategyName) {
        cout << "\n--- " << strategyName << " ---\n";
        for (int i = 0; i < np; i++) {
            if (allocation[i] != -1) {
                cout << "Process " << i + 1 << " -> Block " << allocation[i] + 1 << endl;
            } else {
                cout << "Process " << i + 1 << " -> Not Allocated\n";
            }
        }
    }

    // --- 3. Private Simulation Logic (The Core Algorithms) ---

    void firstFit() {
        reset(); // Start with a fresh state
        for (int i = 0; i < np; i++) {
            for (int j = 0; j < nb; j++) {
                if (tempBlockSize[j] >= processSize[i]) {
                    allocation[i] = j;
                    tempBlockSize[j] -= processSize[i];
                    break; // Go to the next process
                }
            }
        }
        printAllocation("FIRST FIT"); // Print the results for this run
    }

    void bestFit() {
        reset(); // Start fresh
        for (int i = 0; i < np; i++) {
            int bestIdx = -1;
            for (int j = 0; j < nb; j++) {
                if (tempBlockSize[j] >= processSize[i]) {
                    if (bestIdx == -1 || tempBlockSize[j] < tempBlockSize[bestIdx]) {
                        bestIdx = j;
                    }
                }
            }
            if (bestIdx != -1) {
                allocation[i] = bestIdx;
                tempBlockSize[bestIdx] -= processSize[i];
            }
        }
        printAllocation("BEST FIT");
    }

    void worstFit() {
        reset(); // Start fresh
        for (int i = 0; i < np; i++) {
            int worstIdx = -1;
            for (int j = 0; j < nb; j++) {
                if (tempBlockSize[j] >= processSize[i]) {
                    if (worstIdx == -1 || tempBlockSize[j] > tempBlockSize[worstIdx]) {
                        worstIdx = j;
                    }
                }
            }
            if (worstIdx != -1) {
                allocation[i] = worstIdx;
                tempBlockSize[worstIdx] -= processSize[i];
            }
        }
        printAllocation("WORST FIT");
    }

    void nextFit() {
        reset(); // Start fresh
        int pos = 0; // The "memory" pointer for next-fit
        
        for (int i = 0; i < np; i++) {
            bool allocated = false;
            // Search from the last position
            for (int j = pos; j < nb; j++) {
                if (tempBlockSize[j] >= processSize[i]) {
                    allocation[i] = j;
                    tempBlockSize[j] -= processSize[i];
                    pos = j; // Update position
                    allocated = true;
                    break;
                }
            }
            
            // If not found, reset position (as in your original code)
            if (!allocated) {
                 pos = 0;
            }
        }
        printAllocation("NEXT FIT");
    }


public:
    // --- 4. Public Interface (The "Controls") ---

    /**
     * @brief Constructor to initialize counts
     */
    MemoryAllocator() : nb(0), np(0) {}

    /**
     * @brief Gets all the input from the user.
     */
    void readInput() {
        cout << "Enter number of memory blocks: ";
        cin >> nb;
        cout << "Enter size of each block: ";
        blockSize.resize(nb); // Make room for 'nb' blocks
        for (int i = 0; i < nb; i++) {
            cin >> blockSize[i];
        }

        cout << "\nEnter number of processes: ";
        cin >> np;
        cout << "Enter memory required by each process: ";
        processSize.resize(np); // Make room for 'np' processes
        for (int i = 0; i < np; i++) {
            cin >> processSize[i];
        }
    }

    /**
     * @brief Runs all four allocation strategies back-to-back.
     */
    void runAllStrategies() {
        firstFit();
        bestFit();
        worstFit();
        nextFit();
    }
};

// ===================== MAIN FUNCTION =====================
int main() {
    // 1. Create an allocator object
    MemoryAllocator allocator;

    // 2. Tell it to get the input
    allocator.readInput();

    // 3. Tell it to run all its simulations
    allocator.runAllStrategies();

    return 0; // The main function is now extremely clean
}