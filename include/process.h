#ifndef PROCESS_H
#define PROCESS_H

#include <string>
/*
Basic class for Process representation
It contains relevant attributes as shown below
*/
class Process {
 public:
  
  Process( int pid );
  int Pid();
  std::string User();
  std::string Command();
  void CalcCpuUtilization();
  std::string Ram();
  long int UpTime();
  bool IsValid();
  std::string State();
  float CpuUtilization() const;
  
  // Overload the "less than" comparison operator for Process objects
  //bool operator>(Process const& a) const;

  // private members
 private:
  int         pid_;
  float       cpu_utilization_;
};

#endif