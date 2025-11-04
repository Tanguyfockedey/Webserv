#pragma once

#include "../webserv.hpp"
#include "../request/Request.hpp"
#include "../config/Config.hpp"
#include <sys/wait.h>
#include <string>

#define BUFSIZE	256

class CgiHandler {
	private:
		std::string							_body;
		std::map<std::string, std::string>	_env;
		char**								_env_cstr;
		void	init_env(Request &request);
		char**	get_env_cstr();
	public:
		CgiHandler(Request &request);
		~CgiHandler();
		std::string	executeCgi(const std::string scriptName);
};
