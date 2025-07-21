#include <dirent.h>
#include <unistd.h>
#include <sstream>
#include <string>
#include <vector>

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
      if(std::all_of(folderName.begin(), folderName.end(), isdigit))
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
  memUtilisation = (stof(memTotal) - stof(memFree));
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
vector<string> LinuxParser::CpuUtilization() { return {}; }

// TODO: Read and return the total number of processes
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

// TODO: Read and return the number of running processes
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

// TODO: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Command(int pid[[maybe_unused]]) { return string(); }

// TODO: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid[[maybe_unused]]) { return string(); }

// TODO: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid[[maybe_unused]]) { return string(); }

// TODO: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid[[maybe_unused]]) { return string(); }

// TODO: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid[[maybe_unused]]) { return 0; }
