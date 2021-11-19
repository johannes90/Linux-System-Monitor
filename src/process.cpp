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
// TODO: Return this process's ID
int Process::Pid() 
{ 
    return processID_; 
}

// TODO: Return this process's CPU utilization
float Process::CpuUtilization() 
{ 
    // Linux stores the CPU utilization of a process in the /proc/[PID]/stat file.
    // The "/proc/[pid]/stat" section of the proc man page describes the meaning of the values in this file. 
    // look up "13.Process Data"
    //TODO: handle that inside of parser
    //return 0;
    return LinuxParser::CpuUtilization(Pid());
}
// TODO: Return the command that generated this process
string Process::Command() 
{ 
    // Linux stores the command used to launch the function in the /proc/[pid]/cmdline file.
    //return string(); 
    return LinuxParser::Command(Pid());
}

// TODO: Return this process's memory utilization
string Process::Ram() 
{ 
    // Linux stores memory utilization for the process in /proc/[pid]/status.
    // in line after "Vmsize" convert from kb to mb -> devide by 1000
    //return string();
    return LinuxParser::Ram(Pid());
}



// TODO: Return the user (name) that generated this process
string Process::User() 
{ 
    // proc/[PID]/status -> First Integer of UID line 
    // in /etc/passwd we can find a line where a name is matched to the UID 
    //return string(); 
    return LinuxParser::User(Pid());
}


// TODO: Return the age of this process (in seconds)
long int Process::UpTime() 
{ 
    // Linux stores the process up time in /proc/[pid]/stat.
    // The "/proc/[pid]/stat" section of the proc man page (*link*) describes each of the values in this file. 
    // *link* https://man7.org/linux/man-pages/man5/proc.5.html

    // divide the "clock ticks" value by sysconf(_SC_CLK_TCK)

    // Once you have converted the time value to seconds, 
    // you can use the Format::Time() function from the project starter code to display the seconds in a "HH:MM:SS" format.
    //return 0;
    return LinuxParser::UpTime(Pid()); 
}




// TODO: Overload the "less than" comparison operator for Process objects
// REMOVE: [[maybe_unused]] once you define the function
bool Process::operator<(Process const& a[[maybe_unused]]) const { return true; }