#include<bits/stdc++.h>
using namespace std;

vector<vector<string>> SRC, MNT, MDT, KPDT;
vector<string> APT, OUT;

vector<string> split(const string& s, char d = ' ') {
    vector<string> v; string x; stringstream ss(s);
    while (getline(ss, x, d)) 
    if (!x.empty()) v.push_back(x);
    return v;
}

void read_file(const string& f, vector<vector<string>>& t) {
    ifstream fin(f);
    if (!fin) { cerr << "Can't open " << f << endl; exit(1); }
    string line; 
    while (getline(fin, line)) if (!line.empty()) t.push_back(split(line));
}

pair<bool, vector<string>> is_macro(const vector<string>& line) {
    if (line.empty()) return {false, {}};
    int pp = 0, kp = 0;
    for (size_t i = 1; i < line.size(); ++i)
        (line[i].find('=') != string::npos) ? kp++ : pp++;
    for (auto& e : MNT)
        if (line[0] == e[0] && pp == stoi(e[1]) && kp <= stoi(e[2])) return {true, e};
    return {false, {}};
}

vector<string> get_args(const vector<string>& line, const vector<string>& m) {
    vector<string> args;
    int kp = stoi(m[2]), kpdt_i = stoi(m[4]) - 1;
    vector<pair<string, string>> kps;
    for (int i = 0; i < kp; i++) kps.push_back({KPDT[kpdt_i+i][0], KPDT[kpdt_i+i][1]});
    for (size_t i = 1; i < line.size(); i++)
        if (line[i].find('=') == string::npos) args.push_back(line[i]);
        else {
            auto p = line[i].find('=');
            for (auto& k : kps)
                if (k.first == line[i].substr(0, p))
                    k.second = line[i].substr(p+1);
        }
    for (auto& k : kps)
        if (k.second != "_") args.push_back(k.second);
        else throw runtime_error("Missing arg for " + k.first);
    return args;
}

void expand(int mdt_i) {
    for (int i = mdt_i; i < (int)MDT.size(); i++) {
        if (find(MDT[i].begin(), MDT[i].end(), "MEND") != MDT[i].end()) break;
        string out;
        for (auto& t : MDT[i]) {
            if (t.find("(P,") != string::npos) {
                int idx = stoi(t.substr(t.find(',')+1)) - 1;
                out += (idx < (int)APT.size()) ? APT[idx] + " " : t + " ";
            } else out += t + " ";
        }
        OUT.push_back(out);
    }
}

int main() {
    string path; 
    cout << "Enter Testcase folder: ";
    getline(cin, path);

    read_file(path + "/src.txt", SRC);
    read_file(path + "/MNT.txt", MNT);
    read_file(path + "/MDT.txt", MDT);
    read_file(path + "/KPDT.txt", KPDT);

    for (auto& line : SRC) {
        auto [ok, m] = is_macro(line);
        if (ok) { 
            try { APT = get_args(line, m); }
            catch (exception& e) { cerr << e.what() << endl; return 1; }
            expand(stoi(m[3]) - 1);
        } else {
            string out; for (auto& t : line) out += t + " ";
            OUT.push_back(out);
        }
    }

    for (auto& l : OUT) cout << l << endl;
}