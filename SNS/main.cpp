#include <iostream>
#include "server.h"

#ifdef __linux
    #include <fstream>
    #include <cstring>
    #include <unistd.h>
    #include <signal.h>
    #include <cstdio>
    #include <sys/types.h>
    #include <sys/stat.h>
#endif

/*
*	SNS - Simple Name Server
*	SNS позволяет выполнять подмену ответов DNS сервера на запросы A, CNAME, SOA, MX
*/

pid_t getDaemonPid()
{
    using namespace std;

    unsigned long pid = 0;

    ifstream ifs("pid.log", ifstream::in);
    if(ifs.is_open())
    {
        ifs >> pid;
        ifs.close();
    }
    return static_cast<pid_t>(pid);
}

void putDaemonPid(pid_t pid)
{
    using namespace std;

    ofstream ofs("pid.log", ofstream::out);
    if(ofs.is_open())
    {
        ofs << static_cast<unsigned long>(pid);
        ofs.close();
    }
}

int main(int argc, char* argv[])
{
    using namespace std;
    using namespace SNS;

    if (argc < 2)
    {
        cout << "Config file not selected. Run: " << argv[0] << " <config file>" << endl;
        return 0;
    }

    #ifdef __linux
        if(argc < 3)
            cout << "For run this program as daemon type: " << argv[0] << " <config file> daemon" << endl;
        else if(!strcmp(argv[2],"daemon"))
        {
            pid_t curpid = getDaemonPid();
            if(curpid && !kill(curpid,0))
            {
                cout << "Program already running. For stop type: " << argv[0] << " <config file> stop" << endl;
                return 0;
            }
            else
            {
                pid_t ppid = fork();
                if(ppid == -1)
                {
                    cout << "Daemonization error. Try run this program without flag daemon" << endl;
                    return -1;
                }
                else if(!ppid)
                {
                    freopen("daemon.log", "a", stdout);
                    freopen("daemon.log", "a", stderr);
                    fclose(stdin);

                    umask(0);
                    setsid();

                    try
                    {
                        DNServer& server = DNServer::getInstance(string(argv[1]));
                        chdir("/");
                        server.mainLoop();
                    }
                    catch (const exception& e)
                    {
                        cerr << "Error: " << e.what() << endl;
                    }
                    catch (...)
                    {
                        cerr << "Undetected critical error" << endl;
                    }
                    return 0;
                }
                else
                {
                    putDaemonPid(ppid);
                    cout << "Daemonization ok. For stop daemon type: " << argv[0] << " <config file> stop" << endl;
                    return 0;
                }
            }
        }
        else if(!strcmp(argv[2],"stop"))
        {
            if(!kill(getDaemonPid(), SIGKILL))
                cout << "Daemon killed" << endl;
            else
                cout << "Daemon not running" << endl;
            return 0;
        }
        else
        {
             cout << "Command not recognized" << endl;
             return 0;
        }
    #endif

    try
    {
        DNServer& server = DNServer::getInstance(string(argv[1]));
        server.mainLoop();
    }
    catch (const exception& e)
    {
        cerr << "Error: " << e.what() << endl;
    }
    catch (...)
    {
        cerr << "Undetected critical error" << endl;
    }
    return 0;
}
