#ifndef PROCESS_HPP
# define PROCESS_HPP

#include <iostream>
#include <map>

enum class e_autoStatus
{
    TRUE = "true"
    FALSE = "false"
    UNEXPECTED = "unexpected"
};

struct Config {
    std::string                         cmd;
    int                                 numProcs;
    std::string                         umask;
    std::string                         workingDir;
    e_autoStatus                        autoStart;
    e_autoStatus                        autoRestart;
    std::vector<std::string>            exitCodes;
    int                                 startRetries;
    int                                 startTime;
    std::string                         stopSignal;
    int                                 stopTime;
    std::string                         stdout;
    std::string                         stderr;
    std::map<std::string, std::string>  env;
};

class Process {
    
    public:
    
        Process(void);
        Process();
        Process(Process const &src);
        Process& operator=(Process const &rhs);
        ~Process(void);

        //get
        std::string getCmd(){return this->_config.cmd};
        int getNumProcs(){return this->_config.numProcs};
        std::string getUmask(){return this->_config.umask};
        std::string getWorkingDir(){return this->_config.workingDir};
        e_autoStatus getAutoStart(){return this->_config.autoStart};
        e_autoStatus getAutoRestart(){return this->_config.autoRestart};
        std::vector<std::string>  getExitCodes(){return this->_config.exitCodes};
        int getStartRetries(){return this->_config.startRetries};
        int getStartTime(){return this->_config.startTime};
        std::string getStopSignal(){return this->_config.stopSignal};
        std::string getStdout(){return this->_config.stdout};
        std::string getStderr(){return this->_config.stderr};
        std::map<std::string, std::string>  getEnv(){return this->_config.env};

        //set

        void setCmd(std::string input){this->_config.cmd = input};
        void setNumProcs(int input){this->_config.numProcs = input};
        void setUmask(std::string input){this->_config.umask = input};
        void setWorkingDir(std::string input){this->_config.workingDir = input};
        void setAutoStart(e_autoStatus input){this->_config.autoStart = input};
        void setAutoRestart(e_autoStatus input){this->_config.autoRestart = input};
        void setExitCodes(std::vector<std::string> input){this->_config.exitCodes = input};
        void setStartRetries(int input){this->_config.startRetries = input};
        void setStartTime(int input){this->_config.startTime = input};
        void setStopSignal(std::string input){this->_config.stopSignal = input};
        void setStdout(std::string input){this->_config.stdout = input};
        void setStderr(std::string input){this->_config.stderr = input};
        void setEnv(std::map<std::string, std::string> input){this->_config.env = input};

    private:
        std::string _name;
        Config      _config;
        pid_t       _processus;

};

std::ostream &operator<<(std::ostream &o, Process const &rhs);

#endif