#include "parser.hpp"

Parser::Parser(void) {}

Parser::Parser(std::string config_file) {
    YAML::Node  config = YAML::LoadFile(config_file);



}

Parser::~Parser(void) {

}