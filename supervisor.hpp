#ifndef SUPERVISOR_HPP
# define SUPERVISOR_HPP

#include "process.hpp"
#include <map>
#include <list>
#include <mutex>

class Supervisor {
    public:
        Supervisor(std::map<std::string, t_config> configMap);
        ~Supervisor();
        void reloadConfig();
        void mainShutdown();
        void processStatus();
        void processStart(const std::list<std::string>& processNameList);
        void processStop(const std::list<std::string>& processNameList);
        void processRestart(const std::list<std::string>& processNameList);

    private:
        void init();
        std::mutex _statusMutex;
        std::map<std::string, std::pair<ProcessStatus*,Process*>> _processMap;
};
#endif