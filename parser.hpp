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
        Parser(std::string config_file, std::map<std::string, t_config> &programs_tab);
        ~Parser(void);

        static int getSignalNumber(const std::string& name);

        class BadParaException : public std::exception {
            public:
                virtual const char* what() const throw();
        };
        class RequiredParaException : public std::exception {
            public:
                virtual const char* what() const throw();
        };

    private:

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

        if (node["numprocs"]) {
            conf.numProcs = node["numprocs"].as<int>();
            if (conf.numProcs < 0)
                throw ::Parser::BadParaException();
        }
        if (node["umask"]) {
            conf.umask = node["umask"].as<int>();
            if (conf.umask > 511)
                throw ::Parser::BadParaException();
        }
        if (node["workingdir"])
            conf.workingDir = node["workingdir"].as<std::string>();
        if (node["autostart"])
            conf.autoStart = node["autostart"].as<std::string>();
        if (node["autorestart"])
            conf.autoRestart = node["autorestart"].as<std::string>();
        if (node["exitcodes"]) {
            for (YAML::Node::const_iterator it = node["exitcodes"].begin(); it != node["exitcodes"].end(); it++)
                conf.exitCodes.push_back(it->as<int>());
        }
        if (node["startretries"]) {
            conf.startRetries = node["startretries"].as<int>();
            if (conf.startRetries < 0)
                throw ::Parser::BadParaException();
        }
        if (node["starttime"]) {
            conf.startTime = node["starttime"].as<int>();
            if (conf.startTime < 0)
                throw ::Parser::BadParaException();
        }
        if (node["stopsignal"]) {
            std::string tmp = node["stopsignal"].as<std::string>();
            conf.stopSignal = ::Parser::getSignalNumber(tmp);
        }
        if (node["stoptime"]) {
            conf.stopTime = node["stoptime"].as<int>();
            if (conf.stopTime < 0)
                throw ::Parser::BadParaException();
        }
        if (node["stdout"])
            conf.stdout = node["stdout"].as<std::string>();
        if (node["stderr"])
            conf.stderr = node["stderr"].as<std::string>();
        if (node["env"]) {
            for (YAML::Node::const_iterator it = node["env"].begin(); it != node["env"].end(); it++)
                conf.env[it->first.as<std::string>()] = it->second.as<std::string>();
        }
        return true;
    }
};
}

# endif