#include <iostream>
#include <fstream>
#include <vector>
#include <thread>
#include <chrono>
#include <atomic>
#include <random>
#include <mutex>
#include <cmath>
#include <memory>
#include <iomanip>
#include <sstream>
#include <numeric>

using namespace std;
using namespace std::chrono;

int n, k;
double lambda1, lambda2;
mutex output_mutex;
atomic_bool tas_lock;
atomic_int cas_lock;
atomic_int turn;

long long startTime;

string getTimeString() {
    auto now = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(now - time_point<high_resolution_clock, milliseconds>(milliseconds(startTime)));
    long long ms = duration.count();
    long long hours = ms / (3600 * 1000);
    ms %= (3600 * 1000);
    long long minutes = ms / (60 * 1000);
    ms %= (60 * 1000);
    long long seconds = ms / 1000;
    ms %= 1000;

    stringstream ss;
    ss << setw(2) << setfill('0') << hours << ":"
       << setw(2) << setfill('0') << minutes << ":"
       << setw(2) << setfill('0') << seconds << "."
       << setw(3) << setfill('0') << ms;
    return ss.str();
}

double expDelay(double lambda) {
    random_device rd;
    mt19937 gen(rd());
    exponential_distribution<> dist(1.0 / lambda);
    return dist(gen);
}

void tasEntry() { while (tas_lock.exchange(true)) {} }
void tasExit() { tas_lock.store(false); }

void casEntry() {
    while (true) {
        int expected = 0;
        if (cas_lock.compare_exchange_weak(expected, 1)) {
            break;
        }
    }
}
void casExit() { cas_lock.store(0); }

void boundedCasEntry(int id, vector<unique_ptr<atomic_int>>& entering) {
    entering[id]->store(1);
    turn.store(id);
    while (entering[turn.load()]->load() == 1 && turn.load() != id) {}
    entering[id]->store(0);
}

void boundedCasExit(int id, vector<unique_ptr<atomic_int>>& entering) {
    int next = (id + 1) % n;
    while (next != id && entering[next]->load() == 0) { next = (next + 1) % n; }
    turn.store(next);
}

void runThread(int id, string algo, ofstream& outFile, vector<unique_ptr<atomic_int>>& entering, vector<vector<long long>>& entryTimes, vector<long long>& waitingTimes) {
    for (int i = 0; i < k; ++i) {
        long long reqTime = duration_cast<milliseconds>(high_resolution_clock::now().time_since_epoch()).count();
        {
            lock_guard<mutex> lock(output_mutex);
            outFile << setw(3) << setfill('0') << i + 1 << " CS Requested at " << getTimeString() << " by thread " << setw(3) << setfill('0') << id + 1 << endl;
        }

        if (algo == "tas") tasEntry();
        else if (algo == "cas") casEntry();
        else boundedCasEntry(id, entering);

        long long entryTime = duration_cast<milliseconds>(high_resolution_clock::now().time_since_epoch()).count();
        {
            lock_guard<mutex> lock(output_mutex);
            outFile << setw(3) << setfill('0') << i + 1 << " CS Entered  at " << getTimeString() << " by thread " << setw(3) << setfill('0') << id + 1 << endl;
        }

        waitingTimes[id] += (entryTime - reqTime);
        entryTimes[id].push_back(entryTime - reqTime);

        this_thread::sleep_for(milliseconds((int)expDelay(lambda1)));

        if (algo == "tas") tasExit();
        else if (algo == "cas") casExit();
        else boundedCasExit(id, entering);

        {
            lock_guard<mutex> lock(output_mutex);
            outFile << setw(3) << setfill('0') << i + 1 << " CS Exited   at " << getTimeString() << " by thread " << setw(3) << setfill('0') << id + 1 << endl;
        }

        this_thread::sleep_for(milliseconds((int)expDelay(lambda2)));
    }
}

int main() {
    ifstream inFile("inp-params.txt");
    if (!inFile.is_open()) { cerr << "Input error" << endl; return 1; }
    inFile >> n >> k >> lambda1 >> lambda2;
    inFile.close();

    // Output input parameters
    cout << "Input Parameters:" << endl;
    cout << "n = " << n;
    cout << "   k = " << k ;
    cout << "   lambda1 = " << lambda1 ;
    cout << "   lambda2 = " << lambda2 << endl;
    cout << endl;

    startTime = duration_cast<milliseconds>(high_resolution_clock::now().time_since_epoch()).count();

    vector<string> algos = {"tas", "cas", "bounded_cas"};
    vector<double> avgWaits(3, 0);
    vector<long long> worstWaits(3, 0);

    for (int algoIndex = 0; algoIndex < algos.size(); ++algoIndex) {
        string algo = algos[algoIndex];
        ofstream outFile(algo + "_ME_Output.txt");
        vector<unique_ptr<atomic_int>> entering(n);
        for (int i = 0; i < n; ++i) { entering[i] = make_unique<atomic_int>(0); }
        vector<vector<long long>> entryTimes(n);
        vector<long long> waitingTimes(n, 0);

        vector<thread> threads(n);
        for (int i = 0; i < n; ++i) { threads[i] = thread(runThread, i, algo, ref(outFile), ref(entering), ref(entryTimes), ref(waitingTimes)); }
        for (auto& t : threads) { t.join(); }
        outFile.close();

        long long totalWait = accumulate(waitingTimes.begin(), waitingTimes.end(), 0LL);
        avgWaits[algoIndex] = (double)totalWait / (n * k);

        long long worstWait = 0;
        for (int i = 0; i < n; ++i) {
            for (long long wait : entryTimes[i]) {
                if (wait > worstWait) {
                    worstWait = wait;
                }
            }
        }
        worstWaits[algoIndex] = worstWait;
    }

    cout << setw(10) << "---" << setw(15) << "TAS" << setw(15) << "CAS" << setw(15) << "B.CAS" << endl;
    cout << setw(10) << "Avg. Wait"
         << setw(15) << fixed << setprecision(2) << avgWaits[0]
         << setw(15) << fixed << setprecision(2) << avgWaits[1]
         << setw(15) << fixed << setprecision(2) << avgWaits[2] << endl;
    cout << setw(10) << "Worst Case"
         << setw(15) << worstWaits[0]
         << setw(15) << worstWaits[1]
         << setw(15) << worstWaits[2] << endl;

    return 0;
}