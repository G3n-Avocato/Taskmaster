#include "supervisor.hpp"

Supervisor::Supervisor(std::map<std::string, t_config> configMap) {
    
    std::map<std::string, t_config>::iterator it;

    for (it = configMap.begin(); it != configMap.end(); it++) {
        for (int i = 0; i < it->second.numProcs; i++) {
            Process     proc(i, it->first, it->second);
            this->_processMap[it->first].push_back(std::move(proc));
        }
    }
}

Supervisor::~Supervisor() {
    this->_processMap.clear();
}

void    Supervisor::start_init_boot() {

    std::map<std::string, std::vector<Process>>::iterator  it_m = this->_processMap.begin();

    for (this->_it_vec = it_m->second.begin(); this->_it_vec != it_m->second.end(); this->_it_vec++) {
        if ((*this->_it_vec).getautoStart())
            (*this->_it_vec).startProcess();
    }
}

//void    Supervisor::monitoring_Waitpid() {
//    int status;
//
//    pid_t child_pid = waitpid(, &status, WNOHANG);
//
//
//
//}