Possible Grade: 30 Points

**Languages used:** C/C++

**Operating Systems:** Linux Only

**Expected Materials to submit** Source code files.

**Members per team:** **1 to 3 Max. Each member must submit a copy that includes the names of the members commented in the code.** 



**I. Project Description**

The object of this assignment is to practice and gain reasonable experience with some advanced system and OS programming techniques such as process creation and management, file descriptors, Input/Output redirection, inter-process communications, pipes, etc.  To accomplish so, you will be designing and developing your own command shell (similar to the terminal you typically use in Ubuntu).  So, this is a learning opportunity before it is an assessment component. Please pay attention to the tips in the gray boxes in this document.

As a starting point, use the exercises explained in our class meetings on how to create processes and how to make them execute different programs or commands.

**II. Requirements and Expected Features**

Although I am not expecting that you will be implementing a shell with the full services as the commercial edition (*due to time, resource constraints, and course scope*), I would love for you to implement as many features as your time allows and by the deadline.  Below are some of the features I expect you to implement in your shell: 

**Things to consider before you start**: 

- The Shell exits if the user enters the command *exit*. This can easily be handled with a string comparison.
- You may assume that each item in the command string is separated, on either side, by at least one space (e.g., **myProgram  >  outfile** rather than myProgram >outfile). 

1: Printing the current directory (**pwd**)

**Tip**: Try *getcwd()*  function in C/C++. Literature is rich with examples. 

2: You should allow the user to run and specify commands (and arguments) either by relative or absolute pathnames. Example ( **ls -l**   ).

- Try ***execvp***. It will search the path automatically for you. The first argument should be a pointer to the command string and the second argument should be a pointer to a C-String array that contains the *command string as arg[0] and the other arguments as arg[1] through arg[n].* 
- *I explained this in class, and you worked on hands-on programs. See my slides and your records. to learn more about **execvp**, use the man command on Ubuntu.*

3: The History of Commands (history):

You should maintain a history of commands (including the invalid) previously entered by the user. Something similar to the system log file. You should create a queue (or list) that works in a FIFO fashion, and you should start numbering them with integer numbers starting by 1.  The number should be incremented as new lines are entered by the user (including the invalid commands, if any). 

- There is a history command in Linux. Try to do something similar by storing the non-empty commands in your dynamic list and printing it as needed. 
- Ideally, you create a file that is updated after the shell is terminated and uploaded when the user starts a new terminal session.

4: (input and output redirection) You should allow the shell to redirect default **STDIN** and **STDOUT** for the new processes by using **<** and **> operators**. However, you can go with the assumption that the user will either redirect the input or the output but not both.

**For example**, **./myExe < infile > outfile** would create a new process to run myExe and assign STDIN for the new process to **infile** and STDOUT for the new process to **outfile**.  In many real shells, it gets much more complicated than this (e.g.,>> to append, > to overwrite, >& redirect STDERR and STDOUT, etc.)!  In this assignment, keep it simple. Only < or >. Again, assume that the user with either > or < individually, but not at the same time.

- Use the following c functions, dup and dup2.
- First open the file (use open, or create, open read-only for infiles and create writable for outfiles) and then use dup2.  As explained in class, 0 is the file descriptor for STDIN, and 1 is the file descriptor for STDOUT.
- Besides the examples, I posted with my slides, literature, and the man command are helpful.

\5) The shell should be able to handle simple **piping operations (single piping)**. 

If the user enters the following command: 

**./myProg1   |  ./myProg2** 

The shell should run the first program and then the second program. However, the second program should not start until the first one is done. The output of the first program will be forwarded as an input of the second program.

**Tip**: create more than one process to handle this situation. Use wait () to make the second child wait for the first.  Use the pipe() to establish communication between them so the first process passes the information to the second via a pipe, if needed.

Remember: We had a hands-on exercise that uses the same concept. 

**How to open a file for reading so that I can use it for changing the standards input?**

**int inFd = open("yourfile.txt", O\_RDONLY);****dup2(inFd, 0);**



**How to open a file for writing so that I can use it for changing the standards output?**

**int outFd = open(pathName[0], O\_WRONLY | O\_CREAT, 0666);**

**dup2(outFd , 1);**



**How to make the parent (shell) process wait for the child process and then get things (STDIN and STDOUT) back to normal?**

**wait(0);//flush output and change the input back to the keyboard and output back to the screenfflush(stdout);dup2(originalInputfd, 0);dup2(originalOutputfd, 1);**

**//close the fd'sclose(originalInputfd);close(originalOutputfd);fflush(stdout);**

**More tips:**

The program should have the following phases:

1. preparing the history data structure (read the file to a designated vector)
2. reading a command from the user and adding it to the history vector.
3. if the command is a simple one (e.g., **exit**) then exit the loop and terminate the program.
4. if the command is **history**, print the history vector content
5. if the command is **pwd**, print the current working directory (maybe calling the designated function). See the function I created below in blue.
6. If the command is something different, I would tokenize it first and see if it has no piping, no redirection. If that is the case, fork a child and make it execute the entered command (execvp()) as explained in class.
7. if the command has redirection of an input or output, you can assume that the last token is the file name and open the file and use dup2() to change the standard in or out as explained in Canvas.
8. if the command has a pipe operator, then you need to create two children. There is an example of canvas that does this command so you can use it.



**III.       What will be graded and what are you expected to submit**

The source files (full application) of the terminal (shell simulator) program with features 1-5 are fully accomplished.  The points (1 to 5) are explained above.  That is, get your terminal program ready with all the features 1-5.

**Note**: Extra work will be awarded extra points if you are planning to add more features to your terminal. For example, allowing the terminal to support the input and output redirection at the same time ( ./prog1 < infile.txt > outfile.txt)  or stacked piping (./pr1 | ./pr2 | ./pr3 | …. etc.  However, it is optional.  Awarded extra points will range between 1 to 5 points.

**Important notes:**

- Please submit your work to the designated dropbox via Canvas. No submission via emails. Unless you submit the work via Canvas, you are submitting the work to the wrong customer.
- If you use anything from the literature, please cite it and add it as a reference to your work (as comments or on a separate paper submitted with the work.)
- I will make myself accessible so never hesitate to contact me if you have any questions.
- Develop and test your program in an incremental fashion. So, start with points 1 to 3. Then add 4 and 5.



**Dr. Saleh Alnaeli**

**UW-Stout**
