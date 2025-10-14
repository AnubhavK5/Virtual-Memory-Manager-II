#include "../include/vmm.h"
#include "../include/report.h"
#include <fstream>
#include <sstream>
using namespace std;

int main(int argc, char* argv[]) {
    if (argc < 3) {
        cerr << "Usage: ./vmm <trace_file> <num_frames> [--step]\n";
        return 1;
    }

    ifstream file(argv[1]);
    if (!file.is_open()) {
        cerr << "Error: Cannot open file.\n";
        return 1;
    }

    vector<int> pages;
    int n;
    while (file >> n) pages.push_back(n);

    int frames = stoi(argv[2]);
    bool stepMode = (argc == 4 && string(argv[3]) == "--step");

    cout << "\n==== Virtual Memory Manager ====\n";
    cout << "Frames: " << frames << "\n";
    cout << "Total References: " << pages.size() << "\n\n";

    Result fifoRes = fifo(pages, frames, stepMode);
    Result lruRes = lru(pages, frames, stepMode);
    Result optRes = optimal(pages, frames, stepMode);

    cout << "\nAlgorithm | Page Faults | Hits | Hit Ratio\n";
    cout << "--------------------------------------------\n";
    cout << "FIFO      | " << fifoRes.pageFaults << " | " << fifoRes.hits << " | " << fifoRes.hitRatio << "\n";
    cout << "LRU       | " << lruRes.pageFaults << " | " << lruRes.hits << " | " << lruRes.hitRatio << "\n";
    cout << "Optimal   | " << optRes.pageFaults << " | " << optRes.hits << " | " << optRes.hitRatio << "\n";

    generateReport(frames, pages, fifoRes, lruRes, optRes);
    generateCSV(fifoRes, lruRes, optRes);

    cout << "\nReport saved to report.txt and results.csv\n";

    return 0;
}
