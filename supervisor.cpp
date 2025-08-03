#include "supervisor.hpp"

Supervisor::Supervisor(std::map<std::string, t_config> configMap)
{
    std::map<std::string, t_config>::iterator it;

    for (it = configMap.begin(); it != configMap.end(); it++) {
        for (int i = 0; i < it->second.numProcs; i++) {
            Process*    procs = new Process(i, it->first, it->second, ProcessStatus::STOPPED);
            this->_processMap[it->first].push_back(procs);
        }
    }

    /////////// test print
    std::map<std::string, std::vector<Process*>>::iterator itt;

    for (itt = this->_processMap.begin(); itt != this->_processMap.end(); itt++) {
        std::cout << "Verif Processus = " << itt->first << std::endl; 
        std::vector<Process*>::iterator     it_vec;
        for (it_vec = itt->second.begin(); it_vec != itt->second.end(); it_vec++)
            std::cout << " " << *it_vec << std::endl; 
    }
}

Supervisor::~Supervisor() {
    std::map<std::string, std::vector<Process*>>::iterator  it;
    std::vector<Process*>::iterator                         it_vec;

    for (it = this->_processMap.begin(); it != this->_processMap.end(); it++) {
        for (it_vec = it->second.begin(); it_vec != it->second.end(); it_vec++)
            delete *it_vec; 
        it->second.clear();
    }
    this->_processMap.clear();
}

void    Supervisor::processStart() {
    std::map<std::string, std::vector<Process*>>::iterator  it = this->_processMap.begin();
    std::vector<Process*>::iterator                         it_vec = it->second.begin();

    for (it_vec = it->second.begin(); it_vec != it->second.end(); it_vec++) {
        (*it_vec)->startProcess();
        std::cout << "status in supervisor : " << (*it_vec)->getStatus() << std::endl;
    }
}