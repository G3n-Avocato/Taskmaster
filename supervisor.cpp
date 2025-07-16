#include "supervisor.hpp"
#include <iostream>

Supervisor::Supervisor(std::map<std::string, t_config> configMap)
{
    std::map<std::string, t_config>::iterator it;
    for (it = configMap.begin(); it != configMap.end(); ++it)
    {
        ProcessStatus* status = new ProcessStatus;
        Process* process = new Process(it->second, _statusMutex, status);

        _processMap[it->first] = std::make_pair(status, process);
    }
}

Supervisor::~Supervisor() {}

void Supervisor::reloadConfig() {}

void Supervisor::mainShutdown() {}

void Supervisor::processStatus()
{
    std::map<std::string, std::pair<ProcessStatus*,Process*>>::iterator it;
    for (it = _processMap.begin(); it != _processMap.end(); ++it)
    {
        if (*(it->second.first) == ProcessStatus::START)
        {
            std::cout << it->first << " up" << std::endl;
        }
        else
        {
            std::cout << it->first << " down" << std::endl;
        }
    }
}

void Supervisor::processStart(const std::list<std::string>& processNameList)
{
    std::list<std::string>::const_iterator it;
    for (it = processNameList.begin(); it != processNameList.end(); ++it)
    {
        if (_processMap.find(*it) == _processMap.end())
            return;
    }
    for (it = processNameList.begin(); it != processNameList.end(); ++it)
    {
        if (!_processMap[*it].second->isProcessUp())
            _processMap[*it].second->startProcess(1);
    }
}

void Supervisor::processStop(const std::list<std::string>& processNameList)
{
    std::list<std::string>::const_iterator it;
    for (it = processNameList.begin(); it != processNameList.end(); ++it)
    {
        if (_processMap.find(*it) == _processMap.end())
            return;
    }
    for (it = processNameList.begin(); it != processNameList.end(); ++it)
    {
        if (_processMap[*it].second->isProcessUp())
            _processMap[*it].second->stopProcess();
    }
}

void Supervisor::processRestart(const std::list<std::string>& processNameList)
{
    std::list<std::string>::const_iterator it;
    for (it = processNameList.begin(); it != processNameList.end(); ++it)
    {
        if (_processMap.find(*it) == _processMap.end())
            return;
    }
    for (it = processNameList.begin(); it != processNameList.end(); ++it)
    {
        if (_processMap[*it].second->isProcessUp())
            _processMap[*it].second->stopProcess();
        _processMap[*it].second->startProcess(1);
    }
}

void Supervisor::init() {}
