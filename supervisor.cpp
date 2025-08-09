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
        if (!loop_startRetries_initial_boot())
            break ;
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
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

bool    Supervisor::loop_startRetries_initial_boot() {

    std::map<std::string, std::vector<Process*>>::iterator  it_map = this->_processMap.begin();
    std::vector<Process*>::iterator                         it_vec;
    ProcessStatus                                           p_status;

    for (it_vec = it_map->second.begin(); it_vec != it_map->second.end(); it_vec++) {
        process_state_Running(*it_vec);
        p_status = (*it_vec)->getStatus();

        if (p_status != ProcessStatus::RUNNING && !(*it_vec)->getRunReached()) {
            
            if ((*it_vec)->getStartRetries() > (*it_vec)->getCountRetries()) {

                if (p_status == ProcessStatus::EXITED || p_status == ProcessStatus::BACKOFF) {
                    std::cout << (*it_vec)->getPid() << " stat = " << (*it_vec)->getPrintStatus() << std::endl;
                    (*it_vec)->stopThread();
                    (*it_vec)->setProcessStatus(ProcessStatus::BACKOFF);
                    (*it_vec)->setCountRetries();
                    (*it_vec)->startProcess();
                }
            }
            else {
                (*it_vec)->setProcessStatus(ProcessStatus::FATAL);
                if (allProcessFatal())
                    return false ;
            }
        }
        if ((*it_vec)->getRunReached &&)
        if (allProcessRunning())
            return false ;
    }
    return true ;
}

bool    Supervisor::loop_autoRestart() {
    ProcessStatus   p_status = (*it_vec).getStatus();
    int             config_restart = (*it_vec).getautoRestart();

    if ((*it_vec).getRunReached && p_status == ProcessStatus::EXITED) {
        if (config_restart == 1 || config_restart == 2 && !(*it_vec).getExit_Code()) {
            (*it_vec).stopThread();
            (*it_vec).startProcess();
        }
    }
    return true ;
}

void    Supervisor::process_state_Running(Process *it_vec) {
    std::time_t     end = std::time(nullptr);
    double          diff_time = difftime(end, (*it_vec).getStartRun());
    ProcessStatus   p_status = (*it_vec).getStatus();

    if (p_status == ProcessStatus::STARTING && (*it_vec).isProcessUp()) {
        if (diff_time >= (*it_vec).getStartTime()) {
            (*it_vec).setProcessStatus(ProcessStatus::RUNNING);
            (*it_vec).setRunReached();
        }
    }
}

// fonction de test pour stopper la boucle startretries en cas de fatal all //
// a enlever avant fin du projet
bool    Supervisor::allProcessFatal() {
    std::map<std::string, std::vector<Process*>>::iterator  it_map = this->_processMap.begin();
    std::vector<Process*>::iterator                         it_vec;

    for (it_vec = it_map->second.begin(); it_vec != it_map->second.end(); ) {
        if ((*it_vec)->getStatus() == ProcessStatus::FATAL) {
            std::cout << "    " << (*it_vec)->getPid() << " end startretries : retriescount : " << (*it_vec)->getCountRetries() << " status => " << (*it_vec)->getPrintStatus() << std::endl;
            it_vec++;
        }
        else
            return false ;
    }
    return true ;
}


// fonction de test pour stopper la boucle lorsque all running //
// a enlever avant fin du projet
bool    Supervisor::allProcessRunning() {
    std::map<std::string, std::vector<Process*>>::iterator  it_map = this->_processMap.begin();
    std::vector<Process*>::iterator                         it_vec;

    for (it_vec = it_map->second.begin(); it_vec != it_map->second.end(); ) {
        if ((*it_vec)->getRunReached()) {
            std::cout << "    " << (*it_vec)->getPid() << " all running : " << (*it_vec)->getCountRetries() << " status => " << (*it_vec)->getPrintStatus() << " : " << (*it_vec)->getRunReached() << std::endl;
            it_vec++;
        }
        else
            return false ;
    }
    return true ;
}

// non utiliser
void    Supervisor::processStart() {
    std::map<std::string, std::vector<Process*>>::iterator  it = this->_processMap.begin();
    std::vector<Process*>::iterator                         it_vec;

    for (it_vec = it->second.begin(); it_vec != it->second.end(); it_vec++) {
        (*it_vec)->startProcess();
        std::cout << "status in supervisor : " << (*it_vec)->getPrintStatus() << std::endl;
    }
}