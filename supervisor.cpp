#include "supervisor.hpp"

Supervisor::Supervisor(std::map<std::string, t_config> configMap)
{
    std::map<std::string, t_config>::iterator it;
    for (it = configMap.begin(); it != configMap.end(); it++) {
        
        Process*    procs = new Process(it->second, ProcessStatus::DOWN);
        this->_processMap[it->first] = procs;
    }

    std::map<std::string, Process*>::iterator itt;
    for (itt = this->_processMap.begin(); itt != this->_processMap.end(); itt++)
        std::cout << "verif procs = " << itt->first << " " << itt->second << std::endl; 
}

Supervisor::~Supervisor() {
    std::map<std::string, Process*>::iterator itt;
    for (itt = this->_processMap.begin(); itt != this->_processMap.end(); itt++) {
        delete itt->second; 
    }
    this->_processMap.clear();
}

void    Supervisor::processStart() {
    std::map<std::string, Process*>::iterator it = this->_processMap.begin();
    it->second->startProcess();
    std::cout << "status in supervisor : " << it->second->getStatus() << std::endl;
}