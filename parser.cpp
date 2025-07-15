#include "parser.hpp"

Parser::Parser(void) {}

Parser::Parser(std::string config_file, std::map<std::string, t_config> &programs_tab) {
    YAML::Node  file_node = YAML::LoadFile(config_file);

    if (file_node["programs"]) {
        YAML::Node  programs_node = file_node["programs"];
        
        for (YAML::const_iterator it = programs_node.begin(); it != programs_node.end(); it++) {    
            
            std::string progName = it->first.as<std::string>();

            t_config  conf_struct;
            if (YAML::convert<t_config>::decode(it->second, conf_struct))
                programs_tab[progName] = conf_struct;
            else
                std::cout << "Erreur convert decode YAML to struct config\n";
        }

    }


}

Parser::~Parser(void) {

}

int Parser::getSignalNumber(const std::string& name) {

    const std::map<std::string, int>    signalMap = {
        {"HUP", SIGHUP}, {"INT", SIGINT}, {"QUIT", SIGQUIT},
        {"ILL", SIGILL}, {"TRAP", SIGTRAP}, {"ABRT", SIGABRT},
        {"IOT", SIGIOT}, {"BUS", SIGBUS}, {"FPE", SIGFPE},
        {"KILL", SIGKILL}, {"USR1", SIGUSR1}, {"SEGV", SIGSEGV},
        {"USR2", SIGUSR2}, {"PIPE", SIGPIPE}, {"ALRM", SIGALRM},
        {"TERM", SIGTERM}, {"STKFLT", SIGSTKFLT}, {"CHLD", SIGCHLD},
        {"CONT", SIGCONT}, {"STOP", SIGSTOP}, {"TSTP", SIGTSTP},
        {"TTIN", SIGTTIN}, {"TTOU", SIGTTOU}, {"URG", SIGURG},
        {"XCPU", SIGXCPU}, {"XFSZ", SIGXFSZ}, {"VTALRM", SIGVTALRM},
        {"PROF", SIGPROF}, {"WINCH", SIGWINCH}, {"POLL", SIGPOLL},
        {"IO", SIGIO}, {"PWR", SIGPWR}, {"SYS", SIGSYS}

    };

    std::map<std::string, int>::const_iterator  it = signalMap.find(name);
    if (it == signalMap.end())
        return (-1);
    return (it->second);
}