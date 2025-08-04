#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>
#include <iostream>

#include "process.h"
#include "linux_parser.h"

using std::string;
using std::to_string;
using std::vector;

Process::Process(int pid)
    :mPid{pid},
    mUser{LinuxParser::User(mPid)},
    mCommand{LinuxParser::Command(mPid)},
    mRam{LinuxParser::Ram(mPid)},
    mCpuUtilization{0.0},
    mUpTime{LinuxParser::UpTime(mPid)}
{
    calculateCPUUtilization();
}

// TODO: Return this process's ID
int Process::Pid() { return mPid; }

// TODO: Return this process's CPU utilization
float Process::CpuUtilization() const
{ 
  return mCpuUtilization; 
}

// TODO: Return the command that generated this process
string Process::Command() { return mCommand; }

// TODO: Return this process's memory utilization
string Process::Ram() 
{
    string ram = LinuxParser::Ram(mPid); // unit is KB
    // Convert to MB
    try
    {
        ram = std::to_string(stol(ram) / 1024);
    }
    catch(const std::exception& e)
    {
        ram = "0";
    }
    return ram;
}

// TODO: Return the user (name) that generated this process
string Process::User() { return mUser; }

// TODO: Return the age of this process (in seconds)
long int Process::UpTime() { return LinuxParser::UpTime(mPid); }

// TODO: Overload the "less than" comparison operator for Process objects
// REMOVE: [[maybe_unused]] once you define the function
bool Process::operator<(Process const& a) const 
{ 
    return this->CpuUtilization() > a.CpuUtilization(); 
}

void Process::calculateCPUUtilization() 
{ 
    long uptime = LinuxParser::UpTime();
    vector<float> val = LinuxParser::CpuUtilization(Pid());
    if (val.size() == 5) 
    {
        float totaltime = 0;
        for(int i=0; i<4; i++)
        {
            totaltime += val[i];
        }
        float seconds = uptime - val[4];
        mCpuUtilization = totaltime / seconds;
    } 
    else
    {
        mCpuUtilization = 0;
    }
}