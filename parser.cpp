#include "parser.hpp"

Parser::Parser(const std::string &config_file, std::map<std::string, t_config> &programs_tab) {

    if (!checkTypeFileConfigYaml(config_file)) {
        std::cerr << "Error Exception : wrong file extension " << std::endl;
        return ;
    }
    
    try {
        YAML::Node  file_node = YAML::LoadFile(config_file);

        if (file_node["programs"].IsDefined()) {
            YAML::Node  programs_node = file_node["programs"];

            for (YAML::const_iterator it = programs_node.begin(); it != programs_node.end(); it++) {    

                std::string progName = it->first.as<std::string>();

                t_config  conf_struct = initConfigStruct();


                YAML::convert<t_config>::decode(it->second, conf_struct);
                programs_tab[progName] = conf_struct;
            }
        }
        else
            std::cout << "Error Exception : Node 'programs' doesn't exist \n";
    } catch (const YAML::BadFile &e) {
        std::cerr << "Error Exception : " << e.what() << std::endl; 
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

bool    Parser::checkTypeFileConfigYaml(const std::string &config_file) {
    std::size_t found = config_file.rfind(".");
    
    if (found == std::string::npos)
        return false ;

    std::string extension = config_file.substr(found);
    if (extension == ".yaml" || extension == ".yml")
        return true ;
    else
        return false ;
}

t_config    Parser::initConfigStruct(void) {
    t_config    def_config_struct;

    def_config_struct.numProcs = 1;
    def_config_struct.umask = 022;
    def_config_struct.workingDir = "/";
    def_config_struct.autoStart = "true";
    def_config_struct.autoRestart = "unexpected";
    def_config_struct.exitCodes = {0};
    def_config_struct.startRetries = 3;
    def_config_struct.startTime = 1;
    def_config_struct.stopSignal = ::Parser::getSignalNumber("TERM");
    def_config_struct.stopTime = 10;
    def_config_struct.stdout = "";
    def_config_struct.stderr = "";

    return (def_config_struct);
}

const char* Parser::RequiredParaException::what(void) const throw() {
    return ("Required parameter");
}