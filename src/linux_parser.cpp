#include <dirent.h>
#include <unistd.h>
#include <sstream>
#include <string>
#include <vector>
#include <stdexcept>
#include <iostream>

#include "linux_parser.h"

using std::stof;
using std::stol;
using std::stoul;
using std::stoi;
using std::string;
using std::to_string;
using std::vector;

// read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  try
  {
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
  catch(const std::exception& e)
  {
    std::cerr << e.what() << '\n';
  }
  
  return value;
}

// read data from the filesystem
string LinuxParser::Kernel() {
  string os, version, kernel;
  try
  {
    string line;
    std::ifstream stream(kProcDirectory + kVersionFilename);
    if (stream.is_open()) {
      std::getline(stream, line);
      std::istringstream linestream(line);
      linestream >> os >> version >> kernel;
    }
    return kernel;
  }
  catch(const std::exception& e)
  {
    std::cerr << e.what() << '\n';
  }
  return kernel;
}

// Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  try
  {
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
  catch(const std::exception& e)
  {
    std::cerr << e.what() << '\n';
  }
  return pids;
}

// Read and return the system memory utilization
float LinuxParser::MemoryUtilization() 
{ 
  int mem = 0;
  try
  {
    int totalMem, freeMem, buffersMem, cachedMem;
    //int availableMem;

    std::string line;

    // get and read the /proc/meminfo file
    std::ifstream of ( kProcDirectory + kMeminfoFilename );

    if ( of.is_open() )
    {
      // extract and store line by line until the end of the file
      while ( std::getline( of, line ) )
      {
        // replace : with blank character
        std::replace( line.begin(), line.end(), ':', ' ' );

        // split line
        std::istringstream ss ( line );

        std::string key;
        std::string str_value;
        long l_value = 0;

        // get key and value
        while ( ss >> key >> str_value ) 
        {

          l_value = stol(str_value);

          if ( key == "MemTotal" )
            totalMem = l_value;

          if ( key == "MemFree" )
            freeMem = l_value;

          //if ( key == "MemAvailable" )
          //  mem_available = i_value;

          if ( key == "Buffers" )
            buffersMem = l_value;

          if ( key == "Cached" )
            cachedMem = l_value;
        }
      }
    }

    if ( totalMem == 0 )
      throw std::invalid_argument("argument out of bounds");
    else
      mem = ( totalMem - (freeMem + buffersMem + cachedMem) )/ totalMem;

    return mem;
  }
  catch(const std::exception& e)
  {
    std::cerr << e.what() << '\n';
  }

  return mem;
}

// Read and return the system uptime
long LinuxParser::UpTime() { 
  
  std::string str_utime;
  //std::string str_utime, str_stime;

  string line;
  long l_utime = 0;
  try
  {
    // get and read the /proc/stat filename
    std::ifstream stream(kProcDirectory + kUptimeFilename);
    
    if (stream.is_open()) 
    {
      std::getline(stream, line);

      // split line
      std::istringstream linestream(line);

      // linestream >> str_utime >> str_stime;

      linestream >> str_utime;
    }

    long l_utime = stol(str_utime);
    //long l_stime = stol(str_stime);

    return l_utime;
  }
  catch(const std::exception& e)
  {
    std::cerr << e.what() << '\n';
  }

  return l_utime;
}

// Read and return the number of jiffies for the system
long LinuxParser::Jiffies( std::string cpu_name ) { 
  
  LinuxParser::CPUData cpu = LinuxParser::CpuUtilization( cpu_name );

  return cpu.User_ + cpu.Nice_ + cpu.System_ + cpu.IRQ_ + cpu.SoftIRQ_ + cpu.Idle_ + cpu.IOwait_ + cpu.Steal_; 
}

// Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies( std::string cpu_name ) { 
  
  LinuxParser::CPUData cpu = LinuxParser::CpuUtilization( cpu_name );

  return cpu.User_ + cpu.Nice_ + cpu.System_ + cpu.IRQ_ + cpu.SoftIRQ_;
}

// Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies( std::string cpu_name ) { 
  
  LinuxParser::CPUData cpu = LinuxParser::CpuUtilization( cpu_name );

  return cpu.Idle_ + cpu.IOwait_; 
}

// Read and return CPU utilization
LinuxParser::CPUData LinuxParser::CpuUtilization( std::string cpu_name ) { 
  
  LinuxParser::CPUData cpu;
  try
  {
    string line;

    // get and read the /proc/stat filename
    std::ifstream stream(kProcDirectory + kStatFilename);
    
    if (stream.is_open()) 
    {
      // extract and store line by line until the end of the file
      while ( std::getline( stream, line ) )
      {
        // split line
        std::istringstream linestream(line);

        string key;
        std::string str_value;
        
        linestream >> key;
        
        if ( key.compare(cpu_name) == 0 )
        {
          int count = 0;

          // cpu_utilization --> 0 user 1 nice 2 system 3 idle 4 ioWait 5 irq 6 softIrq 7 steal 8 guest 9 guest_nice
          while ( linestream >> str_value )
          {
            unsigned long lu_value = stoul(str_value);
            switch ( count )
            {
              case kUser_ :
                cpu.User_ = lu_value;
                break;
              case kNice_ :
                cpu.Nice_ = lu_value;
                break;
              case kSystem_ :
                cpu.System_ = lu_value;
                break;
              case kIdle_ :
                cpu.Idle_ = lu_value;
                break;
              case kIOwait_ :
                cpu.IOwait_ = lu_value;
                break;
              case kIRQ_ :
                cpu.IRQ_ = lu_value;
                break;
              case kSoftIRQ_ :
                cpu.SoftIRQ_ = lu_value;
                break;
              case kSteal_ :
                cpu.Steal_ = lu_value;
                break;
              case kGuest_ :
                cpu.Guest_ = lu_value;
                break;
              case kGuestNice_ :
                cpu.GuestNice_ = lu_value;
                break;
            }
            ++count;
          }
          break;
        }
      }
    }

    return cpu;
  }
  catch(const std::exception& e)
  {
    std::cerr << e.what() << '\n';
  }
  return cpu;
}

// Read and return the total number of processes
int LinuxParser::TotalProcesses() 
{ 
  int total_proc = 0;
  try
  {
    std::string line;
  
    // get and read the /proc/stat filename
    std::ifstream of ( kProcDirectory + kStatFilename );

    if ( of.is_open() )
    {
      // extract and store line by line until the end of the file
      while ( std::getline( of, line ) )
      {
        // split line
        std::istringstream ss ( line );

        std::string key;
        std::string str_value;
        int i_value = 0;

        // get key and value
        while ( ss >> key >> str_value ) {

          i_value = stoi(str_value);

          if ( key == "processes" )
            total_proc = i_value;
        }
      }
    }

    return total_proc; 
  }
  catch(const std::exception& e)
  {
    std::cerr << e.what() << '\n';
  }
  return total_proc;
}

// Read and return the number of running processes
int LinuxParser::RunningProcesses() { 
  
  int procs_running = 0;
  try
  {
    std::string line;

    // get and read the /proc/stat filename
    std::ifstream of ( kProcDirectory + kStatFilename );

    if ( of.is_open() )
    {
      // extract and store line by line until the end of the file
      while ( std::getline( of, line ) )
      {
        // split line
        std::istringstream ss ( line );

        std::string key;
        std::string str_value;
        int i_value = 0;

        // get key and value
        while ( ss >> key >> str_value ) {

          i_value = stoi(str_value);

          if ( key == "procs_running" )
            procs_running = i_value;
        }
      }
    }

    return procs_running;
  }
  catch(const std::exception& e)
  {
    std::cerr << e.what() << '\n';
  }
  return procs_running; 
}

// Read and return the command associated with a process
string LinuxParser::Command( int pid ) { 
  
  string line;

  // get and read the /proc/stat filename
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kCmdlineFilename);
  
  if (stream.is_open()) 
    std::getline(stream, line);

  return line; 
}

