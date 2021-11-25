#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "process.h"
#include "linux_parser.h"

using std::string;
using std::to_string;
using std::vector;

// Process constructor
Process::Process(int processID)
{
    processID_ = processID;
}

/* Getter Functions */
// Return this process's ID
int Process::Pid() const
{ 
    return processID_; 
}

//Return this process's CPU utilization
float Process::CpuUtilization() const
{ 
    return LinuxParser::CpuUtilization(Pid());
}
// Return the command that generated this process
string Process::Command() 
{ 
    // Linux stores the command used to launch the function in the /proc/[pid]/cmdline file.
    return LinuxParser::Command(Pid());
}

// Return this process's memory utilization
string Process::Ram() 
{ 
    // Linux stores memory utilization for the process in /proc/[pid]/status.
    // in line after "Vmsize" convert from kb to mb -> devide by 1000
    return LinuxParser::Ram(Pid());
}

// Return the user (name) that generated this process
string Process::User() 
{ 
    // proc/[PID]/status -> First Integer of UID line 
    // in /etc/passwd we can find a line where a name is matched to the UID 
    return LinuxParser::User(Pid());
}

// Return the age of this process (in seconds)
long int Process::UpTime() 
{ 
    // we need to substract from systems uptime here because
    // because the process uptime is the time after the system boot
    return LinuxParser::UpTime() - LinuxParser::UpTime(Pid());   
}


// Overload the "less than" comparison operator for Process objects
bool Process::operator<(Process const& a)  const{ 

    // sort the processes according to their CPU utilization 
    // -> those functions that could potentially modify "a" have to be made const because "a" is const
    return  (a.CpuUtilization() < a.CpuUtilization());     
}