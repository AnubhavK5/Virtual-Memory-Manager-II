#include "../include/report.h"
#include <fstream>
#include <set>
#include <map>
#include <iomanip>
using namespace std;

void generateReport(int frames, const vector<int>& pages,
                    const Result& fifoRes, const Result& lruRes, const Result& optRes) {
    ofstream out("report.txt");
    if (!out.is_open()) return;

    set<int> uniquePages(pages.begin(), pages.end());
    map<int, int> freq;
    for (int p : pages) freq[p]++;

    out << "==== Virtual Memory Manager Report ====\n\n";
    out << "Total References: " << pages.size() << "\n";
    out << "Unique Pages: " << uniquePages.size() << "\n";
    out << "Frames Allocated: " << frames << "\n\n";

    out << "Page Reference Frequency:\n";
    for (auto& [page, count] : freq)
        out << "  Page " << page << " -> " << count << " times\n";

    out << "\n--- Performance Summary ---\n";
    out << left << setw(10) << "Algorithm" << setw(15) << "Page Faults" 
        << setw(10) << "Hits" << setw(10) << "Hit Ratio" << "\n";
    out << string(45, '-') << "\n";
    out << left << setw(10) << "FIFO" << setw(15) << fifoRes.pageFaults
        << setw(10) << fifoRes.hits << setw(10) << fifoRes.hitRatio << "\n";
    out << left << setw(10) << "LRU" << setw(15) << lruRes.pageFaults
        << setw(10) << lruRes.hits << setw(10) << lruRes.hitRatio << "\n";
    out << left << setw(10) << "Optimal" << setw(15) << optRes.pageFaults
        << setw(10) << optRes.hits << setw(10) << optRes.hitRatio << "\n";

    out.close();
}

void generateCSV(const Result& fifoRes, const Result& lruRes, const Result& optRes) {
    ofstream csv("results.csv");
    if (!csv.is_open()) return;

    csv << "Algorithm,PageFaults,Hits,HitRatio\n";
    csv << "FIFO," << fifoRes.pageFaults << "," << fifoRes.hits << "," << fifoRes.hitRatio << "\n";
    csv << "LRU," << lruRes.pageFaults << "," << lruRes.hits << "," << lruRes.hitRatio << "\n";
    csv << "Optimal," << optRes.pageFaults << "," << optRes.hits << "," << optRes.hitRatio << "\n";

    csv.close();
}
