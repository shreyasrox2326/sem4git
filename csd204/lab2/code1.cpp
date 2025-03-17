#include <iostream>
#include <unistd.h> 
#include <sys/types.h>
#include <sys/wait.h>

using namespace std;

int main() 
{
    int x;
    cout << "Enter number of iterations: ";
    cin >> x;
    pid_t cpid;
    for (int i=0; i<x; i++)
    {
        cpid=fork();
        if (cpid<0)
        {
            cerr << "Fork failed" <<endl;
            return 1;
        }
        else if (cpid>0)
        {
            pid_t pid = getpid();
            cout << "This is parent process #"<< pid << "   Waiting for child: #" << cpid <<endl;
            wait(NULL);
            cout << "This is parent process #"<< pid << "   Reaped child:      #" << cpid << " Exiting..." <<endl;
        }
        else if (cpid == 0)
        {
            pid_t pid = getpid();
            usleep(1000*100);
            cout << "This is child  process #" << pid <<endl;
        }
        
        
    }
}