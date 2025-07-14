# include "process.hpp"

#include <sys/types.h>
#include <unistd.h>
#include <cstring>

Process::Process(const t_config& config) : _processus(-2), _config(config)
{

}

Process::~Process(void) 
{
    if (_processus > 0)
    {
        stopProcess();
        if (isProcessUp())
        {
            killProcess();
        }
    }
}

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

std::vector<char*> Process::buildArgv(const std::string& cmd) 
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


std::vector<char*> Process::buildEnvp(const std::map<std::string, std::string>& envMap) 
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

void Process::freeCStringVector(std::vector<char*>& vec) {
    for (char* ptr : vec) {
        free(ptr);
    }
    vec.clear();
}

bool Process::startProcess()
{
    _processus = fork();

    if (_processus == -1) 
    {
        perror("fork");
        return false;
    }
    else if (_processus > 0) 
    {
        std::cout << "Parent process" << std::endl;
        return true;
    }
    else if (_processus == 0) 
    {
        std::cout << "Child process" << std::endl;
        umask(_config.umask);
        std::vector<char*> argv = buildArgv(_config.cmd);
        std::vector<char*> envp = buildEnvp(_config.env);
        execve(argv[0], argv.data(), envp.data());
        perror("execve failed");
        freeCStringVector(argv);
        freeCStringVector(envp);
        _exit(EXIT_FAILURE);
    }
}

bool Process::isProcessUp()
{
    return _processus > 0 && kill(_processus, 0) == 0;
}

int Process::stopProcess()
{
    if (_processus <= 0)
    {
        return 0;
    }

    int waited = 0;
    const int maxWait = _config.stoptime;

    kill(_processus, _custom_sig);
    while (waited < maxWait)
    {
        if (!isProcessUp())
        {
            return 1;
        } 
        sleep(1);
        waited++;
    }

    killProcess();
    return 2;
}

void Process::killProcess()
{
    if (_processus > 0)
    {
        kill(_processus, SIGKILL);
    }
}

pid_t Process::getPid() const 
{
    return _processus;
}