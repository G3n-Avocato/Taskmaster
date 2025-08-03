# include "process.hpp"

Process::Process(int i, std::string name, const t_config& config, ProcessStatus stat) : _id(i), _name(name), _config(config)
{
    {
        std::lock_guard<std::mutex> lock(this->_status_mutex);
        this->_status = stat;
    }
    //init_string_workDir(name);
    if (!open_file_std()) 
        exit(1); // GESTION D'ERREUR A REVOIR
    this->_exec.argv = buildArgv(this->_config.cmd);
    this->_exec.envp = buildEnvp(this->_config.env);
}

Process::~Process(void) 
{
    if (this->_t1.joinable())
        this->_t1.join();

    {
        std::lock_guard<std::mutex> lock(this->_status_mutex);
        std::cout << " in destruction " << this->_id << " : " << enumtoString(this->_status) << std::endl;
    }
    freeCStringVector(this->_exec.argv);
    freeCStringVector(this->_exec.envp);
    //if (_processus > 0)
    //{
    //    stopProcess();
    //    if (isProcessUp())
    //    {
    //        killProcess();
    //    }
    //}
}

bool    Process::open_file_std() {
    size_t  pos;

    if (!this->_config.stdout.empty()) {
        if (this->_config.numProcs > 1) {
            pos = this->_config.stdout.find('.');
            this->_config.stdout.insert(pos, "_");
            pos = this->_config.stdout.find('.');
            this->_config.stdout.insert(pos, std::to_string(this->_id));
        }
        this->_exec.fd_out = open(this->_config.stdout.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (this->_exec.fd_out < 0) {
            perror("open fd_out");
            return false ;
        }
    }
    else 
        this->_exec.fd_out = 1;
    if (!this->_config.stderr.empty()) {
        if (this->_config.numProcs > 1) {
            pos = this->_config.stderr.find('.');
            this->_config.stderr.insert(pos, "_");
            pos = this->_config.stderr.find('.');
            this->_config.stderr.insert(pos, std::to_string(this->_id));
        }
        this->_exec.fd_err = open(this->_config.stderr.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);

        if (this->_exec.fd_err < 0) {
            perror("open fd_err");
            return false ;
        }
    }
    else 
        this->_exec.fd_err = 2;
    return true ;
}

// FCT PRINT ENUM STATUS TEST // 
const char* enumtoString(ProcessStatus stat) {
    switch (stat) {
        case STOPPED: return "STOPPED";
        case STARTING: return "STARTING";
        case RUNNING: return "RUNNING";
        case BACKOFF: return "BACKOFF";
        case STOPPING: return "STOPPING";
        case EXITED: return "EXITED";
        case FATAL: return "FATAL";
        case UNKNOWN: return "UNKNOWN";
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
    this->_processus = fork();    

    if (this->_processus == -1) 
    {
        {    
            std::lock_guard<std::mutex> lock(this->_status_mutex);            
            this->_status = ProcessStatus::STOPPED;
        }
        perror("fork");
        return false;
    }
    else if (this->_processus == 0) {
        child_exec_process();
    }
    else if (this->_processus > 0) 
    {
        std::time_t start = std::time(nullptr);
        std::time_t end = std::time(nullptr);
        double      diff = difftime(end, start);
        if (startsec >= diff)
            //good 

        if (this->_exec.fd_out != 1)
            close(this->_exec.fd_out);
        if (this->_exec.fd_err != 2)
            close(this->_exec.fd_err);
        
        
        std::cout << "Parent process " << this->_id << std::endl;
        {
            std::lock_guard<std::mutex> lock(this->_status_mutex);
            this->_status = ProcessStatus::STARTING;
            std::cout << " set starting from parent " << this->_id << " : " << enumtoString(this->_status) << std::endl;
        }
        
        this->_t1 = std::thread(&Process::thread_monitoring_status, this);
        
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
                this->_status = ProcessStatus::RUNNING;
            }
        }
        else if (child_pid == this->_processus) {
            {
                std::lock_guard<std::mutex> lock(this->_status_mutex);
                if (WIFEXITED(status))
                    this->_status = ProcessStatus::EXITED;
                else if (WIFSIGNALED(status)) //a revoir avec le bon exit status 
                    this->_status = ProcessStatus::EXITED;
                else
                    this->_status = ProcessStatus::EXITED;
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

void    Process::child_exec_process() {
    std::cout << "Child process " << this->_id << std::endl;

    umask(this->_config.umask);
    if (chdir(this->_config.workingDir.c_str()) < 0) {
        perror("chdir");
        exit(EXIT_FAILURE);
    }
    if (dup2(this->_exec.fd_out, STDOUT_FILENO) < 0) {
        perror("dup2 stdout");
        exit(EXIT_FAILURE);
    }
    if (dup2(this->_exec.fd_err, STDERR_FILENO) < 0) {
        perror("dup2 stderr");
        exit(EXIT_FAILURE);
    }

    if (this->_exec.fd_out != 1)
        close(this->_exec.fd_out);
    if (this->_exec.fd_err != 2)
        close(this->_exec.fd_err);

    //std::vector<char*> argv = buildArgv(this->_config.cmd);
    //std::vector<char*> envp = buildEnvp(this->_config.env);
    
    if (execve(this->_exec.argv[0], this->_exec.argv.data(), this->_exec.envp.data()) == -1) {
        std::cerr << "Execve failed : " << strerror(errno) << std::endl;
        freeCStringVector(this->_exec.argv);
        freeCStringVector(this->_exec.envp);
        //this->~Process();
        exit(EXIT_FAILURE);
    }
    freeCStringVector(this->_exec.argv);
    freeCStringVector(this->_exec.envp);
    exit(EXIT_FAILURE);
}

/// fonction inutile chaque processus a son env et normalement se croise pas
/// creer les repertoire dans le parent pas seulement dans l'enfant
//void    Process::init_workDir() {
    
    //if (chdir(this->_config.workingDir.c_str()) < 0) {
    //    perror("chdir");
    //    exit(EXIT_FAILURE);
    //}
    //std::string     nameDir = this->_name + "_" + std::to_string(this->_id);
    //std::filesystem::create_directory(nameDir);
    //if (chdir(nameDir.c_str()) < 0) {
    //    perror("chdir");
    //    exit(EXIT_FAILURE);
    //}

    ///
    //std::filesystem::path cwd = std::filesystem::current_path();
    //std::cout << "current path : " << cwd << std::endl;
//}


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