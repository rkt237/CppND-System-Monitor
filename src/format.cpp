#include <string>
#include <chrono>
#include <sstream>
#include <iomanip>
#include <iostream>

#include "format.h"

using std::string;

// helper function to change the time format
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
string Format::ElapsedTime( long int elapsed_time ) {

    std::string time;
    
    try
    {
        // store elapsed_time in the standard librarie chrono
        std::chrono::seconds elapsed_s (elapsed_time);

        // convert seconds to hours. 3600s --> 1h
        std::chrono::hours   elapsed_h ( std::chrono::duration_cast<std::chrono::hours>(elapsed_s) );

        // convert seconds to minutes 60s --> 1 min
        std::chrono::minutes elapsed_m ( std::chrono::duration_cast<std::chrono::minutes>(elapsed_s) );
        
        // hours
        int h = elapsed_h.count();

        // minutes
        int min = ( elapsed_m.count() % std::chrono::hours::period::num ) / std::chrono::minutes::period::num;

        // seconds = elapsed_time - 3600*h - 60*min
        int sec = ( elapsed_s.count() % std::chrono::hours::period::num ) % std::chrono::minutes::period::num;

        std::stringstream ss;
        ss << std::setw(2) << std::setfill('0') << h << ":" 
        << std::setw(2) << std::setfill('0') << min << ":" 
        << std::setw(2) << std::setfill('0') << sec;

        time = ss.str();

        return time;
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }

    return time;
}