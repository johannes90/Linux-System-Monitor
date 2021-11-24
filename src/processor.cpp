#include "processor.h"
#include "linux_parser.h"
#include <vector>
#include <string>



// Return the aggregate CPU utilization
// calculate aggregate utilization exactly as described here: 
// https://stackoverflow.com/questions/23367857/accurate-calculation-of-cpu-usage-given-in-percentage-in-linux
float Processor::Utilization() 
{ 
    // computing the aggregate utilization:
    std::vector<std::string> cpuUtilization = LinuxParser::CpuUtilization();

    float total   = (float)LinuxParser::Jiffies();
    float idle    = (float)LinuxParser::IdleJiffies();  

    //differentiate: actual values minus the previous one
    float totald = total - prevTotal;
    float idled = idle - prevIdle;
    
    float CPU_Percentage = (totald - idled)/totald;

    //update prev values
    prevTotal = total; 
    prevIdle  = idle;

    return CPU_Percentage;
}

//proc/stat

// numbers after cpu tag = clock ticks -> add those up and devide by total time that passed 
// we do not need individual cpu utilization (cpu0, cpu1, ...)
// more info on that in "11. Processor Data" 
