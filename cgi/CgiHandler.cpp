#include "CgiHandler.hpp"

CgiHandler::CgiHandler(Request &request)
{
	this->_body = request.get_body();
	this->init_env(request);
}

CgiHandler::CgiHandler(CgiHandler &src)
{
	this->_body = src._body;
	this->_env = src._env;
}

CgiHandler::~CgiHandler(void) {}

CgiHandler	&CgiHandler::operator=(CgiHandler &src)
{
	if (this == &src)
		return (*this);
	this->_body = src._body;
	this->_env = src._env;
	return (*this);
}

void	CgiHandler::init_env(Request &request)
{
	std::map<std::string, std::string>	headers = request.get_headers();

	std::ostringstream	str1;
	str1 << this->_body.length();

	this->_env["REDIRECT_STATUS"] = "200";
	/* if (headers.find("Auth-Scheme") != headers.end() && headers["Auth-Scheme"] != "")
		this->_env["AUTH_TYPE"] = headers["Authorization"]; */
	/* this->_env["AUTH_TYPE"] = headers["Authorization"]; */
	this->_env["CONTENT_LENGTH"] = str1.str();
	this->_env["CONTENT_TYPE"] = headers["Content-type"];
	this->_env["GATEWAY_INTERFACE"] = "CGI/1.1";
	this->_env["PATH_INFO"] = request.get_uri_fragment();
	this->_env["QUERY_STRING"] = request.get_uri_query();
	/* this->_env["REMOTE_ADDR"] = "8081";
	this->_env["REMOTE_HOST"] = ""; */
	this->_env["REMOTE_IDENT"] = headers["Authorization"];
	this->_env["REMOTE_USER"] = headers["Authorization"];
	this->_env["REQUEST_METHOD"] = request.get_method();
	this->_env["REQUEST_URI"] = request.get_uri();
	this->_env["SCRIPT_NAME"] = request.get_uri_fragment();
	this->_env["SERVER_NAME"] = headers["Hostname"];
	/* this->_env["SERVER_PORT"] = "8081"; */
	this->_env["SERVER_PROTOCOL"] = "HTTP/1.1";
	this->_env["SERVER_SOFTWARE"] = "Webserv/0.1";
}

std::string	CgiHandler::executeCgi(const std::string scriptName)
{
	int			stdin_save = dup(STDIN_FILENO);
	int			stdout_save = dup(STDOUT_FILENO);
	FILE		*file_in = tmpfile();
	FILE		*file_out = tmpfile();
	int			fd_in = fileno(file_in);
	int			fd_out = fileno(file_out);

	char* const* nll = NULL;

	pid_t		pid;
	std::string	ret;

	write(fd_in, _body.c_str(), _body.size());
	if (!(pid = fork()))
	{
		dup2(fd_in, STDIN_FILENO);
		dup2(fd_out, STDOUT_FILENO);
		execve(scriptName.c_str(), nll, nll);
	}
	else
	{
		std::cerr << RED << "Fork error" << RESET << std::endl;
		return ("Status: 500\r\n\r\n");
	}
	dup2(stdin_save, STDIN_FILENO);
	dup2(stdout_save, STDOUT_FILENO);
	fclose(file_in);
	fclose(file_out);
	close(fd_in);
	close(fd_out);
	close(stdin_save);
	close(stdout_save);

	return ret;
}

/* std::string		CgiHandler::executeCgi(std::string& scriptName) {
	pid_t		pid;
	int			saveStdin;
	int			saveStdout;
	char		**env;
	std::string	newBody;

	// SAVING STDIN AND STDOUT IN ORDER TO TURN THEM BACK TO NORMAL LATER
	saveStdin = dup(STDIN_FILENO);
	saveStdout = dup(STDOUT_FILENO);

	FILE	*fIn = tmpfile();
	FILE	*fOut = tmpfile();
	long	fdIn = fileno(fIn);
	long	fdOut = fileno(fOut);
	int		ret = 1;

	write(fdIn, _body.c_str(), _body.size());
	lseek(fdIn, 0, SEEK_SET);

	pid = fork();

	if (pid == -1)
	{
		std::cerr << RED << "Fork crashed." << RESET << std::endl;
		return ("Status: 500\r\n\r\n");
	}
	else if (!pid)
	{
		char * const * nll = NULL;

		dup2(fdIn, STDIN_FILENO);
		dup2(fdOut, STDOUT_FILENO);
		execve(scriptName.c_str(), nll, env);
		std::cerr << RED << "Execve crashed." << RESET << std::endl;
		write(STDOUT_FILENO, "Status: 500\r\n\r\n", 15);
	}
	else
	{
		char	buffer[1024] = {0};

		waitpid(-1, NULL, 0);
		lseek(fdOut, 0, SEEK_SET);

		ret = 1;
		while (ret > 0)
		{
			memset(buffer, 0, 1024);
			ret = read(fdOut, buffer, 1024 - 1);
			newBody += buffer;
		}
	}

	dup2(saveStdin, STDIN_FILENO);
	dup2(saveStdout, STDOUT_FILENO);
	fclose(fIn);
	fclose(fOut);
	close(fdIn);
	close(fdOut);
	close(saveStdin);
	close(saveStdout);

	for (size_t i = 0; env[i]; i++)
		delete[] env[i];
	delete[] env;

	if (!pid)
		exit(0);

	return (newBody);
} */
