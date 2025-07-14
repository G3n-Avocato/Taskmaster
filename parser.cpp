#include "parser.hpp"

Parser::Parser(void) {}

Parser::Parser(std::string config_file, std::map<std::string, Config> &programs_tab) {
    YAML::Node  file_node = YAML::LoadFile(config_file);

    if (file_node["programs"]) {
        YAML::Node  programs_node = file_node["programs"];
        
        for (YAML::const_iterator it = programs_node.begin(); it != programs_node.end(); it++) {    
            
            std::string progName = it->first.as<std::string>();

            Config  conf_struct;
            if (YAML::convert<Config>::decode(it->second, conf_struct))
                programs_tab[progName] = conf_struct;
            else
                std::cout << "Erreur convert decode YAML to struct config\n";
        }

    }


}

Parser::~Parser(void) {

}