#ifndef PROCESS_HPP
# define PROCESS_HPP

#include <iostream>
#include <map>


class Process {
    
    public:
    
        Process(void);
        Process();
        Process(Process const &src);
        Process& operator=(Process const &rhs);
        ~Process(void);

    private:

        std::map<pid_t, std::string>    _processus;

};

std::ostream &operator<<(std::ostream &o, Process const &rhs);

#endif