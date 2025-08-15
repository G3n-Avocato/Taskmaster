# include "process.hpp"

Process::Process(int i, std::string name, const t_config& config) : _id(i), _name(name), _config(config)
{
    this->_status = ProcessStatus::STOPPED;
    this->_exit_code = false;

    //this->_exec.argv = buildArgv(this->_config.cmd);
    //this->_exec.envp = buildEnvp(this->_config.env);

    this->_count_retries = 0;
    this->_run_reached = false;

    buildStd_process(this->_config.stdout);
    buildStd_process(this->_config.stderr);

}

Process::Process(Process const &src) {
    this->_processus = src._processus ;
    this->_id = src._id ;
    this->_name = src._name ;
    this->_config = src._config ;
    
    this->_exec.fd_out = src._exec.fd_out ;
    this->_exec.fd_err = src._exec.fd_err ;
    //freeCStringVector(this->_exec.argv);
    //freeCStringVector(this->_exec.envp);
    //this->_exec.argv = buildArgv(src._config.cmd);
    //this->_exec.envp = buildEnvp(src._config.env);  

    this->_status = src._status ;
    this->_start_run = src._start_run ;
    this->_exit_code = src._exit_code ;
    this->_run_reached = src._run_reached ;
    this->_count_retries = src._count_retries ;
}

Process&    Process::operator=(Process const &rhs) {
    if (this != &rhs) {
        this->_processus = rhs._processus ;
        this->_id = rhs._id ;
        this->_name = rhs._name ;
        this->_config = rhs._config ;
        
        this->_exec.fd_out = rhs._exec.fd_out ;
        this->_exec.fd_err = rhs._exec.fd_err ;
        //freeCStringVector(this->_exec.argv);
        //freeCStringVector(this->_exec.envp);
        //this->_exec.argv = buildArgv(rhs._config.cmd);
        //this->_exec.envp = buildEnvp(rhs._config.env);  

        this->_status = rhs._status ;
        this->_start_run = rhs._start_run ;
        this->_exit_code = rhs._exit_code ;
        this->_run_reached = rhs._run_reached ;
        this->_count_retries = rhs._count_retries ;
    }
    return *this ;
}

Process::~Process(void) 
{
    std::cout << "Destruction " << this->_id << " : " << enumtoString(this->_status) << std::endl;
    //freeCStringVector(this->_exec.argv);
    //freeCStringVector(this->_exec.envp);
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
    this->_start_run = 0;
    this->_exit_code = false;
    
    this->_processus = fork();

    if (this->_processus == -1) 
    {         
        this->_status = ProcessStatus::UNKNOWN;
        perror("fork");
        return false;
    }
    else if (this->_processus == 0) {
        child_exec_process();
    }
    else if (this->_processus > 0) 
    {
        this->_status = ProcessStatus::STARTING;

        if (this->_exec.fd_out != 1)
            close(this->_exec.fd_out);
        if (this->_exec.fd_err != 2)
            close(this->_exec.fd_err);
        
        
        std::cout << this->_processus << " Parent process " << this->_id << std::endl;
        return true;
    }
    return false;
}

void    Process::child_exec_process() {
    //std::cout << "Child process " << this->_id << std::endl;

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
    
    std::vector<char *> argv = buildArgv(this->_config.cmd);
    std::vector<char *> envp = buildEnvp(this->_config.env);

    if (execve(argv[0], argv.data(), envp.data()) == -1) {
        
        //std::cerr << "Execve failed : " << strerror(errno) << std::endl;
        freeCStringVector(argv);
        freeCStringVector(envp);
        exit(EXIT_FAILURE);
        //throw std::runtime_error("Error exec process : " + std::string(strerror(errno)) + " | " + std::to_string(this->_processus)); 
    }

    freeCStringVector(argv);
    freeCStringVector(envp);
    exit(EXIT_FAILURE);
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