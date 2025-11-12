#include<bits/stdc++.h>
using namespace std;

vector<string> split(const string &line)
{
    vector<string> parts;
    stringstream iss(line);
    string word;
    while (iss >> word)
    {
        parts.push_back(word);
    }
    return parts;
}

int main()
{
    ifstream fin("input.txt");
    ofstream mnt("mnt.txt");
    ofstream mdt("mdt.txt");
    ofstream kpdt("kpdt.txt");
    ofstream pnt("pnt.txt");
    ofstream fout("intermediate.txt");

    if (!fin.is_open() || !mnt.is_open() || !mdt.is_open() || !kpdt.is_open() || !pnt.is_open() || !fout.is_open())
    {
        cout << "Error in opening file." << endl;
        return 1;
    }

    unordered_map<string, int> pntabIndex;  // For lookup (name → number)
    vector<string> pntabList;               // For maintaining insertion order

    string line, macroname;
    int mdtp = 1, kpdtp = 0, paramNo = 1, pp = 0, kp = 0, flag = 0;

    while (getline(fin, line))
    {
        vector<string> parts = split(line);

        if (parts.empty())
            continue;

        if (parts[0] == "MACRO" || parts[0] == "macro")
        {
            flag = 1;
            if (!getline(fin, line))
                break;

            vector<string> parts2 = split(line);
            macroname = parts2[0];

            // Reset structures
            pntabIndex.clear();
            pntabList.clear();
            pp = 0;
            kp = 0;
            paramNo = 1;

            // Parse parameters
            for (int i = 1; i < parts2.size(); i++)
            {
                string param = parts2[i];
                // Remove '&' and ','
                param.erase(remove(param.begin(), param.end(), '&'), param.end());
                param.erase(remove(param.begin(), param.end(), ','), param.end());

                if (param.find('=') != string::npos)
                {
                    kp++;
                    int pos = param.find('=');
                    string keyword = param.substr(0, pos);
                    string value = param.substr(pos + 1);
                    pntabIndex[keyword] = paramNo++;
                    pntabList.push_back(keyword);
                    kpdt << keyword << "\t" << value << "\n";
                }
                else
                {
                    pp++;
                    pntabIndex[param] = paramNo++;
                    pntabList.push_back(param);
                }
            }

            // Write MNT entry
            mnt << macroname << "\t" << pp << "\t" << kp << "\t" << mdtp << "\t"
                << (kp == 0 ? kpdtp : (kpdtp + 1)) << "\n";

            kpdtp += kp;
        }
        else if (parts[0] == "MEND" || parts[0] == "mend")
        {
            mdt << "MEND\n";
            flag = 0;
            mdtp++;

            // Write PNTAB for this macro
            pnt << macroname << ":\t";
            for (auto &param : pntabList)
                pnt << param << "\t";
            pnt << "\n";
        }
        else if (flag == 1)
        {
            // Inside macro body
            for (const auto &part : parts)
            {
                if (part.find('&') != string::npos)
                {
                    string param = part;
                    param.erase(remove(param.begin(), param.end(), '&'), param.end());
                    param.erase(remove(param.begin(), param.end(), ','), param.end());
                    mdt << "(p," << pntabIndex[param] << ")\t";
                }
                else
                {
                    mdt << part << "\t";
                }
            }
            mdt << "\n";
            mdtp++;
        }
        else
        {
            // Non-macro line → intermediate code
            fout << line << "\n";
        }
    }

    fin.close();
    mnt.close();
    mdt.close();
    kpdt.close();
    pnt.close();
    fout.close();

    cout << "Pass 1 of Two-Pass Macroprocessor completed successfully!" << endl;
    return 0;
}