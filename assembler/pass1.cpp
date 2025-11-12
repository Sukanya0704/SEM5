#include<bits/stdc++.h>
using namespace std;

// --- Data Structure Definitions (Simplified) ---
struct SymbolTableEntry {
    string symbol;
    int address = -1;
    int length = 1;
};

struct LiteralTableEntry {
    string literal;
    int address = -1;
};

// --- Assembler Class ---
class PassOneAssembler {
private:
    int lc = 0;
    vector<pair<int, string>> intermediateCode;
    map<string, SymbolTableEntry> symbolTable; // Use map for easy index finding
    vector<LiteralTableEntry> literalTable;
    vector<int> poolTable = {0};

    // Opcode and Register Tables (Fixed Data)
    const unordered_map<string, string> optab = {
        {"STOP", "(IS, 00)"}, {"ADD", "(IS, 01)"}, {"SUB", "(IS, 02)"},
        {"MOVER", "(IS, 04)"}, {"MOVEM", "(IS, 05)"}, {"READ", "(IS, 09)"},
        {"PRINT", "(IS, 10)"}, // etc.
    };
    const unordered_map<string, int> regtab = {
        {"AREG", 1}, {"BREG", 2}, {"CREG", 3}, {"DREG", 4}
    };

    // Helper Functions
    static string trim(string s) {
        s.erase(s.begin(), find_if(s.begin(), s.end(), [](int ch) { return !isspace(ch); }));
        s.erase(find_if(s.rbegin(), s.rend(), [](int ch) { return !isspace(ch); }).base(), s.end());
        return s;
    }

    vector<string> tokenize(const string &line) {
        vector<string> tokens;
        stringstream ss(line);
        string token;
        while (ss >> token) {
            if (token.back() == ',') token.pop_back(); // Remove trailing comma immediately
            tokens.push_back(token);
        }
        return tokens;
    }

    // Finds the index of a symbol in the symbol table map (0-based)
    int getSymbolIndex(const string &symbol) {
        int index = 0;
        for (const auto &kv : symbolTable) {
            if (kv.first == symbol) return index;
            index++;
        }
        return -1; // Should ideally not happen if handled correctly
    }

public:
    void processFile(const string &inputFileName) {
        ifstream input(inputFileName);
        if (!input) { cerr << "Error opening file: " << inputFileName << "\n"; return; }

        string line;
        while (getline(input, line)) {
            line = trim(line);
            if (line.empty()) continue;
            processLine(line);
        }

        handleLiteralPool(true); // Process remaining literals at END
        printOutput();
    }

