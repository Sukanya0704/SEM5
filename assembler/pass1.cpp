#include <bits/stdc++.h>
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
    map<string, SymbolTableEntry> symbolTable;
    vector<LiteralTableEntry> literalTable;
    vector<int> poolTable = {0};

    // Opcode, Register, and Condition Code Tables (Fixed Data)
    const unordered_map<string, string> optab = {
        {"STOP", "(IS, 00)"}, {"ADD", "(IS, 01)"}, {"SUB", "(IS, 02)"},
        {"MOVER", "(IS, 04)"}, {"MOVEM", "(IS, 05)"}, {"COMP", "(IS, 06)"},
        {"BC", "(IS, 07)"}, // Branch on Condition
        {"DIV", "(IS, 08)"}, {"READ", "(IS, 09)"}, {"PRINT", "(IS, 10)"},
    };
    const unordered_map<string, int> regtab = {
        {"AREG", 1}, {"BREG", 2}, {"CREG", 3}, {"DREG", 4}
    };
    // Condition Code Table (CC)
    const unordered_map<string, int> cctab = {
        {"LT", 1}, {"LE", 2}, {"EQ", 3}, {"GT", 4}, {"GE", 5}, {"ANY", 6} // 'ANY' often means unconditional branch
    };

    // Helper Functions (Same as before)
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

    int getSymbolIndex(const string &symbol) {
        int index = 1;
        for (const auto &kv : symbolTable) {
            if (kv.first == symbol) return index;
            index++;
        }
        return -1;
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

        // 1. Label Processing (Identical to previous version)
        if (optab.find(tokens[0]) == optab.end() && tokens[0] != "START" && tokens[0] != "END" &&
            tokens[0] != "ORIGIN" && tokens[0] != "EQU" && tokens[0] != "LTORG" &&
            tokens[0] != "DS" && tokens[0] != "DC") {
            label = tokens[0];
            mnemonic = tokens[1];
            tokenIndex = 1;
            
            if (symbolTable.find(label) == symbolTable.end()) {
                symbolTable[label] = SymbolTableEntry{label, -1, 1};
            }
            if (symbolTable[label].address == -1) {
                symbolTable[label].address = lc;
            }
        }

        string operand1 = (tokenIndex + 1 < (int)tokens.size()) ? tokens[tokenIndex + 1] : "";
        string operand2 = (tokenIndex + 2 < (int)tokens.size()) ? tokens[tokenIndex + 2] : "";

        string ic;
        int currentLC = lc;

        // 2. Mnemonic Processing
        if (mnemonic == "START") {
            lc = stoi(operand1);
            ic = "(AD, 01) (C, " + operand1 + ")";
        } 
        // ... (AD and DL directives logic is omitted for brevity but remains the same) ...
        else if (mnemonic == "LTORG") {
            ic = "(AD, 05)";
            handleLiteralPool(false);
        } else if (mnemonic == "END") {
            ic = "(AD, 02)";
        } else if (mnemonic == "DS") {
            int size = stoi(operand1);
            if (!label.empty()) symbolTable[label].length = size;
            ic = "(DL, 01) (C, " + operand1 + ")";
            lc += size;
        } else if (mnemonic == "DC") {
            string constant = operand1;
            if (constant.size() > 2 && constant.front() == '\'' && constant.back() == '\'') {
                constant = constant.substr(1, constant.size() - 2);
            }
            ic = "(DL, 02) (C, " + constant + ")";
            lc++;
        }
        // ... (EQU and ORIGIN logic is omitted for brevity but remains the same) ...
        else if (mnemonic == "EQU") {
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
            // Simplified ORIGIN handling for this example
            int new_lc = 0;
            if (symbolTable.count(operand1)) {
                new_lc = symbolTable[operand1].address;
            } else {
                cerr << "Error: Undefined symbol in ORIGIN.\n"; return;
            }
            lc = new_lc;
            ic = "(AD, 03) (S, " + to_string(getSymbolIndex(operand1)) + ")";
        }
        else if (optab.count(mnemonic)) { // Imperative Statement (IS)
            ic = optab.at(mnemonic) + " ";
            
            // Special handling for BC (Branch on Condition)
            if (mnemonic == "BC" && cctab.count(operand1)) {
                // BC LT, NEXT -> (IS, 07) (1) (S, #)
                ic += "(" + to_string(cctab.at(operand1)) + ") ";
                // Operand 2 is the target symbol
                if (!operand2.empty()) {
                    if (symbolTable.find(operand2) == symbolTable.end()) {
                        symbolTable[operand2] = SymbolTableEntry{operand2, -1, 1}; // Forward reference
                    }
                    ic += "(S, " + to_string(getSymbolIndex(operand2)) + ")";
                }
            }
            // General handling for other IS instructions (e.g., MOVER BREG, A)
            else {
                if (regtab.count(operand1)) {
                    ic += "(" + to_string(regtab.at(operand1)) + ") ";
                } 
                
                // Determine the second operand (which might be the first operand for single-operand instructions)
                string second_op = (mnemonic == "READ" || mnemonic == "PRINT") ? operand1 : operand2;

                if (!second_op.empty()) {
                    if (second_op.rfind("='", 0) == 0) { // Literal detected
                        literalTable.push_back({second_op, -1});
                        ic += "(L, " + to_string(literalTable.size() - 1) + ")";
                    } else { // Symbol detected
                        if (symbolTable.find(second_op) == symbolTable.end()) {
                            symbolTable[second_op] = SymbolTableEntry{second_op, -1, 1}; // Forward reference
                        }
                        ic += "(S, " + to_string(getSymbolIndex(second_op)) + ")";
                    }
                }
            }
            lc++; // IS instructions occupy 1 memory unit
        } else {
            cerr << "Unknown mnemonic: " << mnemonic << " on line: " << line << "\n";
            return;
        }

        intermediateCode.emplace_back(currentLC, ic);
    }

    void handleLiteralPool(bool isEnd) { /* Logic remains identical */
        int poolStartIndex = poolTable.back();
        for (size_t i = poolStartIndex; i < literalTable.size(); ++i) {
            if (literalTable[i].address == -1) {
                literalTable[i].address = lc;
                string lit = literalTable[i].literal;
                lit = lit.substr(2, lit.size() - 3);
                intermediateCode.emplace_back(lc, "(DL, 02) (C, " + lit + ")");
                lc++;
            }
        }
        if (!isEnd && poolStartIndex < (int)literalTable.size()) {
            poolTable.push_back((int)literalTable.size());
        }
    }

    void printOutput() { /* Logic remains identical */
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