#include <bits/stdc++.h>
using namespace std;

// read symtab/littab  (format: index symbol address)
map<string, string> readTable(const string &filename) {
    map<string, string> table;
    ifstream f(filename);
    if (!f.is_open()) return table;
    string i, s, a;
    while (f >> i >> s >> a) table[i] = a;
    return table;
}

int main() {
    ifstream ic("intermediate.txt");
   

    map<string, string> symtab = readTable("symtab.txt");
    map<string, string> littab = readTable("littab.txt");

    cout << "\n----- PASS 2 OUTPUT -----\n";
    ofstream out("output.txt");

    string line;
    int lc = 0;
    bool started = false;

    while (getline(ic, line)) {
        if (line.empty()) continue;
        for (char &c : line)
            if (c == '(' || c == ')' || c == ',') c = ' ';
        stringstream ss(line);
        vector<string> p; string w;
        while (ss >> w) p.push_back(w);
        if (p.empty()) continue;

        string type = p[0], code = p.size() > 1 ? p[1] : "00";
        string reg = "0", operand = "000";

        // START sets LC
        if (type == "AD" && code == "1" && p.size() > 3 && p[2] == "C") {
            lc = stoi(p[3]);
            started = true;
            continue;
        }

        // LTORG/END assign literals, no output
        if (type == "AD" && (code == "5" || code == "2")) {
            // assign literal addresses if not done
            continue;
        }

        if (!started) lc = 0;

        if (type == "IS") {
            string opcode = code;
            if (p.size() > 2) reg = p[2];
            if (p.size() > 4) {
                string kind = p[3], idx = p[4];
                if (kind == "S" && symtab.count(idx)) operand = symtab[idx];
                else if (kind == "L" && littab.count(idx)) operand = littab[idx];
                else if (kind == "C") operand = idx;
            }
            cout << lc << "  " << opcode << "  " << reg << "  " << operand << "\n";
            out  << lc << "  " << opcode << "  " << reg << "  " << operand << "\n";
            lc++;  // increment after every instruction
        }

        else if (type == "DL") { // DS/DC
            string val = (p.size() > 3) ? p.back() : "000";
            cout << lc << "  00  0  " << val << "\n";
            out  << lc << "  00  0  " << val << "\n";
            lc++;
        }
    }

    cout << "\nMachine code written to output.txt\n";
}