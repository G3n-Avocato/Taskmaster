#ifndef PARSER_HPP
# define PARSER_HPP

#include "process.hpp"

#include <string>
#include <yaml-cpp/yaml.h>

namespace YAML {
template<>
struct convert<t_config> {
    static bool decode(const Node& node, t_config& conf) {
        if (!node.IsMap())
            return false;

        if (node["cmd"])
            conf.cmd = node["cmd"].as<std::string>();
        if (node["numprocs"])
            conf.numProcs = node["numprocs"].as<int>();
        if (node["umask"])
            conf.umask = node["umask"].as<std::string>();
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
        if (node["startretries"])
            conf.startRetries = node["startretries"].as<int>();
        if (node["starttime"])
            conf.startTime = node["starttime"].as<int>();
        if (node["stopsignal"])
            conf.stopSignal = node["stopsignal"].as<std::string>();
        if (node["stoptime"])
            conf.stopTime = node["stoptime"].as<int>();
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


class Parser {

    public:
        
        Parser(void);
        Parser(std::string config_file, std::map<std::string, t_config> &programs_tab);
        ~Parser(void);
    
    private:

};

# endif