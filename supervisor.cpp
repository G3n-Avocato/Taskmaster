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

    /////////// test print //////////////////////////////
    std::map<std::string, std::vector<Process*>>::iterator itt;

    for (itt = this->_processMap.begin(); itt != this->_processMap.end(); itt++) {
        std::cout << "Verif Processus = " << itt->first << std::endl; 
        std::vector<Process*>::iterator     it_vec;
        for (it_vec = itt->second.begin(); it_vec != itt->second.end(); it_vec++)
            std::cout << " " << *it_vec << std::endl; 
    }
    /////////////////////////////////////////////////////

    std::map<std::string, std::vector<Process*>>::iterator  it_m = this->_processMap.begin();
    std::vector<Process*>::iterator                         it_vec;

    for (it_vec = it_m->second.begin(); it_vec != it_m->second.end(); it_vec++) {

        if ((*it_vec)->getautoStart())
            (*it_vec)->startProcess();
    }
    while (true) {
        loop_startRetries_initial_boot();
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

void    Supervisor::loop_startRetries_initial_boot() {

    std::map<std::string, std::vector<Process*>>::iterator  it_map = this->_processMap.begin();
    std::vector<Process*>::iterator                         it_vec;
    std::time_t                                             end;
    double                                                  diff_time;
    ProcessStatus                                           p_status;

    for (it_vec = it_map->second.begin(); it_vec != it_map->second.end() && (*it_vec)->getStartRetries() > 0; it_vec++) {
        end = std::time(nullptr);
        diff_time = difftime(end, (*it_vec)->getStartRun());
        p_status = (*it_vec)->getStatus();
        
        if ((*it_vec)->getStartRetries() > (*it_vec)->getCountRetries()) {
            
            if (diff_time > (*it_vec)->getStartTime()) {
                if (p_status == ProcessStatus::RUNNING || p_status == ProcessStatus::STARTING)
                    continue ;
                else if (p_status == ProcessStatus::EXITED || p_status == ProcessStatus::BACKOFF) {
                    (*it_vec)->setProcessStatus(ProcessStatus::BACKOFF);
                    (*it_vec)->startProcess();
                    (*it_vec)->setCountRetries();
                }
            }
        }
        else {
            if (diff_time > (*it_vec)->getStartTime()) {
                if (p_status == ProcessStatus::RUNNING || p_status == ProcessStatus::STARTING)
                    continue ;
                else
                    (*it_vec)->setProcessStatus(ProcessStatus::FATAL);
            }
        }
    }
}

void    Supervisor::processStart() {
    std::map<std::string, std::vector<Process*>>::iterator  it = this->_processMap.begin();
    std::vector<Process*>::iterator                         it_vec;

    for (it_vec = it->second.begin(); it_vec != it->second.end(); it_vec++) {
        (*it_vec)->startProcess();
        std::cout << "status in supervisor : " << (*it_vec)->getPrintStatus() << std::endl;
    }
}