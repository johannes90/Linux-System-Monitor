#include <dirent.h>
#include <unistd.h>
#include <sstream>
#include <string>
#include <vector>
#include <iostream>
#include <cmath>
#include <iomanip> // for std::setprecision

#include "linux_parser.h" 

using std::stof;
using std::string;
using std::to_string;
using std::vector;

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == filterPrettyName) {
          /*PRETTY_NAME means an operating system name in a format suitable for presentation to the user. [...]  
           source: http://manpages.ubuntu.com/manpages/xenial/man5/os-release.5.html */
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, kernel, version;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

//TODO: New Question after review: ---------------------------------------------------
// I don't quite understand the 3. suggestion to close my filestreams (ifstream object) ?
// I think the filestreams are automatically closed after end of scope 
// source:  https://stackoverflow.com/questions/748014/do-i-need-to-manually-close-an-ifstream
// Or do you mean something else? I am happy to learn something new :) 


// Read and return the system memory utilization
float LinuxParser::MemoryUtilization() 
{ 
  //extract info from /proc/meminfo as with Kernel, Operating system before.
  // Total used memory = MemTotal - Memfree see 

  std::string line, key; //to scan the meminfo file
  float memTotal;
  float memFree;
  float val;    // store the values we are interested in

  std::ifstream filestream(kProcDirectory + kMeminfoFilename);

  if (filestream.is_open())
  {
    while (std::getline(filestream, line))
    {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      // search for MemTotal and MemFree
      while (linestream >> key >> val)
      {
        if (key == filterMemTotalString)
        {
          memTotal = val;
        }
        if (key == filterMemFreeString)
        {
          memFree = val;
          break;
        }
      }
    }
  }
  return (memTotal - memFree)/memTotal; // used memory in percentage
}

// Read and return the system uptime
long LinuxParser::UpTime() 
{ 
  long uptime;
  std::string line; 
  std::ifstream filestream(kProcDirectory + kUptimeFilename);

  if (filestream.is_open())
  {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    linestream >> uptime;
  }
  return uptime; 
}

// Read and return the number of jiffies for the system
long LinuxParser::Jiffies() 
{ 
  // Jiffies = all processor values -> [kuser:ksteal]    
  long jiffies = 0;
  std::vector<std::string> cpuUtilization = LinuxParser::CpuUtilization();
  for (int i = kUser_; i <= kSteal_; i++) {
    jiffies = jiffies + std::stol(cpuUtilization[i]);
  }
  return jiffies;
}

// Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid[[maybe_unused]]) { return 0; } // i don't need that helper function

// Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() 
{
  // Jiffies = idle + iowait ->     
  long idles = 0;
  std::vector<std::string> cpuUtilization = LinuxParser::CpuUtilization();
  for (int i = kIdle_; i <= kIOwait_; i++) {
    idles = idles + std::stol(cpuUtilization[i]);
  }
  return idles;
}

// Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() 
{ 
  // active Jiffies = all processor values - idle values  
  return Jiffies() - IdleJiffies(); 
}

// Read and return CPU utilization numbers in a vector
vector<string> LinuxParser::CpuUtilization() 
{ 
  string line;
  vector<string> cpuUtilizations;
  string value;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) 
    {
      std::istringstream linestream(line);
      // go through line an put all integers in vector with
      linestream >> value;
      if (value == filterCpu)
      {
        while(linestream >> value)
        {
          try{
            cpuUtilizations.push_back(value);
          }
          catch(...){}
        }
        return cpuUtilizations;
      }
    }
  }
  return {}; 
}

// Read and return the total number of processes
int LinuxParser::TotalProcesses() 
{ 
  // we can use basic structure from OS function
  // 1: Open file proc/stat
  // 2: Extract number after "processes"
  string line;
  string key;
  int value;
  std::ifstream filestream(kProcDirectory + kStatFilename);

  if (filestream.is_open()) {
    while (std::getline(filestream, line)) 
    {
    
      std::istringstream linestream(line);

      while (linestream >> key >> value) 
      {
        if (key == filterProcesses) 
        {
          return value;
        }
      }
    }
  }
  return 0.0;
}

// Read and return the number of running processes
int LinuxParser::RunningProcesses() 
{ 
  // we can use basic structure from OS function
  // 1: Open file proc/stat
  // 2: Extract number after "procs_running"
  string line;
  string key;
  int value;
  std::ifstream filestream(kProcDirectory + kStatFilename);

  if (filestream.is_open()) {
    while (std::getline(filestream, line)) 
    {
      std::istringstream linestream(line);

      while (linestream >> key >> value) 
      {
        if (key == filterRunningProcesses) 
        {
          return value;
        }
      }
    }
  }
  return 0.0;
}

