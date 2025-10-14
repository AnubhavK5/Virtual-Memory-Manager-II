#ifndef VMM_H
#define VMM_H

#include <iostream>
#include <vector>
#include <unordered_map>
#include <queue>
#include <algorithm>
#include <iomanip>
#include <fstream>
#include <set>

struct Result {
    int pageFaults;
    int hits;
    double hitRatio;
};

Result fifo(const std::vector<int>& pages, int frames, bool stepMode);
Result lru(const std::vector<int>& pages, int frames, bool stepMode);
Result optimal(const std::vector<int>& pages, int frames, bool stepMode);

#endif
