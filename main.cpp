// Operating Systems

// Keenan Leverty
// Colby Tiefenthaler

#include <iostream>
#include <string>
#include <fstream>
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
            pointer = i + 1; // next character, start of second part of the command
        }
    }

    char *commandListC[commandList.size() + 1];
    commandListC[commandList.size()] = NULL;
    for (int i = 0; i < commandList.size(); i++)
    {
        commandListC[i] = (char *)commandList[i].c_str();
    } // run execute on command

    if (pointer < 0)
    {

        execute_command(commandListC);
    }
    else
    {
        if (function == ">") // change output
        {
            executeRedirect(commandListC, "", commandList[pointer]);
        }
        else if (function == "<") // change input
        {
            executeRedirect(commandListC, commandList[pointer]);
        }
        else if (function == "|") // TODO: impelment pipe
        {
            cout << "|" << endl;
        }
    }
}

// handles redirected inputs
// takes the command list, an optional input path, and optional output path
void executeRedirect(char *commandList[], string inPath = "", string outPath = "")
{
    // save original file descriptors
    int ogIfd = dup(0);
    int ogOfd = dup(1);

    int inFd, outFd;

    // open input file
    if (inPath != "")
    {
        inFd = open(inPath, fstream::in | fstream::app); // TODO: this is probably wrong
    }

    // open output file
    if (outPath != "")
    {
        outFd = open(outPath, fstream::out | fstream::app); // TODO: this is probably wrong
    }

    // change the file descriptors to the new in/out fd
    if (inFd != NULL)
        dup2(inFd, 0);
    if (outFd != NULL)
        dup2(outFd, 1);

    // execute command with new fd
    execute_command(commandList);

    // cleanup when done (after wait)
    fflush(stdout);
    if (inFd != NULL)
    {
        dup2(ogIfd, 0);
        close(ogIfd);
    }
    if (outFd != NULL)
    {
        dup2(ogOfd, 1);
        close(ogOfd);
    }

    fflush(stdout);
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
