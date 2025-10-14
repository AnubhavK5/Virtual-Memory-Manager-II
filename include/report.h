#ifndef REPORT_H
#define REPORT_H

#include <vector>
#include <string>
#include "vmm.h"

void generateReport(int frames, const std::vector<int>& pages,
                    const Result& fifoRes, const Result& lruRes, const Result& optRes);
void generateCSV(const Result& fifoRes, const Result& lruRes, const Result& optRes);

#endif
