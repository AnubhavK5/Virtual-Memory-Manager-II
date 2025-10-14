#include "../include/vmm.h"
using namespace std;

// Helper: show frame contents
static void showFrames(const vector<int>& frames) {
    cout << "Frames: [ ";
    for (int p : frames) cout << p << " ";
    cout << "]\n";
}

Result fifo(const vector<int>& pages, int frames, bool stepMode) {
    queue<int> q;
    vector<int> frameList;
    int pageFaults = 0, hits = 0;

    cout << "\n--- FIFO Page Replacement ---\n";

    for (size_t i = 0; i < pages.size(); ++i) {
        int page = pages[i];
        bool hit = find(frameList.begin(), frameList.end(), page) != frameList.end();

        cout << "Page " << page << " -> ";

        if (hit) {
            cout << "HIT\n";
            hits++;
        } else {
            cout << "FAULT";
            pageFaults++;

            if ((int)frameList.size() < frames) {
                frameList.push_back(page);
                q.push(page);
            } else {
                int victim = q.front(); q.pop();
                replace(frameList.begin(), frameList.end(), victim, page);
                q.push(page);
                cout << " (Replaced " << victim << ")";
            }
            cout << "\n";
        }

        if (stepMode) {
            showFrames(frameList);
            cout << "Press Enter to continue...";
            cin.ignore();
        }
    }

    double hitRatio = (double)hits / pages.size();
    return {pageFaults, hits, hitRatio};
}

Result lru(const vector<int>& pages, int frames, bool stepMode) {
    vector<int> frameList;
    unordered_map<int, int> lastUsed;
    int pageFaults = 0, hits = 0;

    cout << "\n--- LRU Page Replacement ---\n";

    for (size_t i = 0; i < pages.size(); ++i) {
        int page = pages[i];
        bool hit = find(frameList.begin(), frameList.end(), page) != frameList.end();

        cout << "Page " << page << " -> ";

        if (hit) {
            cout << "HIT\n";
            hits++;
        } else {
            cout << "FAULT";
            pageFaults++;

            if ((int)frameList.size() < frames) {
                frameList.push_back(page);
            } else {
                int lruPage = frameList[0];
                int minIndex = lastUsed[lruPage];
                for (int p : frameList) {
                    if (lastUsed[p] < minIndex) {
                        minIndex = lastUsed[p];
                        lruPage = p;
                    }
                }
                replace(frameList.begin(), frameList.end(), lruPage, page);
                cout << " (Replaced " << lruPage << ")";
            }
            cout << "\n";
        }

        lastUsed[page] = i;

        if (stepMode) {
            showFrames(frameList);
            cout << "Press Enter to continue...";
            cin.ignore();
        }
    }

    double hitRatio = (double)hits / pages.size();
    return {pageFaults, hits, hitRatio};
}

Result optimal(const vector<int>& pages, int frames, bool stepMode) {
    vector<int> frameList;
    int pageFaults = 0, hits = 0;

    cout << "\n--- Optimal Page Replacement ---\n";

    for (size_t i = 0; i < pages.size(); ++i) {
        int page = pages[i];
        bool hit = find(frameList.begin(), frameList.end(), page) != frameList.end();

        cout << "Page " << page << " -> ";

        if (hit) {
            cout << "HIT\n";
            hits++;
        } else {
            cout << "FAULT";
            pageFaults++;

            if ((int)frameList.size() < frames) {
                frameList.push_back(page);
            } else {
                int farthest = -1, victim = frameList[0];
                for (int p : frameList) {
                    auto it = find(pages.begin() + i + 1, pages.end(), p);
                    if (it == pages.end()) {
                        victim = p;
                        break;
                    } else {
                        int distance = std::distance(pages.begin(), it);
                        if (distance > farthest) {
                            farthest = distance;
                            victim = p;
                        }
                    }
                }
                replace(frameList.begin(), frameList.end(), victim, page);
                cout << " (Replaced " << victim << ")";
            }
            cout << "\n";
        }

        if (stepMode) {
            showFrames(frameList);
            cout << "Press Enter to continue...";
            cin.ignore();
        }
    }

    double hitRatio = (double)hits / pages.size();
    return {pageFaults, hits, hitRatio};
}
