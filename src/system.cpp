#include <unistd.h>
#include <cstddef>
#include <set>
#include <string>
#include <vector>

#include "process.h"
#include "processor.h"
#include "system.h"
#include "linux_parser.h"

using std::set;
using std::size_t;
using std::string;
using std::vector;
/*You need to complete the mentioned TODOs in order to satisfy the rubric criteria "The student will be able to extract and display basic data about the system."

You need to properly format the uptime. Refer to the comments mentioned in format. cpp for formatting the uptime.*/

// the System class has a lot of ACCESSOR functions 
// Because we have the linux parser class which parses the data needed inside the systems class basically all
// we do here is call the corresponding functions from the parser

//  -> Return the system's CPU 
Processor& System::Cpu() 
{ 
    return cpu_; 
}

// Return a container composed of the system's processes
vector<Process>& System::Processes() 
{ 
    // use the parser to get the process IDs 
    vector<int> processIDs = LinuxParser::Pids();

    //std::vector<Process> processes_ is a private member of system class
    // loop through all process IDS and initialize Process objects for every ID 
    for(auto process : processIDs) {

    processes_.push_back(Process(process));
    }
    std::sort(processes_.begin(), processes_.end());
    return processes_;  
}


// Return the system's kernel identifier
std::string System::Kernel() 
{
    // information about kernel exists in proc/kernel 
    return LinuxParser::Kernel();
}

// Return the system's memory utilization
float System::MemoryUtilization() 
{  
    // proc/meminfo
    // check stackoverflow answer about how htop calculates memory utilization from the data in /proc/meminfo
    // Total used memory = MemTotal - MemFree
    return LinuxParser::MemoryUtilization();
}

// Return the operating system name
std::string System::OperatingSystem() 
{ 
    return LinuxParser::OperatingSystem();
    // Information about the operating system exists outside of the /proc directory, in the /etc/os-release
}

// Return the number of processes actively running on the system
int System::RunningProcesses() 
{ 
    // proc/stat
    return LinuxParser::RunningProcesses(); 
}



// Return the total number of processes on the system
int System::TotalProcesses() 
{ 
    // Information about the total number of processes on the system exists in the  proc/stat
    return LinuxParser::TotalProcesses(); 
}

// Return the number of seconds since the system started running
long int System::UpTime() 
{ 
    // proc/uptime
    return LinuxParser::UpTime();
}