// Read and return the memory used by a process
string LinuxParser::Ram( int pid ) {

  std::string line, ram;

  // get and read the /proc/stat filename
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatusFilename);

  if ( stream.is_open() )
  {
    // extract and store line by line until the end of the file
    while ( std::getline( stream, line ) )
    {
      // replace : with blank character
      std::replace( line.begin(), line.end(), ':', ' ' );

      // split line
      std::istringstream ss ( line );

      std::string key, str_value;
      long l_value = 0;

      // get key and value
      while ( ss >> key >> str_value ) {

        if ( key == "VmSize" ) {
          // calculate the ram in MB
          l_value = stoi(str_value) / 1024;

          ram = std::to_string(l_value);
        }
      }
    }
  }
  return ram; 
}

// Read and return the user ID associated with a process
string LinuxParser::Uid( int pid ) {
  
  std::string line, key, str_value, uid;

  // get and read the /proc/stat filename
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatusFilename);

  if ( stream.is_open() )
  {
    // extract and store line by line until the end of the file
    while ( std::getline( stream, line ) )
    {
      // replace : with blank character
      std::replace( line.begin(), line.end(), ':', ' ' );

      // split line
      std::istringstream ss ( line );

      // get key and value
      while ( ss >> key >> str_value ) {
        if ( key == "Uid" ) {
          uid = str_value;
        }
      }
    }
  }

  return uid;
}

// Read and return the user associated with a process
string LinuxParser::User( int pid ) { 
  
  std::string line, name, x, str_value, user, uid1, uid2;
  
  std::string uid = LinuxParser::Uid( pid );

  // get and read the /proc/stat filename
  std::ifstream stream(kPasswordPath);

  if ( stream.is_open() )
  {
    // extract and store line by line until the end of the file
    while ( std::getline( stream, line ) )
    {
      // replace : with blank character
      std::replace( line.begin(), line.end(), ':', ' ' );

      // split line
      std::istringstream ss ( line );

      // get key and value
      while ( ss >> name >> x >> uid1 >> uid2 ) {
        if ( uid.compare( uid1 ) == 0 ) {
          user = name;
          break;
        }
      }
      
    }
  }
  return user;
}

// Read and return the uptime of a process
long LinuxParser::UpTime( int pid ) { 
  
  long uptime = 0;
  try
  {
    std::vector<std::string> proc_data = readStatFile( pid );

    long sys_conf = sysconf(_SC_CLK_TCK);

    if ( sys_conf && (proc_data.size()>= 21) )
      uptime =  LinuxParser::UpTime() - stol( proc_data[21] )/  sys_conf;

    return uptime;
  }
  catch(const std::exception& e)
  {
    std::cerr << e.what() << '\n';
  }  
  return uptime;
}

std::vector<std::string> LinuxParser::readStatFile ( const int pid ) {
  
  std::string line, str_value;
  std::vector<std::string> procData;

  // get and read the /proc/stat filename
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatFilename);

  if ( stream.is_open() )
  {
    // extract and store line by line until the end of the file
    while ( std::getline( stream, line ) )
    {
      // split line
      std::istringstream ss ( line );

      // get key and value
      while ( ss >> str_value ) {
        procData.push_back(str_value);
      }
    }
  }

  return procData;
}

// Read and return the state associated with a process
string LinuxParser::State( int pid ) {
  
  std::string line, key, str_value, state;

  // get and read the /proc/stat filename
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatusFilename);

  if ( stream.is_open() )
  {
    // extract and store line by line until the end of the file
    while ( std::getline( stream, line ) )
    {
      // replace : with blank character
      std::replace( line.begin(), line.end(), ':', ' ' );

      // split line
      std::istringstream ss ( line );

      // get key and value
      while ( ss >> key >> str_value ) {
        if ( key == "State" ) {
          state = str_value;
        }
      }
    }
  }

  return state;
}
