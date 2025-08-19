#pragma once

#include "../webserv.hpp"
#include "../request/Request.hpp"
#include "../config/Config.hpp"
#include <sys/wait.h>

class CgiHandler {
	private:
		std::string							_body;
		std::map<std::string, std::string>	_env;
	public:
		// constructors & destructors
		CgiHandler(Request &request, Config &config);
		CgiHandler(CgiHandler &src);
		virtual ~CgiHandler(void);
		// operators
		CgiHandler		&operator=(CgiHandler &src);
		// methods
		void			init_env(Request &request, Config &config);
		std::string		executeCgi(std::string &scriptName);
};
