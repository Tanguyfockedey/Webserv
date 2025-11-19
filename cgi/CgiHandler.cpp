#include "CgiHandler.hpp"

CgiHandler::CgiHandler(Request &request)
{
	this->_body = request.get_body();
	init_env(request);
	this->_env_cstr = get_env_cstr();
}

CgiHandler::~CgiHandler()
{
	for (int i = 0; _env_cstr[i]; i++)
		delete _env_cstr[i];
	delete _env_cstr;
}

void	CgiHandler::init_env(Request &request)
{
	std::map<std::string, std::string>	headers = request.get_headers();

	this->_env["QUERY_STRING"] = request.get_uri_query();

	/* std::ostringstream	str1;
	str1 << this->_body.length(); */
	
	/* this->_env["REDIRECT_STATUS"] = "200";
	if (headers.find("Auth-Scheme") != headers.end() && headers["Auth-Scheme"] != "")
		this->_env["AUTH_TYPE"] = headers["Authorization"];
	this->_env["AUTH_TYPE"] = headers["Authorization"];
	this->_env["CONTENT_LENGTH"] = str1.str();
	this->_env["CONTENT_TYPE"] = headers["Content-type"];
	this->_env["GATEWAY_INTERFACE"] = "CGI/1.1";
	this->_env["PATH_INFO"] = request.get_uri_fragment();
	this->_env["REMOTE_ADDR"] = "8081";
	this->_env["REMOTE_HOST"] = "";
	this->_env["REMOTE_IDENT"] = headers["Authorization"];
	this->_env["REMOTE_USER"] = headers["Authorization"];
	this->_env["REQUEST_METHOD"] = request.get_method();
	this->_env["REQUEST_URI"] = request.get_uri();
	this->_env["SCRIPT_NAME"] = request.get_uri_fragment();
	this->_env["SERVER_NAME"] = headers["Hostname"];
	this->_env["SERVER_PORT"] = "8081";
	this->_env["SERVER_PROTOCOL"] = "HTTP/1.1";
	this->_env["SERVER_SOFTWARE"] = "Webserv/0.1"; */
}

char**	CgiHandler::get_env_cstr()
{
	char	**env = new char * [this->_env.size() + 1];
	std::map<std::string, std::string>::iterator iter;

	int i = 0;
	for (iter = _env.begin(); iter != _env.end(); iter++)
	{
		std::string elem = iter->first + "=" + iter->second;
		env[i] = new char[elem.size() + 1];
		env[i] = strcpy(env[i], elem.c_str());
		i++;
	}
	env[i] = NULL;
	return env;
}

std::string	CgiHandler::executeCgi(const std::string scriptName)
{
	int		stdin_save = dup(STDIN_FILENO);
	int		stdout_save = dup(STDOUT_FILENO);
	FILE	*file_in = tmpfile();
	FILE	*file_out = tmpfile();
	int		fd_in = fileno(file_in);
	int		fd_out = fileno(file_out);

	pid_t			pid;
	char* const* 	nll = NULL;
	char**			env = get_env_cstr();

	std::string		body;

	write(fd_in, _body.c_str(), _body.size());
	lseek(fd_in, 0, SEEK_SET);

	pid = fork();
	if (pid == -1) // fork failed
		return ("500 Internal server error\r\n\r\n");
	else if (!pid) // CGI child process
	{
		dup2(fd_in, STDIN_FILENO);
		dup2(fd_out, STDOUT_FILENO);
		execve(scriptName.c_str(), nll, env);
		std::cout << "HERE" + errno <<std::endl;
		write(STDOUT_FILENO, "500 Internal server error\r\n\r\n", 30);
	}
	else // parent process
	{
		char	buffer[BUFSIZE] = {0};

		waitpid(-1, NULL, 0);
		lseek(fd_out, 0, SEEK_SET);

		int bytes_read = 1;
		while (bytes_read > 0)
		{
			memset(buffer, 0, BUFSIZE);
			bytes_read = read(fd_out, buffer, BUFSIZE - 1);
			body += buffer;
		}
	}
	
	// cleanup
	dup2(stdin_save, STDIN_FILENO);
	dup2(stdout_save, STDOUT_FILENO);
	fclose(file_in); fclose(file_out);
	close(fd_in); close(fd_out);
	close(stdin_save); close(stdout_save);

	return body;
}
