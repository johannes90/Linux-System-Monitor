#include "processor.h"
#include "linux_parser.h"
#include <vector>
#include <string>




// TODO: Return the aggregate CPU utilization
float Processor::Utilization() 
{ 
    // computing the aggregate utilization:
    std::vector<std::string> cpuUtilization = LinuxParser::CpuUtilization();

    // calculate aggregate utilization as described here: 
    //https://stackoverflow.com/questions/23367857/accurate-calculation-of-cpu-usage-given-in-percentage-in-linux
    int user = stoi(cpuUtilization[0]);
    int nice = stoi(cpuUtilization[1]);
    int system = stoi(cpuUtilization[2]);
    int idle = stoi(cpuUtilization[3]);
    int iowait = stoi(cpuUtilization[4]);
    int irq = stoi(cpuUtilization[5]);
    int softirq = stoi(cpuUtilization[6]);
    int steal = stoi(cpuUtilization[7]);
    int guest = stoi(cpuUtilization[8]);
    int guest_nice = stoi(cpuUtilization[9]);

    //TODO: what are the prev values ?
    //prevIdle  = previdle +previowait
    idle        = idle + iowait;

    // int prevNonIdle = prevuser + prevnice + prevsystem + previrq + prevsoftirq + prevsteal;
    int nonIdle = user + nice + system + irq + softirq + steal;

    // int prevTotal = prevIdle + prevNonIdle;
    int total = idle + nonIdle;
    
    return 0.0;

}

//proc/stat

// numbers after cpu tag = clock ticks -> add those up and devide by total time that passed 
// we do not need individual cpu utilization (cpu0, cpu1, ...)
// more info on that in "11. Processor Data" 
