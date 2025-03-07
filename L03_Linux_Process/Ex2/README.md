Explain code :
    Using exec in a child process
        1. Create a child process using fork().
        2. If argv[1] is '1', the child process executes ls -lah using execlp().
        3. If argv[1] is '2', the child process executes date using execvp().
        4. The parent process first prints "Im your dad punk!", then waits for the child process to finish using wait(NULL).
        5. Once the child process terminates, the parent process prints "Done" and exits.