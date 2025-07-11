#ifndef SUPERVISOR_HPP
# define SUPERVISOR_HPP

struct Config {
    std::string                         cmd;
    int                                 numprocs;
    std::string                         umask;
    std::string                         workingdir;
    std::string                         autostart;
    std::string                         autorestart;
    std::vector<std::string>            exitcodes;
    int                                 startretries;
    int                                 starttime;
    std::string                         stopsignal;
    int                                 stoptime;
    std::string                         stdout;
    std::string                         stderr;
    std::map<std::string, std::string>  env;

};

class Supervisor {

    public:
        
        Supervisor(void);
        Supervisor();
        Supervisor(Supervisor const &src);
        Supervisor& operator=(Supervisor const &rhs);
        ~Supervisor(void);

    private:

        Config  _config;

};

# endif