#include <iostream>
#include <unistd.h>
#include <cstring>
#include <sys/wait.h>

int main() {
    // Path to the script
    const char *scriptPath = "testCGI.py";

    // Arguments for the script (argv array must be null-terminated)
    char *const argv[] = {const_cast<char *>("/usr/bin/env"), const_cast<char *>("python3"), const_cast<char *>(scriptPath), NULL};

    // Environment variables (envp array must be null-terminated)
    char *const envp[] = {NULL};

    // Create pipes for stdin and stdout
    int pipe_stdin[2];
    int pipe_stdout[2];

    if (pipe(pipe_stdin) == -1 || pipe(pipe_stdout) == -1) {
        std::cerr << "Error creating pipes: " << strerror(errno) << std::endl;
        return 1;
    }

    // Fork the process
    pid_t pid = fork();
    if (pid == -1) {
        std::cerr << "Error forking process: " << strerror(errno) << std::endl;
        return 1;
    }

    if (pid == 0) {
        // Child process
        // Redirect stdin
        if (dup2(pipe_stdin[0], STDIN_FILENO) == -1) {
            std::cerr << "Error redirecting stdin: " << strerror(errno) << std::endl;
            return 1;
        }
        close(pipe_stdin[0]);
        close(pipe_stdin[1]);

        // Redirect stdout
        if (dup2(pipe_stdout[1], STDOUT_FILENO) == -1) {
            std::cerr << "Error redirecting stdout: " << strerror(errno) << std::endl;
            return 1;
        }
        close(pipe_stdout[0]);
        close(pipe_stdout[1]);

        // Execute the script
        if (execve("/usr/bin/env", argv, envp) == -1) {
            std::cerr << "Error executing script: " << strerror(errno) << std::endl;
            return 1;
        }
    } else {
        // Parent process
        close(pipe_stdin[0]);
        close(pipe_stdout[1]);

        // Write to the child's stdin if needed
        // const char *input = "name=John&color=#ff0000";
        // write(pipe_stdin[1], input, strlen(input));
        close(pipe_stdin[1]);

        // Read from the child's stdout
        char buffer[1024];
        ssize_t bytesRead;
        while ((bytesRead = read(pipe_stdout[0], buffer, sizeof(buffer) - 1)) > 0) {
            buffer[bytesRead] = '\0';
            std::cout << buffer;
        }
        close(pipe_stdout[0]);

        // Wait for the child process to finish
        int status;
        waitpid(pid, &status, 0);
    }

    return 0;
}