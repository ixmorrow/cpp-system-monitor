#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>
#include <unordered_map>
#include <iostream>
#include <unistd.h>

#include "linux_parser.h"

using std::stof;
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
  string os, version, kernel;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
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

// DONE: Read and return the system memory utilization
float LinuxParser::MemoryUtilization() {
  std::unordered_map<string, float> mem_stats; 
  string key, stat, unit;
  string line;
  std::ifstream stream(kProcDirectory + kMeminfoFilename);
  if (stream.is_open()){
    for(int i=0; i < 4; i++){
      std::getline(stream, line);
      std::istringstream linestream(line);
      linestream >> key >> stat >> unit;
      mem_stats[key] = stof(stat);
    }
  }
  // returning MemTotal / MemFree to start
  return mem_stats.at("MemFree:")/mem_stats.at("MemTotal:"); 
}

// DONE: Read and return the system uptime
long LinuxParser::UpTime() { 
  string uptime, idletime;
  string line;
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  if (stream.is_open()){
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> uptime >> idletime;
  }

  return stol(uptime);
}

// DONE: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() {
  std::ifstream filestream(kProcDirectory + kStatFilename);
  std::string line;
  std::string cpu;
  long user, nice, system, idle, iowait, irq, softirq;
  long system_total_jiffies = 0;

  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      linestream >> cpu;
      if (cpu.substr(0, 3) == "cpu") {
        linestream >> user >> nice >> system >> idle >> iowait >> irq >> softirq;
        long cpu_total_jiffies = user + nice + system + idle + iowait + irq + softirq;
        system_total_jiffies += cpu_total_jiffies;
      }
    }
  }

  return system_total_jiffies;
}

// DONE: Read and return the number of active jiffies for a PID
long LinuxParser::ActiveJiffies(int pid) {
  std::ifstream filestream(kProcDirectory + std::to_string(pid) + kStatFilename);
  std::string line;
  std::getline(filestream, line);
  std::istringstream linestream(line);
  std::istream_iterator<std::string> beg(linestream), end;
  std::vector<std::string> statValues(beg, end);

  // The active jiffies are the sum of utime, stime, cutime, and cstime values
  long utime = std::stol(statValues[13]);
  long stime = std::stol(statValues[14]);
  long cutime = std::stol(statValues[15]);
  long cstime = std::stol(statValues[16]);
  long active_jiffies = utime + stime + cutime + cstime;

  return active_jiffies;
}

// DONE: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() {
  std::ifstream filestream(kProcDirectory + kStatFilename);
  std::string line;
  std::string cpu;
  long user, nice, system, idle, iowait, irq, softirq;
  long total_active_jiffies = 0;

  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      linestream >> cpu;
      if (cpu.substr(0, 3) == "cpu") {
        linestream >> user >> nice >> system >> idle >> iowait >> irq >> softirq;
        long active_jiffies = user + nice + system + irq + softirq;
        total_active_jiffies += active_jiffies;
      }
    }
  }

  return total_active_jiffies;
}

// DONE: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() {
  std::ifstream filestream(kProcDirectory + kStatFilename);
  std::string line;
  std::string cpu;
  long user, nice, system, idle, iowait, irq, softirq;
  long total_idle_jiffies = 0;

  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      linestream >> cpu;
      if (cpu.substr(0, 3) == "cpu") {
        linestream >> user >> nice >> system >> idle >> iowait >> irq >> softirq;
        long idle_jiffies = idle + iowait;
        total_idle_jiffies += idle_jiffies;
      }
    }
  }

  return total_idle_jiffies;
}

// DONE: Read and return the total number of processes
int LinuxParser::TotalProcesses() {
  string line, key, total_processes;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()){
    while(key != "processes"){
      std::getline(stream, line);
      std::istringstream linestream(line);
      linestream >> key >> total_processes;
    }
  }

  return std::stoi(total_processes);
}

// DONE: Read and return the number of running processes
int LinuxParser::RunningProcesses() {
  string line, key, running_processes;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()){
    while(key != "procs_running"){
      std::getline(stream, line);
      std::istringstream linestream(line);
      linestream >> key >> running_processes;
    }
  }

  return std::stoi(running_processes);
}

vector<string> LinuxParser::ProcessCpuUtilization(int pid) {
  string line, key, cpu_stat, cpu_usage;
  vector<string> cpu_stats;
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatFilename);
  if (stream.is_open()){
    std::getline(stream, line);
    std::istringstream linestream(line);
    // Read and discard the first four words
    for (int i = 0; i < 12; ++i) {
        linestream >> cpu_stat;
    }
    for(int i = 0; i < 4; ++i){
      linestream >> cpu_usage;
      cpu_stats.push_back(cpu_usage);
    }
    // Read and discard the next 4 items
    for(int i = 0; i < 4; ++i){
      linestream >> cpu_stat;
    }
    linestream >> cpu_usage;
    cpu_stats.push_back(cpu_usage);
  }

  return cpu_stats;
}

// DONE: Read and return the command associated with a process
string LinuxParser::Command(int pid) { 
  string line, cmd;
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kCmdlineFilename);
  if (stream.is_open()){
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> cmd;
  }
  return cmd; 
}

// DONE: Read and return the memory used by a process
string LinuxParser::Ram(int pid) {
  string line, key, ram_usage;
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatusFilename);
  if (stream.is_open()){
    while(key != "VmSize:"){
      std::getline(stream, line);
      std::istringstream linestream(line);
      linestream >> key >> ram_usage;
    }
  }

  return ram_usage;
}

// DONE: Read and return the user ID associated with a process
string LinuxParser::Uid(int pid) {
  string line, key, uid;
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatusFilename);
  if (stream.is_open()){
    while(key != "Uid:"){
      std::getline(stream, line);
      std::istringstream linestream(line);
      linestream >> key >> uid;
    }
  }
  return uid;
}

std::vector<std::string> split(const std::string &s, char delim) {
    std::vector<std::string> tokens;
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        tokens.push_back(item);
    }
    return tokens;
}

// DONE: Read and return the user associated with a process
string LinuxParser::User(int pid) {
  string uid = Uid(pid);
  string line, username, delimeter, uid_key;
  std::ifstream stream(kPasswordPath);
  if (stream.is_open()){
    while(uid_key != uid){
      std::getline(stream, line);
      std::vector<std::string> tokens = split(line, ':');
      username = tokens[0];
      uid_key = tokens[2];
    }
  }
  return username; 
}

// DONE: Read and return the uptime of a process
long LinuxParser::UpTime(int pid) {
  string line, pid_stat;
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatFilename);
  if (stream.is_open()){
    std::getline(stream, line);
    std::istringstream linestream(line);
    // Read and discard the first four words
    for (int i = 0; i < 22; i++) {
        linestream >> pid_stat;
    }
  }

  return std::stol(pid_stat)/sysconf(_SC_CLK_TCK); 
}