// Read and return the command associated with a process
string LinuxParser::Command(int pid) 
{ 
  string command, line;
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kCmdlineFilename);
  if (stream.is_open()) 
  {

    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> command;
  }
  
  return command; 
}

//Read and return the memory used by a process
string LinuxParser::Ram(int pid)
{ 
  string ram;
  string line;
  string key;
  string value;
  std::ifstream filestream(kProcDirectory + std::to_string(pid) + kStatusFilename);
  if (filestream.is_open()) {

    while (std::getline(filestream, line)) 
    {
      std::istringstream  linestream(line);
      linestream >> key;
      if (key == filterProcMem) // I used VmRSS instead of VmSize (which is virtual memory size and therefore potentially more than the physical RAM size)
      // source: https://man7.org/linux/man-pages/man5/proc.5.html
      {
        linestream >> value;
        float mem = stof(value) / 1000.;
        // use a stream and setprecision to change the displayed precision of the value
        std::stringstream s;
        s << std::fixed << std::setprecision(2) << mem; 
        return s.str();
        
      } 
    }
  }
  return string();
}

// Read and return the user ID associated with a process
string LinuxParser::Uid(int pid)
{ 
  string line; 
  string key; 
  string uid;
  std::ifstream filestream(kProcDirectory + std::to_string(pid) + kStatusFilename);
  if (filestream.is_open()) 
  {
    while (std::getline(filestream, line)) 
    {
      std::istringstream  linestream(line);
      linestream >> key >> uid;
      if(key == filterUID)
      {
      return uid;
      }
      
    } 
  }
  return string(); 
}

//  Read and return the user associated with a process
// Not sure if correct because user = dumper ???
string LinuxParser::User(int pid)
{ 
  string line, user, password, uid;
  std::ifstream filestream(kPasswordPath);
  if (filestream.is_open()) 
  {
    while (std::getline(filestream, line))
    {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> user >> password >> uid) 
      {
        if (uid == LinuxParser::Uid(pid))
        {
          return user;
        }
      }
    }
  }
  return string();
}

// Read and return the uptime of a process
long LinuxParser::UpTime(int pid)
{ 
  float clocktime = sysconf(_SC_CLK_TCK); // frequence of clock ticks [1/s]

  string line, value;
  vector<string> states;
  std::ifstream filestream(kProcDirectory + std::to_string(pid) + kStatFilename);
  if (filestream.is_open()) 
  {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    // extract the 22th value (=uptime) 

    // review suggestion did not work for me, see comment down below
    /*std::string value2;
    while (linestream) {

        //linestream.seekg(21, std::ios_base::beg) >> value; // -> that seems to give me a wrong result or I made a mistake .. 
        linestream.seekg(22, std::ios_base::beg) >> value2;
        return stol(value) / clocktime;
      }
    */
    for (int i = 0; i < 21; i++)
    {
      linestream >> value;
    }
    // that is the 22th. value of the line = uptime 
    linestream >> value;
    
    // convert number of ticks to seconds
    return stol(value) / clocktime; //conversion from seconds to hh::mm::ss is done in the NcursesDisplay class with format function
  }

  return long(); //uptime; 
}

// compute CPU utilization, see also process.cpp
// calculation the cpu utilization using the computation in the following post
// https://stackoverflow.com/questions/16726779/how-do-i-get-the-total-cpu-usage-of-an-application-from-proc-pid-stat/16736599#16736599
float LinuxParser::CpuUtilization(int processID) 
{ 
  string line, statvalue;
  vector<string> statvalues;
  std::ifstream filestream(kProcDirectory + to_string(processID) + kStatFilename);

  long upTimeSystem = LinuxParser::UpTime();
  long uTime, sTime, cuTime, csTime, startTime, totalTime;  // values from /proc/[pid]/stat:
  float cpuUsage, seconds;
 
  if (filestream.is_open()) 
  {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    for( int i = 0; i < 22; i++)
    {
      linestream >> statvalue;
      statvalues.push_back(statvalue); 
    }
    uTime      = stol(statvalues[13]);
    sTime      = stol(statvalues[14]);
    startTime  = stol(statvalues[21]);
    cuTime     = stol(statvalues[15]);
    csTime     = stol(statvalues[16]);
    float Hertz = (float)sysconf(_SC_CLK_TCK);
    totalTime  = uTime + sTime + cuTime + csTime; 
    seconds  =(float) (upTimeSystem - (startTime / Hertz) ); //(startTime / Hertz = uptime(pid) ?! )

    // compute acutal cpu usage 
    if (seconds != 0){
      cpuUsage = (float) ( (totalTime / Hertz) / seconds);  
    }
    else
    {
      cpuUsage = 0.f;
    }
  }

  return cpuUsage;
}
