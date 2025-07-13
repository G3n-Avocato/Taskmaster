#ifndef PARSER_HPP
# define PARSER_HPP

#include "supervisor.hpp"

#include <string>
#include <yaml-cpp/yaml.h>

namespace YAML {
template<>
struct convert<Config> {
    static
}
}

class Parser {

    public:
        
        Parser(void);
        Parser(std::string config_file);
        ~Parser(void);
    
    private:

};

# endif