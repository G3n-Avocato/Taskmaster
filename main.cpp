#include "supervisor.hpp"
#include "parser.hpp"

std::string     enumAutoRestart(StateRestart tmp) {
    switch (tmp) {
        case True: return "true";
        case False: return "false";
        case Unexpected: return "unexpected";
        default: return "default";
    }
}

int main (int argc, char** argv) {

    if (argc != 2)
        return (1);

    try {
        std::map<std::string, t_config>   programs_tab;
        Parser  parser(argv[1], programs_tab);


/////////////////////////////////////////////////////////////////////
//PRINT STRUCT TEST                                                //
/////////////////////////////////////////////////////////////////////
        std::cout << "NB de bloc : " << programs_tab.size() << std::endl;
        for (std::map<std::string, t_config>::iterator it = programs_tab.begin(); it != programs_tab.end(); it++) {
            std::cout << "Program: " << it->first << std::endl;
            std::cout << "\tcmd: " << it->second.cmd << std::endl;
            std::cout << "\tnumProcs: " << it->second.numProcs << std::endl;
            std::cout << "\tumask: " << it->second.umask << std::endl;
            std::cout << "\tworkingDir: " << it->second.workingDir << std::endl;
            std::cout << "\tautoStart: " << it->second.autoStart << std::endl;
            std::cout << "\tautoRestart: " << enumAutoRestart(it->second.autoRestart) << std::endl;

            std::cout << "\texitcodes: " << std::endl;
            for (size_t i = 0; i < it->second.exitCodes.size(); i++)
                std::cout << "\t\t : " << it->second.exitCodes[i] << std::endl;

            std::cout << "\tstartRetry: " << it->second.startRetries << std::endl;
            std::cout << "\tstartTime: " << it->second.startTime << std::endl;
            std::cout << "\tstopSignal: : " << it->second.stopSignal << std::endl;
            std::cout << "\tstopTime: " << it->second.stopTime << std::endl;
            std::cout << "\tstdout: " << it->second.stdout << std::endl;
            std::cout << "\tstderr: " << it->second.stderr << std::endl;

            std::cout << "\tenv: " << std::endl;
            for (std::map<std::string, std::string>::iterator iter = it->second.env.begin(); iter != it->second.env.end(); iter++)
                std::cout << "\t\t" << iter->first << " : " << iter->second << std::endl;

        }
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

        Supervisor test1(programs_tab);

        //test1.processStart();

    } catch (const YAML::BadFile &e) {
        std::cerr << "Unable to open YAML file : " << e.what() << std::endl;
        return (1);
    } catch (const Parser::BadParaException &e) {
        std::cerr << "Error Exception : " << e.what() << std::endl;
        return (1);
    } catch (const Parser::RequiredParaException &e) {
        std::cerr << "Error Exception : " << e.what() << std::endl;
        return (1);
    } catch (const YAML::Exception &e) {
        std::cerr << "Error Exception : " << e.what() << std::endl;
        return (1);
    }


}