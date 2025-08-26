# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: mcygan <mcygan@student.s19.be>             +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/04/20 16:37:10 by tafocked          #+#    #+#              #
#    Updated: 2025/08/25 14:13:31 by mcygan           ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME	= webserv

FILES	= webserv.cpp \
		config/Config.cpp \
		config/Location.cpp \
		request/Request.cpp \
		request/Response.cpp \
		server/Client.cpp \
		server/Cluster.cpp \
		server/Server.cpp \
		cgi/CgiHandler.cpp \
		utils/utils.cpp \
			
CC		= g++
CFLAGS	= -Wall -Wextra -Werror -std=c++98 -pedantic -g3
LDLIBS	=

FILES_DIR = ./
OBJ_DIR	= .obj
OBJ		= $(addprefix $(OBJ_DIR)/, $(FILES:%.cpp=%.o))

all: $(NAME)

re: fclean all

$(OBJ_DIR)/%.o: $(FILES_DIR)/%.cpp
	mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

$(NAME): $(OBJ)
	$(CC) $(CFLAGS) $(LDLIBS) $(OBJ) -o $(NAME)

clean:
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -f $(NAME)

.PHONY: all re clean fclean