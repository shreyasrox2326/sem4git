#include <iostream>
#include <unistd.h>
#include <fstream>
#include <iomanip>
#include <pwd.h>

using namespace std;

string symbolic_user_id (int input)
{
    struct passwd* pw = getpwuid(input);
    if (pw != nullptr) 
    {
        return (string(pw->pw_name));
    } 
    else
    {
        return string("NO_USER");
    }
}

string my_func(string input)
{
    int i=0;
    while (input[i] != ':')
    {
        i++;
    }
    i++;
    while (input[i] == ' ')
    {
        i++;
    }
    string line2 = input.substr(i);
    // line2 = line2.substr(0, line2.find_first_of(' '));
    return line2;
}


int extract_uid (string line)
{
    
    string line2 = line.substr(line.find_first_of("0123456789"));
    line2 = line2.substr(0, line2.find_first_not_of("1234567890"));
    return (stoi(line2));
}

int main ()
{
    if (chdir("/") == 0)
    cout << "Changed Directory to root.\n";

    if (chdir("./proc") == 0)
    cout << "Entered proc\n";

    cout << "PID:            NAME:           STATUS:        SYMBOLIC USERID:" << endl;
    for(int i=1; i<=32768; i++)
    {
        string dir = "./" + to_string(i);
        string line;
        if (chdir (dir.c_str()) == 0)
        {
            //cout << "PID:    ";
            cout << left << setw(10) << i;
            ifstream fin("status");
            for (int ln = 0; ln < 10; ln++)
            {
                getline(fin,line);
                if (ln == 0 or ln == 2)
                cout << left << setw(16) << my_func(line);
                if (ln == 8){
                    cout << left << setw(1) << symbolic_user_id(extract_uid(my_func(line)));
                    // cout << left << setw(10) << extract_uid(line);
                }
                
            }
            chdir("/proc");
            cout << "\n";
        }
    }
}