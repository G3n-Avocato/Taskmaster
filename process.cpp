# include "process.hpp"

Process::Process(int i, std::string name, const t_config& config, ProcessStatus stat) : _id(i), _name(name), _config(config)
{
    {
        std::lock_guard<std::mutex> lock(this->_status_mutex);
        this->_status = stat;
        this->_exit_code = false;
    }

    this->_exec.argv = buildArgv(this->_config.cmd);
    this->_exec.envp = buildEnvp(this->_config.env);

    this->_count_retries = 0;
    this->_run_reached = false;

    buildStd_process(this->_config.stdout);
    buildStd_process(this->_config.stderr);

    //this->count_restart = 0;
    //this->start_autorestart = 0;
}

Process::~Process(void) 
{
    stopThread();
    {
        std::lock_guard<std::mutex> lock(this->_status_mutex);
        std::cout << "Destruction " << this->_id << " : " << enumtoString(this->_status) << std::endl;
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
        default: return "default";
    }
}

bool Process::startProcess()
{
    open_file_std();
    {    
        std::lock_guard<std::mutex> lock(this->_start_mutex);            
        this->_start_run = 0;
        this->_exit_code = false;
    }
    
    this->_processus = fork();
    if (this->_processus == -1) 
    {
        {    
            std::lock_guard<std::mutex> lock(this->_status_mutex);            
            this->_status = ProcessStatus::UNKNOWN;
        }
        perror("fork");
        return false;
    }
    else if (this->_processus == 0) {
        child_exec_process();
    }
    else if (this->_processus > 0) 
    {
        {
            std::lock_guard<std::mutex> lock(this->_status_mutex);
            this->_status = ProcessStatus::STARTING;
            //std::cout << " status from parent " << this->_id << " : " << enumtoString(this->_status) << std::endl;
        }

        if (this->_exec.fd_out != 1)
            close(this->_exec.fd_out);
        if (this->_exec.fd_err != 2)
            close(this->_exec.fd_err);
        
        this->_t1 = std::thread(&Process::thread_monitoring_status, this);
        
        std::cout << this->_processus << " Parent process " << this->_id << std::endl;
        return true;
    }
    return false;
}

void    Process::thread_monitoring_status() {
    int status; 
    
    while (true) {
        pid_t child_pid = waitpid(this->_processus, &status, WNOHANG);

        if (child_pid == -1) {
            {
                std::lock_guard<std::mutex> lock(this->_status_mutex);
                this->_status = ProcessStatus::UNKNOWN;
            }
            perror("waitpid error");
            break  ;
        }
        if (child_pid == 0) {
            {
                std::lock_guard<std::mutex> lock(this->_start_mutex);
                if (this->_start_run == 0)
                    this->_start_run = std::time(nullptr);
            }
        }
        else if (child_pid == this->_processus) {
            {
                std::lock_guard<std::mutex> lock(this->_status_mutex);
                if (WIFEXITED(status)) {
                    this->_status = ProcessStatus::EXITED;
                    
                    for (size_t i = 0; i < this->_config.exitCodes.size(); i++) {                        
                        if (WEXITSTATUS(status) == this->_config.exitCodes[i]) {
                            {
                                std::lock_guard<std::mutex> lock(this->_exitcode_mutex);
                                this->_exit_code = true;
                            }
                        }
                    }
                }
                else if (WIFSIGNALED(status)) //a revoir avec le bon exit status 
                    this->_status = ProcessStatus::EXITED;
                else
                    this->_status = ProcessStatus::EXITED;
            }
            break ;
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

    execve(this->_exec.argv[0], this->_exec.argv.data(), this->_exec.envp.data());
    //if (execve(this->_exec.argv[0], this->_exec.argv.data(), this->_exec.envp.data()) == -1) {
    //    std::cerr << "Execve failed : " << strerror(errno) << std::endl;
    //    freeCStringVector(this->_exec.argv);
    //    freeCStringVector(this->_exec.envp);
    //    this->~Process();
    //    exit(EXIT_FAILURE);
    //}
    freeCStringVector(this->_exec.argv);
    freeCStringVector(this->_exec.envp);
    exit(EXIT_FAILURE);
}

bool    Process::stopThread() {
    if (this->_t1.joinable())
        this->_t1.join();
    return true ;
}

bool Process::isProcessUp()
{
    return this->_processus > 0 && kill(this->_processus, 0) == 0;
}



///////////////////////////////////////////////////
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