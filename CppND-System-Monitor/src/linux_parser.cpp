#include <dirent.h>
#include <unistd.h>
#include <sstream>
#include <string>
#include <vector>
#include <filesystem>

#include "linux_parser.h"

using std::all_of;
using std::isdigit;
using std::stoi;
using std::stof;
using std::stol;
using std::string;
using std::to_string;
using std::vector;

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
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

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, kernel, version;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

vector<int> LinuxParser::Pids() {
  vector<int> pids;
  
  for(auto const& entry : std::filesystem::directory_iterator(kProcDirectory))
  {
    if(std::filesystem::is_directory(entry.status()))
    {
      std::string folderName = entry.path().filename().string();
      if(std::all_of(folderName.begin(), folderName.end(), isDigit))
      {
        pids.push_back(std::stoi(folderName));
      } 
    }
  }

  return pids;
}

float LinuxParser::MemoryUtilization()
{
  float memUtilisation = 0.0;
  std::string memTotal, memFree, line, temp;

  std::ifstream stream(kProcDirectory + kMeminfoFilename);
  if(stream.is_open())
  {
    while(std::getline(stream, line))
    {
      if(line.starts_with("MemTotal"))
      {
        std::istringstream linestream(line);
        linestream >> temp >> memTotal;
      }
      else if(line.starts_with("MemFree"))
      {
        std::istringstream linestream(line);
        linestream >> temp >> memFree;
      }
      else if(!memTotal.empty() && !memFree.empty())
      {
        break;
      }
    }   
  }
  memUtilisation = (stof(memTotal) - stof(memFree)) / stof(memTotal);
  return memUtilisation;
}

long LinuxParser::UpTime() 
{
  std::string line, uptime;
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  if(stream.is_open())
  {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> uptime;
  }
  return stol(uptime);
}

// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() { return 0; }

// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid[[maybe_unused]]) { return 0; }

// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() { return 0; }

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() { return 0; }

// TODO: Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization()
{
  vector<std::string> cpus;
  std::string key, user, nice, system, idle, iowait, irq, softirq, steal, guest, guest_nice, line;

  std::ifstream stream(kProcDirectory + kStatFilename);
  if(stream.is_open())
  {
    std::getline(stream, line);
    std::istringstream linestream(line);
    while (linestream >> key >> user >> nice >> system >> idle >>
      iowait >> irq >> softirq >> steal >> guest >> guest_nice) 
    {
      if (key == "cpu") 
      {
        cpus.push_back(user);
        cpus.push_back(nice);  
        cpus.push_back(system);
        cpus.push_back(idle);
        cpus.push_back(iowait);
        cpus.push_back(irq);
        cpus.push_back(softirq);
        cpus.push_back(steal);
        cpus.push_back(guest);
        cpus.push_back(guest_nice);
        return cpus;
      }
    }
  }
  return cpus;
}

int LinuxParser::TotalProcesses() 
{ 
  std::string line, temp, processes;

  std::ifstream stream(kProcDirectory + kStatFilename);
  if(stream.is_open())
  {
    while(std::getline(stream, line))
    {
      if(line.starts_with("processes"))
      {
        std::istringstream linestream(line);
        linestream >> temp >> processes;
      }
    }
  }
  return stol(processes);
}

int LinuxParser::RunningProcesses() 
{ 
  std::string line, temp, runningProcesses;

  std::ifstream stream(kProcDirectory + kStatFilename);
  if(stream.is_open())
  {
    while(std::getline(stream, line))
    {
      if(line.starts_with("procs_running"))
      {
        std::istringstream linestream(line);
        linestream >> temp >> runningProcesses;
      }
    }
  }
  return stol(runningProcesses);
}

std::vector<float> LinuxParser::CpuUtilization(int pid)
{
  vector<float> cpuValues{};
  string line;
  float time = 0;
  string value;
  std::ifstream filestream(kProcDirectory + std::to_string(pid) + kStatFilename);
  if (filestream.is_open()) 
  {
    while (std::getline(filestream, line)) 
    {
      std::istringstream linestream(line);
      for (int i = 1; i <= kStarttime_; i++) 
      {
        linestream >> value;
        if (i == kUtime_ || i == kStime_ || i == kCutime_ || i == kCstime_ ||
            i == kStarttime_) 
        {
          time = std::stof(value)/ sysconf(_SC_CLK_TCK);
          cpuValues.push_back(time);
        }
      }
    }
  }
  return cpuValues;
}
// Read and return the command associated with a process
string LinuxParser::Command(int pid) 
{
  std::string line, command;

  std::ifstream stream(kProcDirectory + std::to_string(pid) + kCmdlineFilename);
  if(stream.is_open())
  {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> command;
  }
  return command;
}

// Read and return the memory used by a process
string LinuxParser::Ram(int pid) 
{
  string line;
  string key;
  string value = "";
  std::ifstream filestream(kProcDirectory + "/" + std::to_string(pid) +
                           kStatusFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "VmSize") {
          return value;
        }
      }
    }
  }
  return value;
}

// Read and return the user ID associated with a process
string LinuxParser::Uid(int pid) 
{
  std::string line, temp, uid;

  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatusFilename);
  if(stream.is_open())
  {
    while(std::getline(stream, line))
    {
      if(line.starts_with("Uid"))
      {
        std::istringstream linestream(line);
        linestream >> temp >> uid;
      }
    }
  }
  return uid;
}

// Read and return the user associated with a process
string LinuxParser::User(int pid) 
{
  std::string line, temp, uid, user;

  // Finding the UID associated with the process
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatusFilename);
  if(stream.is_open())
  {
    while(std::getline(stream, line))
    {
      if(line.starts_with("Uid"))
      {
        std::istringstream linestream(line);
        linestream >> temp >> uid;
      }
    }
  }

  // Finding the user associated with the process
  std::string temp2 , username, password, ID;
  std::ifstream stream2(kPasswordPath);
  if(stream2.is_open())
  {
    while(std::getline(stream2, line))
    {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      linestream >> username >> password >> ID;
      if(uid == ID)
      {
        return username;
      }
    }
  }
  return "";
}

// Read and return the uptime of a process
long LinuxParser::UpTime(int pid) 
{
  std::string line, uptime;

  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatFilename);
  if(stream.is_open())
  {
    std::getline(stream, line);
    std::istringstream linestream(line);

    for(int i= 0; i < 22; i++)
    {
     linestream >> uptime; 
    }
    return stol(uptime);
  }
  return 0;
}

bool LinuxParser::isDigit(unsigned char c)
{
  return c >= '0' && c <= '9';
}