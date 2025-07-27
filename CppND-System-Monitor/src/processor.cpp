#include "processor.h"
#include "linux_parser.h"
#include <vector>
#include <iostream>

Processor::Processor()
    : mPrevTotalCpuTime(0.0), mPrevIdleTime(0.0)
{
}

// TODO: Return the aggregate CPU utilization
float Processor::Utilization() 
{
    std::vector<std::string> cpus = LinuxParser::CpuUtilization();
    if(cpus.empty())
    {
        return 0.0;
    }
    float user = stof(cpus[LinuxParser::kUser_]);
    float nice = stof(cpus[LinuxParser::kNice_]);
    float system = stof(cpus[LinuxParser::kSystem_]);
    float idle = stof(cpus[LinuxParser::kIdle_]);
    float iowait = stof(cpus[LinuxParser::kIOwait_]);
    float irq = stof(cpus[LinuxParser::kIRQ_]);
    float softirq = stof(cpus[LinuxParser::kSoftIRQ_]);
    float steal = stof(cpus[LinuxParser::kSteal_]);
    float guest = stof(cpus[LinuxParser::kGuest_]);
    float guest_nice = stof(cpus[LinuxParser::kGuestNice_]);

    float totalCpuTime = user + nice + system + idle + iowait + irq + softirq + steal;
    float idleTime = idle + iowait;

    float totalCpuTimeDiff = totalCpuTime - mPrevTotalCpuTime;
    float idleTimeDiff = idleTime - mPrevIdleTime;
    float utilization = (totalCpuTimeDiff - idleTimeDiff) / totalCpuTimeDiff;
    mPrevTotalCpuTime = totalCpuTime;
    mPrevIdleTime = idleTime;
    return utilization;
}