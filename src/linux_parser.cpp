#include <dirent.h>
#include <unistd.h>
#include <sstream>
#include <string>
#include <vector>
#include <iostream>

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
        if (key == "PRETTY_NAME") {
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

// Read and return the system memory utilization
float LinuxParser::MemoryUtilization() 
{ 
  //extract info from /proc/meminfo as with Kernel, Operating system before.
  // Total used memory = MemTotal - Memfree see 

  std::string line, key; //to scan the meminfo file
  float memTotal, memFree, val;    // store the values we are interested in

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
          if (key == "MemTotal")
          {
            memTotal = val;
          }
          if (key == "MemFree")
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
  //long idletime, test;
  std::string line; //to scan the uptime file

  std::ifstream filestream(kProcDirectory + kUptimeFilename);

  if (filestream.is_open())
  {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    linestream >> uptime;
  }
  return uptime; 
}

// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() { return 0; }

// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid[[maybe_unused]]) { return 0; }

// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() 
{ 
  return Jiffies() - IdleJiffies(); 
}

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() { return 0; }

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
      // QUESTION more elegant way to extract numbers ?
      linestream >> value;
      if (value == "cpu")
      {
        while(linestream >> value)
        {
          std::cout << value << std::endl;
          try{
            std::cout << stoi(value);
            cpuUtilizations.push_back(value);
          }
          catch(...){}// we just care for all numbers after "cpu"}
          
        }
      }
    }
  }


  return cpuUtilizations; 
}
// TODO: Read and return the total number of processes
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
        if (key == "processes") 
        {
          return value;
        }
      }
    }
  }
  return 0.0;
}

// TODO: Read and return the number of running processes
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
        if (key == "procs_running") 
        {
          return value;
        }
      }
    }
  }
  return 0.0;
}

// TODO: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Command(int pid[[maybe_unused]]) { return string(); }

// TODO: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid[[maybe_unused]]) { return string(); }

// TODO: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid[[maybe_unused]]) { return string(); }

// TODO: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid[[maybe_unused]]) { return string(); }

// TODO: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid[[maybe_unused]]) { return 0; }

//TODO: compute CPU utilization, see also process.cpp
float LinuxParser::CpuUtilization(int processID)
{
  std::cout << processID; 
  return 0.0;
}
