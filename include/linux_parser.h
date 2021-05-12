#ifndef SYSTEM_PARSER_H
#define SYSTEM_PARSER_H

#include <fstream>
#include <regex>
#include <string>
#include <unistd.h>

#define printVariableNameAndValue(x) std::cout << "Name of variable **" <<(#x)<<"** and the value of variable is => "<<x<<"\n"

namespace LinuxParser {

// Paths
const std::string kProcDirectory{"/proc/"};
const std::string kCmdlineFilename{"/cmdline"};
const std::string kCpuinfoFilename{"/cpuinfo"};
const std::string kStatusFilename{"/status"};
const std::string kStatFilename{"/stat"};
const std::string kUptimeFilename{"/uptime"};
const std::string kMeminfoFilename{"/meminfo"};
const std::string kVersionFilename{"/version"};
const std::string kOSPath{"/etc/os-release"};
const std::string kPasswordPath{"/etc/passwd"};

// System
float MemoryUtilization();
long UpTime();
std::vector<int> Pids();
int TotalProcesses();
int RunningProcesses();
std::string OperatingSystem();
std::string Kernel();

// cpu datastruct
struct CPUData {
  long User_;
  long Nice_;
  long System_;
  long Idle_;
  long IOwait_;
  long IRQ_;
  long SoftIRQ_;
  long Steal_;
  long Guest_;
  long GuestNice_;
};

// CPU
enum CPUStates {
  kUser_ = 0,
  kNice_,
  kSystem_,
  kIdle_,
  kIOwait_,
  kIRQ_,
  kSoftIRQ_,
  kSteal_,
  kGuest_,
  kGuestNice_
};

CPUData CpuUtilization(std::string cpu_name);
long Jiffies( std::string cpu_name );
long ActiveJiffies( std::string cpu_name );
long IdleJiffies( std::string cpu_name );

// Processes
std::string Command(int pid);
std::string Ram(int pid);
std::string Uid(int pid);
std::string User(int pid);
long int UpTime(int pid);
std::string State( int pid );

std::vector<std::string> readStatFile ( const int pid );
};  // namespace LinuxParser

#endif