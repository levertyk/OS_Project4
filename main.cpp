// Operating Systems

// Keenan Leverty
// Colby Tiefenthaler
// Damien Hess

#include <iostream>
#include <string>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <unistd.h>
#include <sys/wait.h>
#include <vector>

using namespace std;

const int MAX_HISTORY = 128;
vector<string> history;

int num_commands = 0;

// TOKENIZER
vector<string> tokenize(string commands)
{
    vector<string> commandList;
    // split commands delimited by spaces into commandList
    string delim = " ";
    while (commands != "")
    {

        if (commands.find(delim) != string::npos)
        {
            commandList.push_back(commands.substr(0, commands.find(delim)));
            commands = commands.substr(commands.find(delim) + 1, string::npos);
        }
        else
        {
            commandList.push_back(commands.substr(0, string::npos));
            commands = "";
        }
    }

    return commandList;
}

void add_history(string command)
{
    if (history.size() >= MAX_HISTORY)
    {
        history.erase(history.begin());
    }
    history.push_back(command);
    num_commands++;
}

void display_history()
{
    for (int i = 0; i < history.size(); i++)
    {
        cout << i + 1 << ". " << history[i] << endl;
    }
}

void execute_command(char *args[])
{
    pid_t pid = fork();
    if (pid == -1) // bad
    {
        cerr << "Failed to fork child process" << endl;
        exit(EXIT_FAILURE);
    }
    else if (pid == 0)
    { // child

        if (execvp(args[0], args) == -1)
        {
            perror("Error executing command");
            exit(EXIT_FAILURE);
        }
    }
    else // parnet
    {
        int status;
        waitpid(pid, &status, 0);
    }
}

int main()
{
    string command;
    char *args[100];

    cout << "Simple Shell. Type 'exit' to quit." << endl;

    while (true)
    {
        cout << "$ ";
        getline(cin, command);
        if (command.empty())
        {
            continue;
        }
        add_history(command);

        if (command == "exit")
        {
            break;
        }
        else if (command == "pwd")
        {
        }
        else if (command == "history")
        {
            display_history();
        }
        else if (command.substr(0, 2) == "./")
        {
        }
        else if (command.substr(0, 5) == "java ")
        {
        }
        else
        { // if no match run command with execvp
            // use vector to tokenize data
            vector<string> commandList;
            commandList = tokenize(command);
            // change to c string for execution
            char *commandListC[commandList.size() + 1];
            commandListC[commandList.size()] = NULL;
            for (int i = 0; i < commandList.size(); i++)
            {
                commandListC[i] = (char *)commandList[i].c_str();
            } // run execute on command

            cout << "hello world" << endl;
            execute_command(commandListC);
        }
    }

    return 0;
}
