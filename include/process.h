#ifndef PROCESS_H
#define PROCESS_H

#include <string>
/*
Basic class for Process representation
It contains relevant attributes as shown below
*/
class Process {
 public:
    // class constructor 
    Process(int processID);

  int Pid() const;                               // is const because of the operator overloading function
  std::string User();                     
  std::string Command();                  
  float CpuUtilization() const;                  // is const because of the operator overloading function      
  std::string Ram();                       
  long int UpTime();                       
  bool operator<(Process const& a) const;    

  // Declare any necessary private members
 private:
    // the ID itself
    int processID_; 
};

#endif