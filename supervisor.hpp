#ifndef SUPERVISOR_HPP
# define SUPERVISOR_HPP

#include "process.hpp"

class Supervisor {

    public:
        
        Supervisor(std::map<std::string, t_config> configMap);
        ~Supervisor(void);

        void    processStart();

    private:

        bool    loop_startRetries_initial_boot();
        bool    loop_autoRestart(Process *it_vec);
        void    process_state_Running(Process *it_vec);
        
        bool    allProcessFatal();
        bool    allProcessRunning();

        std::map<std::string, std::vector<Process*>> _processMap;
};

# endif