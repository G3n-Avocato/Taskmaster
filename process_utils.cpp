# include "process.hpp"

std::vector<std::string> splitWords(const std::string& input) 
{
    std::vector<std::string> words;
    size_t start = 0;
    size_t end = 0;

    while (start < input.length()) 
    {
        while (start < input.length() && std::isspace(input[start])) {
            start++;
        }
        if (start >= input.length()) 
        {
            return words;
        }
        end = start;
        while (end < input.length() && !std::isspace(input[end])) 
        {
            end++;
        }
        words.push_back(input.substr(start, end - start));
        start = end;
    }
    return words;
}

std::vector<char*>  Process::buildArgv(const std::string& cmd) 
{
    std::vector<std::string> words = splitWords(cmd);
    std::vector<char*> argv;

    for (const std::string& word : words) 
    {
        argv.push_back(strdup(word.c_str()));
    }
    argv.push_back(nullptr);
    return argv;
}


std::vector<char*>  Process::buildEnvp(const std::map<std::string, std::string>& envMap) 
{
    std::vector<char*> envp;

    for (auto it = envMap.begin(); it != envMap.end(); ++it) 
    {
        std::string entry = it->first + "=" + it->second;
        envp.push_back(strdup(entry.c_str()));
    }

    envp.push_back(nullptr);
    return envp;
}

void    Process::freeCStringVector(std::vector<char*>& vec) {
    for (char* ptr : vec) {
        free(ptr);
    }
    vec.clear();
}

bool    Process::buildStd_process(std::string& str, int& fd) {
    size_t pos;
    
    if (!str.empty()) {
        if (this->_config.numProcs > 1) {
            pos = str.find('.');
            str.insert(pos, "_");
            pos = str.find('.');
            str.insert(pos, std::to_string(this->_id));
        }
        fd = open(str.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (fd < 0) {
            perror("open std");
            return false ;
        }
        return true ;
    }
    return false ;
}

void    Process::open_file_std() {
    if (!buildStd_process(this->_config.stdout, this->_exec.fd_out))
        this->_exec.fd_out = 1;
    if (!buildStd_process(this->_config.stderr, this->_exec.fd_err))
        this->_exec.fd_err = 2;
}