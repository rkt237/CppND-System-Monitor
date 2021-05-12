#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>
#include <iostream>

#include "linux_parser.h"
#include "process.h"

using std::string;
using std::to_string;
using std::vector;

Process::Process( int pid ) 
: pid_(pid) {

    CalcCpuUtilization();
}

// Return this process's ID
int Process::Pid() { return pid_; }

//bool Process::operator>(Process const& a) const { 
//    return cpu_utilization_ > a.cpu_utilization_; 
//};

// Set this process's CPU utilization
void Process::CalcCpuUtilization() {

    cpu_utilization_ = 0.0;
    try
    {
        std::vector<std::string> proc_data = LinuxParser::readStatFile( pid_ );
        if( proc_data.size() >= 21 )
        {
            long utime     = stol( proc_data[13] );
            long stime     = stol( proc_data[14] );
            long cutime    = stol( proc_data[15] );
            long cstime    = stol( proc_data[16] );
            long starttime = stol( proc_data[21] );

            long total_time = utime + stime + cutime + cstime;

            long sys_conf = sysconf(_SC_CLK_TCK);

            float seconds = float(LinuxParser::UpTime()) - float(starttime / sys_conf);

            if ( seconds && sys_conf )
                cpu_utilization_ = float(total_time / sys_conf) / float(seconds);
        }
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
}

// Return this process's CPU utilization
float Process::CpuUtilization() const { 
    return cpu_utilization_; 
}

// Return the command that generated this process
string Process::Command() { 
    std::string command = LinuxParser::Command(pid_);
    if ( command.size() > 50 )
        return command.substr(0, 50) + "...";
    else
        return command;
 }

// Return this process's memory utilization
string Process::Ram() { 
    return LinuxParser::Ram( pid_ ); 
}

// Return the user (name) that generated this process
string Process::User() { 
    return LinuxParser::User(pid_); 
}

// Return the age of this process (in seconds)
long int Process::UpTime() { 
    return LinuxParser::UpTime(pid_);
}

// Return the process's state 
 string Process::State() {
    return LinuxParser::State(pid_);
 }

// Return false if the state of this process is X, x or Z
 bool Process::IsValid() {
    
    bool valid = false; 
    std::string state = LinuxParser::State(pid_);

    if ( state.size() >= 1 ) {

        char c_state = state[0];

        switch (c_state)  {

            case 'X':
            case 'x':
            case 'Z':
                valid = false;
                break;
            
            default:
                valid = true;
                break;
        }
    }

    //remove also process with empty command 
    std::string command = Command();

    return ( valid && !(command.empty()) );
 }