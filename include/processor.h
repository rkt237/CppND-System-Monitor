#ifndef PROCESSOR_H
#define PROCESSOR_H

#include <string>

class Processor {
 public:

  Processor();
  Processor( std::string cpu_name );
  float Utilization();
  long  Jiffies();
  long  ActiveJiffies();
  long  IdleJiffies();

  std::string CpuName() const { return cpu_name_;};
  void setName( const std::string& str_name ) { cpu_name_ = str_name; };

 private:
    
    std::string   cpu_name_;

    unsigned long jiffies_;         // Total jiffies
    unsigned long active_jiffies_;  // NonIdle (active) Jiffies is user + nice + system_all
    unsigned long idle_jiffies_;    // Idle-Jiffies

};

#endif