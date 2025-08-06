# include "process.hpp"

pid_t Process::getPid() const 
{
    return this->_processus;
}

const char* Process::getPrintStatus() const {
    {
        std::lock_guard<std::mutex> lock(this->_status_mutex);
        const char* str = enumtoString(this->_status);
        return str ; 
    }
}

ProcessStatus Process::getStatus() const {
    {
        std::lock_guard<std::mutex> lock(this->_status_mutex);
        return this->_status ; 
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

int     Process::getStartTime() const {
    return this->_config.startTime ;
}

std::time_t Process::getStartRun() const {
    {
        std::lock_guard<std::mutex> lock(this->_status_mutex);
        return this->_start_run ;
    }
}

int     Process::getStartRetries() const {
    return this->_config.startRetries ;
}

int     Process::getCountRetries() const {
    return this->_count_retries ;
}

void    Process::setCountRetries() {
    this->_count_retries++;
}

void    Process::setProcessStatus(ProcessStatus tmp) {
    {
        std::lock_guard<std::mutex> lock(this->_status_mutex);
        this->_status = tmp;
    }
}