#include <dirent.h>
#include <unistd.h>
#include <sstream>
#include <string>
#include <vector>
#include <iostream>
#include <cmath>

#include "linux_parser.h" 
// QUESTION: --------------------------------------------------------------------------------------------------------------------------------------------
//that header is marked as "fatal error: linux_parser.h: No such file or directory"
// the monitor exe runs anyway and everything works, I can also debug the code on my machine with VSCode
// Why is there an error and how can i get rid of it? 
// QUESTION: --------------------------------------------------------------------------------------------------------------------------------------------

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
      // QUESTION: --------------------------------------------------------------------------------------------------------------------------------------------
      // QUESTION more elegant way to extract numbers ?
      // QUESTION: --------------------------------------------------------------------------------------------------------------------------------------------
      linestream >> value;
      if (value == "cpu")
      {
        while(linestream >> value)
        {
          //std::cout << value << std::endl;
          try{
            int _ = stoi(value); // only to ommit non integer values
            cpuUtilizations.push_back(value);
          }
          catch(...){}// we just care for all numbers after "cpu"}
          
        }
      }
    }
  }


  return cpuUtilizations; 
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
        if (key == "processes") 
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
        if (key == "procs_running") 
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
  string ram, line, key, value;
  std::ifstream filestream(kProcDirectory + std::to_string(pid) + kStatusFilename);
  if (filestream.is_open()) {

    while (std::getline(filestream, line)) 
    {
      std::istringstream  linestream(line);
      linestream >> key;
      if (key == "VmSize:")
      {
        linestream >> value;
        ram = std::to_string(  round(stof(value)/100)/10 ); // convert from kb to mb
        // QUESTION: -------------------------------------------------------------------------------------------------------------------------------------------- 
        //how do i round to x decimal places and also only display theese (output is xxxx.0000000)in c++ ? 
        // QUESTION: --------------------------------------------------------------------------------------------------------------------------------------------
        break;
      } 
    }
  }
  return ram; 
}

// Read and return the user ID associated with a process
string LinuxParser::Uid(int pid)
{ 
  string line, key, uid;
  std::ifstream filestream(kProcDirectory + std::to_string(pid) + kStatusFilename);
  if (filestream.is_open()) 
  {
    while (std::getline(filestream, line)) 
    {
      std::istringstream  linestream(line);
      linestream >> key >> uid;
      if(key == "Uid:")
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
  long uptime;
  float clocktime = sysconf(_SC_CLK_TCK); // frequence of clock ticks [1/s]

  string line, value;
  vector<string> states;
  std::ifstream filestream(kProcDirectory + std::to_string(pid) + kStatFilename);
  if (filestream.is_open()) 
  {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    // extract the 22th value (=uptime) 
    // QUESTION: --------------------------------------------------------------------------------------------------------------------------------------------
    // QUESTION: What is a more elegant way of discarding the first N values of a stream???
    // QUESTION: --------------------------------------------------------------------------------------------------------------------------------------------
    for (int i = 0; i < 21; i++)
    {
      linestream >> value;
    }
    // that is the 22th. value of the line = uptime 
    linestream >> value;

    // convert number of ticks to seconds
    uptime = stol(value) / clocktime;
  }

  return uptime; //conversion from seconds to hh::mm::ss is done in the NcursesDisplay class with format function
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
      cpuUsage = (float) 100 * ( (totalTime / Hertz) / seconds);  
    }
    else
    {
      cpuUsage = 0.f;
    }
  }

  return cpuUsage;
}
