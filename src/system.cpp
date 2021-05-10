#include <unistd.h>
#include <cstddef>
#include <set>
#include <string>
#include <vector>
#include <functional>
#include <algorithm>
#include <iostream>

#include "linux_parser.h"
#include "process.h"
#include "processor.h"
#include "system.h"

using std::set;
using std::size_t;
using std::string;
using std::vector;

// Contructor
System::System() : time_(0) {
    Init();
}

void System::Init () {
    kernel_             = LinuxParser::Kernel();
    operating_system_   = LinuxParser::OperatingSystem();
}

// Return the system's CPU
Processor& System::Cpu() { return cpu_; }

// Return a container composed of the system's processes
vector<Process>& System::Processes() { 
    
    try
    {
        // list of current processes
        std::vector<int> pid_list = LinuxParser::Pids();

#if 0
        // list of existing process-id
        std::set<int> existing_process;

        // set the existing processes in the list
        for ( Process proc : processes_ ) {
            existing_process.insert( proc.Pid() );
        }

        for ( int pid : pid_list ) {
            // put the new pid on the list
            if ( existing_process.find(pid) == existing_process.end() )
                processes_.emplace_back(pid); // add a new Process to the list
        }

        // remove zombie and dead process
        RemoveInvalidProcess();


        // read the cpu-data for the updated process-list
        for ( Process proc : processes_ )
            proc.CalcCpuUtilization();
#endif

        // clear the vector
        processes_.clear();
        for ( auto pid : pid_list ) {
            
            Process proc(pid);
            
            // remove zombie, dead process or process with empty command 
            if ( proc.IsValid() )  {
                
                // read the cpu-data for the updated process-list
                proc.CalcCpuUtilization();

                // build the process vector
                processes_.push_back(proc);
            }
        }

        // sort the processes from biggest to smallest CpuUtilization
        std::sort(processes_.begin(), processes_.end(), 
        [](const Process& a, const Process& b) { return ( a.CpuUtilization() - b.CpuUtilization() ) > 0.0001; } );

        return processes_; 
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    
    return processes_;
}

// Return the system's kernel identifier (string)
std::string System::Kernel() { return kernel_; }

// Return the system's memory utilization
float System::MemoryUtilization() { return LinuxParser::MemoryUtilization(); }

// Return the operating system name
std::string System::OperatingSystem() { return operating_system_; }

// Return the number of processes actively running on the system
int System::RunningProcesses() { return LinuxParser::RunningProcesses(); }

// Return the total number of processes on the system
int System::TotalProcesses() { return LinuxParser::TotalProcesses(); }

// Return the number of seconds since the system started running
long int System::UpTime() { 
    
    time_ = LinuxParser::UpTime();
    return time_;
}

void System::RemoveInvalidProcess() {

    std::vector<Process> existing_process = processes_;
    for ( Process proc : processes_ ) { 
        existing_process.push_back(proc); 
    }

    processes_.clear();
    for ( Process proc : existing_process ) {
        if ( proc.IsValid() )
            processes_.push_back(proc);
    }
}