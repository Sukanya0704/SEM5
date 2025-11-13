#include <iostream>
#include <sstream>
#include <unordered_map>
#include <string>
#include <algorithm>
#include <fstream>
#include <vector>
using namespace std;

vector<string> split(string line)
{
    string word;
    vector<string> v;
    stringstream ss(line);
    while (ss >> word)
    {
        v.push_back(word);
    }
    return v;
}

class MNTEntry
{
public:
    string name;
    int pp;
    int kp;
    int mdtp;
    int kpdtp;
    MNTEntry(string n = "", int p = 0, int k = 0, int m = 0, int kpdtp_val = 0)
    {
        name = n;
        pp = p;
        kp = k;
        mdtp = m;
        kpdtp = kpdtp_val;
    }
    int getpp() { return pp; }
    int getkp() { return kp; }
    int getmdtp() { return mdtp; }
    int getkpdtp() { return kpdtp; }
};

int main()
{

    ifstream fin("intermediate.txt"), mdtb("mdt.txt"), mntb("mnt.txt"), kpdtb("kpdt.txt");
    ofstream fout("output.txt");
    if (!fin.is_open() || !mdtb.is_open() || !mntb.is_open() || !kpdtb.is_open() || !fout.is_open())
    {
        cout << "Error in opening files " << endl;
        return 1;
    }
    unordered_map<string, MNTEntry> mnt;
    unordered_map<int, string> aptab;
    unordered_map<string, int> aptabinverse;
    vector<string> mdt, kpdt;
    string line;
    while (getline(mdtb, line))
        mdt.push_back(line);
    while (getline(kpdtb, line))
        kpdt.push_back(line);
    while (getline(mntb, line))
    {
        stringstream ss(line);
        string name;
        int pp;
        int kp;
        int mdtp;
        int kpdtp;
        ss >> name >> pp >> kp >> mdtp >> kpdtp;
        mnt[name] = MNTEntry(name, pp, kp, mdtp, kpdtp);
    }

    while (getline(fin, line))
    {
        vector<string> parts = split(line);
        if (mnt.count(parts[0]))
        {
            MNTEntry &entry = mnt[parts[0]];
            int pp = entry.getpp();
            int kp = entry.getkp();
            int mdtp = entry.getmdtp();
            int kpdtp = entry.getkpdtp();
            int paramno = 1;
            for (int i = 1; i <= pp && i < parts.size(); i++)
            {
                aptab[paramno] = parts[i];
                aptabinverse[parts[i]] = paramno++;
            }
            for (int i = kpdtp - 1; i < kpdtp - 1 + kp && i < kpdt.size(); i++)
            {
                stringstream ss(kpdt[i]);
                string paramname, defaultvalue;
                ss >> paramname >> defaultvalue;
                aptab[paramno] = defaultvalue;
                aptabinverse[paramname] = paramno++;
                cout << "Adding to aptab: " << paramno << " -> " << defaultvalue << endl;
                cout << "Adding to aptabinverse: " << paramname << " -> " << paramno << endl;
            }
            for (int i = pp + 1; i < parts.size(); i++)
            {
                // int pos=parts[i].find("=");
                if (parts[i].find("=") != string::npos)
                {
                    int pos = parts[i].find("=");
                    string keyword = parts[i].substr(0, pos);
                    string value = parts[i].substr(pos + 1);
                    if (aptabinverse.count(keyword))
                        aptab[aptabinverse[keyword]] = value;
                }
            }

            for (int i = mdtp - 1; i < mdt.size() && mdt[i] != "MEND"; i++)
            {
                stringstream ss(mdt[i]);
                string token;
                fout << "+";
                while (ss >> token)
                {
                    if (token.find("(p,") != string::npos)
                    {
                        int num = stoi(token.substr(3, token.find(')') - 3));
                        fout << aptab[num] << " ";
                    }
                    else
                    {
                        fout << token << " ";
                    }
                }
                fout << "\n";
            }
            aptab.clear();
            aptabinverse.clear();
        }
    }

    cout << "Pass 2 of two pass Macroprocessor done successfully! " << endl;

    return 0;
}