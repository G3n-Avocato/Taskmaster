# include "process.hpp"

pid_t Process::getPid() const 
{
    return this->_processus;
}

const char* Process::getStatus() const {
    {
        std::lock_guard<std::mutex> lock(this->_status_mutex);
        const char* str = enumtoString(this->_status);
        return str ; 
    }
}

bool        Process::getautoStart() const {
    return this->_config.autoStart ;
}

int         Process::getautoRestart() const {
    switch (this->_config.autoRestart) {
        case True: return 1;
        case False: return 0;
        case Unexpected: return 2;
        default: return 2;
    }
}

double      Process::getdiffTime() {
    return this->_difftime ;
    //if (this->_config.startTime > this->_difftime)
    //    return false ;
    //else
    //    return true ;
}

int     Process::getCountRetries() const {
    return this->_count_retries ;
}

int     Process::getStartRetries() const {
    return this->_config.startRetries ;
}

int     Process::getStartTime() const {
    return this->_config.startTime ;
}

void    Process::setCountRetries() {
    this->_count_retries++;
}

void    Process::setdiffTime() {
    this->_difftime = difftime(this->_end, this->_start);
}