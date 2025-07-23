#ifndef PARSER_HPP
# define PARSER_HPP

#include "process.hpp"

#include <map>
#include <string>
#include <exception>
#include <yaml-cpp/yaml.h>
#include <signal.h>

class Parser {

    public:
        
        Parser(void);
        Parser(const std::string& config_file, std::map<std::string, t_config> &programs_tab);
        ~Parser(void);

        static int getSignalNumber(const std::string& name);

        class BadParaException : public std::exception {
            
            public:
                
                explicit BadParaException(const std::string &msg) : _message(msg) {}

                virtual const char* what() const noexcept override {
                    return _message.c_str();
                }
            
            private:
                std::string _message;
        
        };
        class RequiredParaException : public std::exception {
            public:
                virtual const char* what() const throw();
        };

    private:
        
        bool        checkTypeFileConfigYaml(const std::string &config_file);
        t_config    initConfigStruct(void);

};

namespace YAML {
template<>
struct convert<t_config> {
    static bool decode(const Node& node, t_config& conf) {
        //if (!node.IsMap())
        //    throw RepresentationException(node.Mark(), "Representation exception");
        const std::set<std::string> Keys = {
            "cmd", "numprocs", "umask", "workingdir",
            "autostart", "autorestart", "exitcodes",
            "startretries", "starttime", "stopsignal",
            "stoptime", "stdout", "stderr", "env"
        };
        for (YAML::const_iterator it = node.begin(); it != node.end(); it++) {
            std::string key = it->first.as<std::string>();
            if (Keys.find(key) == Keys.end())
                throw YAML::Exception(it->first.Mark(), "Bad Key");
        }

        if (!node["cmd"])
            throw ::Parser::RequiredParaException();
        conf.cmd = node["cmd"].as<std::string>();

        if (node["numprocs"]) { //voir limite system du nbre du processus
            conf.numProcs = node["numprocs"].as<int>();
            if (conf.numProcs < 0)
                throw ::Parser::BadParaException("Error Exception : Bad Parameter 'numprocs' ");
        }
        if (node["umask"]) {                    // 0 -> 511 max
            conf.umask = node["umask"].as<int>();
            if (conf.umask > 511)
                throw ::Parser::BadParaException("Error Exception : Bad Parameter 'umask' ");
        }
        if (node["workingdir"])
            conf.workingDir = node["workingdir"].as<std::string>();
        if (node["autostart"]) {                // true or false that's all
            conf.autoStart = node["autostart"].as<std::string>();
            if (conf.autoStart != "true" && conf.autoStart != "false")
                    throw ::Parser::BadParaException("Error Exception : Bad Parameter 'autostart' ");
        }
        if (node["autorestart"]) {              // true or false or unexpected that's all
            conf.autoRestart = node["autorestart"].as<std::string>();
            if (conf.autoRestart != "true" && conf.autoRestart != "false" && conf.autoRestart != "unexpected")
                throw ::Parser::BadParaException("Error Exception : Bad Parameter 'autorestart' ");
        }
        if (node["exitcodes"]) {
            conf.exitCodes.clear();
            for (YAML::Node::const_iterator it = node["exitcodes"].begin(); it != node["exitcodes"].end(); it++)
                conf.exitCodes.push_back(it->as<int>());
        }
        if (node["startretries"]) {
            conf.startRetries = node["startretries"].as<int>();
            if (conf.startRetries < 0)
                throw ::Parser::BadParaException("Error Exception : Bad Parameter 'startretries' ");
        }
        if (node["starttime"]) {
            conf.startTime = node["starttime"].as<int>();
            if (conf.startTime < 0)
                throw ::Parser::BadParaException("Error Exception : Bad Parameter 'starttime' ");
        }
        if (node["stopsignal"]) {
            std::string tmp = node["stopsignal"].as<std::string>();
            conf.stopSignal = ::Parser::getSignalNumber(tmp);
            if (conf.stopSignal == -1)
                throw ::Parser::BadParaException("Error Exception : Bad Parameter 'stopsignal' ");
        }
        if (node["stoptime"]) {
            conf.stopTime = node["stoptime"].as<int>();
            if (conf.stopTime < 0)
                throw ::Parser::BadParaException("Error Exception : Bad Parameter 'stoptime' ");
        }
        if (node["stdout"]) {
            if (!node["stdout"].IsScalar())
                throw ::Parser::BadParaException("Error Exception : Bad Parameter 'stdout' ");
            conf.stdout = node["stdout"].as<std::string>();
        }
        if (node["stderr"]) {
            if (!node["stderr"].IsScalar())
                throw ::Parser::BadParaException("Error Exception : Bad Parameter 'stderr' ");
            conf.stderr = node["stderr"].as<std::string>();
        }
        if (node["env"]) {
            for (YAML::Node::const_iterator it = node["env"].begin(); it != node["env"].end(); it++)
                conf.env[it->first.as<std::string>()] = it->second.as<std::string>();
        }
        return true;
    }
};
}

# endif