EXPLAIN:
    Using signal to prevent zombie process
        1. The parent process starts and creates a child process.
        2. The child process runs, prints "Im here dad", then waits for 1 second before exiting.
        3. The parent process is waiting in pause().
        When the child process terminates, it sends SIGCHLD to the parent.
        4. The waitHandler() function is called, and the child process is cleaned up using wait(NULL), preventing a zombie process.
        5. The parent process continues waiting for new signals.