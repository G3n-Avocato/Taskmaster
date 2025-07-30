#ifndef SUPERVISOR_HPP
# define SUPERVISOR_HPP

#include "process.hpp"

class Supervisor {

    public:
        
        Supervisor(std::map<std::string, t_config> configMap);
        ~Supervisor(void);

        void    processStart();

    private:

        std::map<std::string, Process*> _processMap;
};

# endif