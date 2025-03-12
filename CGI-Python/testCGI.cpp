#include <iostream>
#include <unistd.h>
#include <cstring>
#include <sys/wait.h>
#include <string>

// void processPythonCGI(const HttpRequest request, HttpResponse &response)
// {

// }

void processPythonCGI_Dummy()
{
	// Path to the script
	const char *scriptPath = "processPlayer.py";
	httpRequest->getPath()

	// Arguments for the script (argv array must be null-terminated)
	// char *const argv[] = {const_cast<char *>("/usr/bin/env"), const_cast<char *>("python3"), const_cast<char *>(scriptPath), NULL};
	char *const argv[] = {const_cast<char *>("/usr/bin/python3"), const_cast<char *>(scriptPath), NULL};
	std::string method = "REQUEST_METHOD=POST";
	std::string query = "QUERY_STRING=playerName=John";
	std::string contentType = "CONTENT_TYPE=application/x-www-form-urlencoded";
	std::string contentLength = "CONTENT_LENGTH=16"; httpRequest->contentLength
	std::string uploadDir = "UPLOAD_DIR=/tmp";
	std::string fileSize = "HTTP_FILESIZE=1024";
	std::string status = "REDIRECT_STATUS=200";

	char * envp[] = {
		const_cast<char *>(method.c_str()),
		const_cast<char *>(query.c_str()),
		const_cast<char *>(contentType.c_str()),
		const_cast<char *>(contentLength.c_str()),
		const_cast<char *>(uploadDir.c_str()),
		const_cast<char *>(fileSize.c_str()),
		const_cast<char *>(status.c_str()),
		NULL
	};

	// Create pipes for stdin and stdout
	int pipe_stdin[2];
	int pipe_stdout[2];

	if (pipe(pipe_stdin) == -1 || pipe(pipe_stdout) == -1)
	{
		std::cerr << "Error creating pipes: " << strerror(errno) << std::endl;
		return ;
	}

	// Fork the process
	pid_t pid = fork();
	if (pid == -1)
	{
		std::cerr << "Error forking process: " << strerror(errno) << std::endl;
		return ;
	}

	if (pid == 0)
	{
		// Child process
		// Redirect stdin
		if (dup2(pipe_stdin[0], STDIN_FILENO) == -1)
		{
			std::cerr << "Error redirecting stdin: " << strerror(errno) << std::endl;
			return ;
		}
		close(pipe_stdin[0]);
		close(pipe_stdin[1]);

		// Redirect stdout
		if (dup2(pipe_stdout[1], STDOUT_FILENO) == -1)
		{
			std::cerr << "Error redirecting stdout: " << strerror(errno) << std::endl;
			return ;
		}
		close(pipe_stdout[0]);
		close(pipe_stdout[1]);

		// Execute the script
		if (execve(argv[0], argv, envp) == -1)
		{
			std::cerr << "Error executing script: " << strerror(errno) << std::endl;
			return ;
		}
	}
	else
	{
		// Parent process
		close(pipe_stdin[0]);
		close(pipe_stdout[1]);

		// Write to the child's stdin if needed
		// const char *input = "name=John&color=#ff0000";
		// write(pipe_stdin[1], input, strlen(input));
		close(pipe_stdin[1]);

		// Read from the child's stdout
		char buffer[1024];
		size_t bytesRead;
		while ((bytesRead = read(pipe_stdout[0], buffer, sizeof(buffer) - 1)) > 0)
		{
			buffer[bytesRead] = '\0';
			std::cout << buffer;
		}
		close(pipe_stdout[0]);

		// Wait for the child process to finish
		int status;
		waitpid(pid, &status, 0);
	}
}

int main()
{
	processPythonCGI_Dummy();
	return 0;
}
