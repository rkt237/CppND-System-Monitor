#include <vector>
#include <iostream>

#include "linux_parser.h"
#include "processor.h"

Processor::Processor() : jiffies_(0), active_jiffies_(0), idle_jiffies_(0) {
    cpu_name_ = "cpu";
}

Processor::Processor( std::string cpu_name )
: jiffies_(0), active_jiffies_(0), idle_jiffies_(0) {
    
    if ( cpu_name.empty() )
        cpu_name_ = "cpu";
    else
        cpu_name_ = cpu_name;
}

long Processor::Jiffies() {
    return jiffies_;
}

long Processor::ActiveJiffies() {
    return active_jiffies_;
}

long Processor::IdleJiffies() {
    return idle_jiffies_;
}

// Return the aggregate CPU utilization
float Processor::Utilization()  { 

    float cpu_utilization = 0.0;
    try
    {
        // Idle-Jiffies is idle + iowait
        idle_jiffies_    = LinuxParser::IdleJiffies( cpu_name_ );

        // NonIdle (active) Jiffies is user + nice + system_all
        long active = LinuxParser::ActiveJiffies( cpu_name_ );

        // total jiffies
        long Total   = LinuxParser::Jiffies( cpu_name_ );

        // differentiate: actual value minus the previous one
        long total_d = Total - jiffies_;
        long active_d  = active - active_jiffies_;

        // measurement interval = active time units / total time units
        if ( total_d > 0 )
            cpu_utilization = active_d/total_d;
        else
            cpu_utilization = 0;

        // store the new value
        jiffies_        = Total;
        active_jiffies_ = active;

        return cpu_utilization;
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }

    return cpu_utilization;
}