# include "process.hpp"

Process::Process(const t_config& config, ProcessStatus stat) : _config(config) 
{
    std::lock_guard<std::mutex> lock(this->_status_mutex);
    this->_status = stat;
}

Process::~Process(void) 
{
    if (this->_t1.joinable())
        this->_t1.join();

    {
        std::lock_guard<std::mutex> lock(this->_status_mutex);
        std::cout << " in destruction : " << enumtoString(this->_status) << std::endl;
    }
    //if (_processus > 0)
    //{
    //    stopProcess();
    //    if (isProcessUp())
    //    {
    //        killProcess();
    //    }
    //}
}

// FCT PRINT ENUM STATUS TEST // 
const char* enumtoString(ProcessStatus stat) {
    switch (stat) {
        case START: return "START";
        case DOWN: return "DOWN";
        case UNEXPECT_DOWN: return "UNEXPECT_DOWN";
        case SHUTING_DOWN: return "SHUTING_DOWN";
        case STARTING: return "STARTING";
        default: return "unknown";
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

/////////////////////////
bool Process::startProcess()
{
    //{
    //    std::lock_guard<std::mutex> lock(this->_status_mutex);
    //    this->_status = ProcessStatus::STARTING;
    //    std::cout << " after fork : " << enumtoString(this->_status) << std::endl;
    //}
    this->_processus = fork();    

    if (this->_processus == -1) 
    {
        {    
            std::lock_guard<std::mutex> lock(this->_status_mutex);            
            this->_status = ProcessStatus::DOWN;
        }
        perror("fork");
        return false;
    }
    else if (this->_processus == 0) 
    {
        std::cout << "Child process" << std::endl;

        std::vector<char*> argv = buildArgv(this->_config.cmd);
        std::vector<char*> envp = buildEnvp(this->_config.env);

        // TEST argv et envp sont valides //
        //std::vector<char*>::iterator it = argv.begin();
        //for ( ; it != argv.end(); it++)
        //    std::cout << "vector argv data : " << *it << std::endl;
        //std::vector<char*>::iterator itt = envp.begin();
        //for ( ; itt != envp.end(); itt++)
        //    std::cout << "vector envp data : " << *itt << std::endl;


        if (execve(argv[0], argv.data(), envp.data()) == -1)
            std::cerr << "Execve failed : " << strerror(errno) << std::endl;
        freeCStringVector(argv);
        freeCStringVector(envp);
        exit(EXIT_FAILURE);
    }
    else if (this->_processus > 0) 
    {
        std::cout << "Parent process" << std::endl;
        {
            std::lock_guard<std::mutex> lock(this->_status_mutex);
            this->_status = ProcessStatus::STARTING;
            std::cout << " set starting from parent : " << enumtoString(this->_status) << std::endl;
        }
        
        this->_t1 = std::thread(&Process::thread_monitoring_status, this);
        //std::cout << " apres lancement thread t1 : " << enumtoString(this->_status) << std::endl;

        return true;
    }
    return false;
}

void    Process::thread_monitoring_status() {
    int status; 
    
    while (true) {
        pid_t child_pid = waitpid(this->_processus, &status, WNOHANG);

        if (child_pid == -1) {
            perror("waitpid error");
            break  ;
        }
        if (child_pid == 0) {
            {
                std::lock_guard<std::mutex> lock(this->_status_mutex);
                this->_status = ProcessStatus::START;
            }
        }
        else if (child_pid == this->_processus) {
            {
                std::lock_guard<std::mutex> lock(this->_status_mutex);
                if (WIFEXITED(status))
                    this->_status = ProcessStatus::DOWN;
                else if (WIFSIGNALED(status)) //a revoir avec le bon exit status 
                    this->_status = ProcessStatus::UNEXPECT_DOWN;
                else
                    this->_status = ProcessStatus::UNEXPECT_DOWN;
            }
            break ;
        }
        {
            std::lock_guard<std::mutex> lock(this->_status_mutex);
            std::cout << " set statut start ou down in thread  : " << enumtoString(this->_status) << std::endl;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

bool Process::isProcessUp()
{
    return this->_processus > 0 && kill(this->_processus, 0) == 0;
}

//////////////////////////
int Process::stopProcess()
{
    if (_processus <= 0)
    {
        return 0;
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

void Process::killProcess()
{
    if (this->_processus > 0)
    {
        kill(this->_processus, SIGKILL);
    }
}

pid_t Process::getPid() const 
{
    return this->_processus;
}

const char* Process::getStatus() const {
    {
        std::lock_guard<std::mutex> lock(this->_status_mutex);
        const char* str = enumtoString(this->_status);
        return str ; 
    }
}