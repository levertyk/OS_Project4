// Operating Systems

// Keenan Leverty
// Colby Tiefenthaler

#include <iostream>
#include <string>
#include <fstream>
#include <cstring>
#include <stdexcept>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <vector>
#include <sys/stat.h>

using namespace std;

constexpr int MAX_HISTORY = 128;
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

// handles redirected inputs
// takes the command list, an optional input path, and optional output path
void executeRedirect(vector<string> commandList, string function, int functionIndex)
{
    // save original file descriptors
    int ogInfd = dup(0);
    int ogOutfd = dup(1);

    int inFd, outFd;

    char path[commandList[functionIndex + 1].size() + 1];
    path[sizeof(path)] = NULL;
    for (int i = 0; i < sizeof(path) - 1; i++)
    {
        path[i] = *commandList[functionIndex + 1].c_str();
    }

    // open input file
    if (function == "<")
    {
        inFd = open(path, O_WRONLY | O_CREAT, 0666); // TODO: this is probably wrong
    }
    else
    {
        outFd = open(path, O_WRONLY | O_CREAT, 0666); // TODO: this is probably wrong
    }

    // change the file descriptors to the new in/out fd
    if (inFd != NULL)
        dup2(inFd, 0);
    if (outFd != NULL)
        dup2(outFd, 1);

    commandList.pop_back();
    commandList.pop_back();

    char *commandListC[commandList.size() + 1];
    commandListC[commandList.size()] = nullptr;
    for (int i = 0; i < commandList.size(); i++)
    {
        commandListC[i] = (char *)commandList[i].c_str();
    } // run execute on command

    // execute command with new fd
    execute_command(commandListC);

    // cleanup when done (after wait)
    fflush(stdout);
    if (inFd != NULL)
    {
        dup2(ogInfd, 0);
        close(ogInfd);
    }
    if (outFd != NULL)
    {
        dup2(ogOutfd, 1);
        close(ogOutfd);
    }

    fflush(stdout);
}

void executePipe(int pipefd[2], char *cmd1[], char *cmd2[])
{
    pid_t pid;
    int status;

    if (pipe(pipefd) == -1)
    {
        cerr << "Error: could not create pipe." << endl;
        exit(EXIT_FAILURE);
    }

    pid = fork();

    if (pid == -1)
    {
        cerr << "Error: fork failed." << endl;
        exit(EXIT_FAILURE);
    }
    else if (pid == 0)
    { // chilled
        close(pipefd[0]);
        dup2(pipefd[1], STDOUT_FILENO); // change the stdout to be the pipe

        if (execvp(cmd1[0], cmd1))

            exit(EXIT_FAILURE); // Exit child process if execvp fails}
    }
    else
    { // parnet
        close(pipefd[1]);
        dup2(pipefd[0], STDIN_FILENO);
        if (execvp(cmd2[0], cmd2))
            exit(EXIT_FAILURE);
    }
}

// looks for >,< or \ tokens in the array
void execute_parse(vector<string> commandList)
{
    string function;
    int pointer = -1;

    // finds either the < > or |
    // saves the location in the pointer var
    // saves the character as function
    for (int i = 0; i < commandList.size(); i++)
    {
        if (commandList[i] == "<" || commandList[i] == ">" || commandList[i] == "|")
        {
            function = commandList[i];
            pointer = i;
        }
    }

    char *commandListC[commandList.size() + 1];
    commandListC[commandList.size()] = nullptr;
    for (int i = 0; i < commandList.size(); i++)
    {
        commandListC[i] = (char *)commandList[i].c_str();
    } // run execute on command

    // If no special characters are found, execute the command normally
    if (pointer < 0)
    {
        execute_command(commandListC);
    }
    else // special command found
    {

        if (function != "|") // change input
        {                    // gives it the first command, ignores the function char, then gives it the path
            executeRedirect(commandList, function, pointer);
        }
        else // TODO: impelment pipe
        {    // gives it the first command, ignores the function, then gives it the second command
            int pipefd[2];
            executePipe(pipefd, (char *)commandList[0].c_str(), (char *)commandList[2].c_str());
        }
    }
}

int main()
{
    string command;
    char *args[100];
    char directory[100];

    cout << "Simple Shell. Type 'exit' to quit." << endl;

    while (true)
    {
        fflush(stdout);
        getcwd(directory, sizeof(directory));
        cout << directory << "$ ";
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
        else if (command == "history")
        {
            display_history();
        }
        else
        { // if no match run command with execvp
            // use vector to tokenize data
            vector<string> commandList;
            commandList = tokenize(command);
            // change to c string for execution

            execute_parse(commandList);
        }
    }

    return 0;
}