    void processLine(const string &line) {
        vector<string> tokens = tokenize(line);
        if (tokens.empty()) return;

        string label = "";
        string mnemonic = tokens[0];
        int tokenIndex = 0;

        // 1. Label Processing
        if (optab.find(tokens[0]) == optab.end() && tokens[0] != "START" && tokens[0] != "END" &&
            tokens[0] != "ORIGIN" && tokens[0] != "EQU" && tokens[0] != "LTORG" &&
            tokens[0] != "DS" && tokens[0] != "DC") {
            label = tokens[0];
            mnemonic = tokens[1];
            tokenIndex = 1;
            
            // If label is present, update/insert symbol in Symbol Table
            if (symbolTable.find(label) == symbolTable.end()) {
                symbolTable[label] = SymbolTableEntry{label, -1, 1}; // Default entry
            }
            if (symbolTable[label].address == -1) {
                symbolTable[label].address = lc; // Assign address if not already assigned (e.g., in EQU)
            }
        }

        string operand1 = (tokenIndex + 1 < (int)tokens.size()) ? tokens[tokenIndex + 1] : "";
        string operand2 = (tokenIndex + 2 < (int)tokens.size()) ? tokens[tokenIndex + 2] : "";

        string ic;
        int currentLC = lc; // LC before processing the instruction

        // 2. Mnemonic Processing
        if (mnemonic == "START") {
            lc = stoi(operand1);
            ic = "(AD, 01) (C, " + operand1 + ")";
        } else if (mnemonic == "LTORG") {
            ic = "(AD, 05)";
            handleLiteralPool(false);
        } else if (mnemonic == "END") {
            ic = "(AD, 02)";
            // Final literal pool processing done in processFile
        } else if (mnemonic == "DS") {
            int size = stoi(operand1);
            if (!label.empty()) symbolTable[label].length = size;
            ic = "(DL, 01) (C, " + operand1 + ")";
            lc += size;
        } else if (mnemonic == "DC") {
            string constant = operand1;
            // Remove quotes from constant e.g., '10' -> 10
            if (constant.size() > 2 && constant.front() == '\'' && constant.back() == '\'') {
                constant = constant.substr(1, constant.size() - 2);
            }
            ic = "(DL, 02) (C, " + constant + ")";
            lc++;
        } else if (mnemonic == "EQU") {
            if (!label.empty()) {
                if (symbolTable.count(operand1)) {
                    symbolTable[label].address = symbolTable[operand1].address;
                } else {
                    cerr << "Error: Undefined symbol " << operand1 << " in EQU\n";
                    return;
                }
            }
            ic = "(AD, 04)";
        } else if (mnemonic == "ORIGIN") {
            // Simplified ORIGIN: assuming simple symbol or symbol+offset
            int new_lc = 0;
            if (operand1.find('+') != string::npos) {
                // Not implemented in this reduced version for brevity, but logic would resolve address
                cerr << "Warning: Complex ORIGIN not fully resolved.\n";
            } else if (symbolTable.count(operand1)) {
                new_lc = symbolTable[operand1].address;
            } else {
                cerr << "Error: Undefined symbol in ORIGIN.\n"; return;
            }
            lc = new_lc;
            ic = "(AD, 03) (S, " + to_string(getSymbolIndex(operand1)) + ")";
        } else if (optab.count(mnemonic)) { // Imperative Statement (IS)
            ic = optab.at(mnemonic) + " ";

            if (regtab.count(operand1)) {
                ic += "(" + to_string(regtab.at(operand1)) + ") ";
            } else {
                // Assume 1 operand IS like STOP/READ/PRINT if no register
                operand2 = operand1; 
                operand1 = ""; 
            }

            if (!operand2.empty()) {
                if (operand2.rfind("='", 0) == 0) { // Literal detected
                    literalTable.push_back({operand2, -1});
                    ic += "(L, " + to_string(literalTable.size() - 1) + ")";
                } else { // Symbol detected
                    if (symbolTable.find(operand2) == symbolTable.end()) {
                        symbolTable[operand2] = SymbolTableEntry{operand2, -1, 1}; // Forward reference
                    }
                    ic += "(S, " + to_string(getSymbolIndex(operand2)) + ")";
                }
            }
            lc++; // IS instructions occupy 1 memory unit
        } else {
            cerr << "Unknown mnemonic: " << mnemonic << " on line: " << line << "\n";
            return;
        }

        intermediateCode.emplace_back(currentLC, ic);
    }

    void handleLiteralPool(bool isEnd) {
        int poolStartIndex = poolTable.back();
        for (size_t i = poolStartIndex; i < literalTable.size(); ++i) {
            if (literalTable[i].address == -1) {
                literalTable[i].address = lc;

                string lit = literalTable[i].literal;
                // Extract constant value from literal e.g., ='10' -> 10
                lit = lit.substr(2, lit.size() - 3);

                intermediateCode.emplace_back(lc, "(DL, 02) (C, " + lit + ")");
                lc++;
            }
        }
        if (!isEnd && poolStartIndex < (int)literalTable.size()) {
            poolTable.push_back((int)literalTable.size());
        }
    }

    void printOutput() {
        cout << "\n=========== SYMBOL TABLE ===========\n";
        cout << "Symbol\tAddress\tLength\n";
        cout << "----------------------------------\n";
        for (const auto &kv : symbolTable)
            cout << kv.second.symbol << "\t" << kv.second.address << "\t" << kv.second.length << "\n";

        cout << "\n=========== LITERAL TABLE ===========\n";
        cout << "Literal\tAddress\n";
        cout << "----------------------\n";
        for (const auto &entry : literalTable)
            cout << entry.literal << "\t" << entry.address << "\n";

        cout << "\n=========== POOL TABLE =============\n";
        for (auto idx : poolTable)
            cout << "#" << idx << "\n";

        cout << "\n=========== INTERMEDIATE CODE (IC) =============\n";
        cout << "LC\tCode\n";
        cout << "--------------------------\n";
        for (const auto &entry : intermediateCode)
            cout << entry.first << "\t" << entry.second << "\n";
    }
};

int main() {
    PassOneAssembler assembler;
    // NOTE: Ensure 'input.asm' exists with assembly code
    assembler.processFile("input.asm"); 
    return 0;
}