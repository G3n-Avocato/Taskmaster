# include "process.hpp"

#include <sys/types.h>
#include <unistd.h>
#include <cstring>
#include <thread>
#include <sys/wait.h>

Process::Process(const t_config& config, std::mutex& mutex, ProcessStatus *processStatus) : _config(config), _statusMutex(mutex), _processStatus(processStatus)
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

void Process::startProcess(int retry)
{
    _processus = fork();
    (void) retry;
    if (_processus == -1) 
    {
        perror("fork");
        std::unique_lock<std::mutex> lock(_statusMutex);
        *_processStatus = ProcessStatus::DOWN;
        lock.unlock();
        if (retry > 0)
            startProcess(--retry);
    }
    else if (_processus > 0) 
    {
        std::cout << "Parent process" << std::endl;
        std::thread([this, retry]() {
            int waited = 0;
            const int maxWait = _config.startTime;

            while (waited < maxWait)
            {
                if (this->isProcessUp())
                {
                    std::unique_lock<std::mutex> lock(_statusMutex);
                    *_processStatus = ProcessStatus::START;
                    lock.unlock();
                    std::cout << "Process start gracefully." << std::endl;
                }
                std::this_thread::sleep_for(std::chrono::seconds(1));
                waited++;
            }
            if (waited == maxWait && retry > 0)
            {
                startProcess(retry -1);
            }
        }).detach();
    }
    else if (_processus == 0) 
    {
        std::cout << "Child process" << std::endl;
        //::umask(_config.umask);
        std::vector<char*> argv = buildArgv(_config.cmd);
        std::vector<char*> envp = buildEnvp(_config.env);
        
        execve(argv[0], argv.data(), envp.data());
        
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

    kill(_processus, _config.stopSignal);
    std::thread([this]() {
        int waited = 0;
        const int maxWait = _config.stopTime;

        while (waited < maxWait)
        {
            if (!this->isProcessUp())
            {
                int status;
                waitpid(_processus, &status, 0);
                std::unique_lock<std::mutex> lock(_statusMutex);
                *_processStatus = ProcessStatus::DOWN;
                lock.unlock();
                std::cout << "Process stopped gracefully." << std::endl;
                return;
            }
            std::this_thread::sleep_for(std::chrono::seconds(1));
            waited++;
        }
        
        std::cout << "Timeout reached. Killing process." << std::endl;
        this->killProcess();
        int status;
        waitpid(_processus, &status, 0);
        std::unique_lock<std::mutex> lock(_statusMutex);
        *_processStatus = ProcessStatus::UNEXPECT_DOWN;
        lock.unlock();
    }).detach();
    return 2;
}
/*
int Process::checkExitCode()
{
    if (_processus <= 0)
    {
        return -1;
    }

    int waited = 0;
    const int maxWait = _config.stopTime;

    kill(_processus, _config.stopSignal);
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
*/

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