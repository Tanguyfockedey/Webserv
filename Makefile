# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: tafocked <tafocked@student.s19.be>         +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/04/20 16:37:10 by tafocked          #+#    #+#              #
#    Updated: 2025/06/09 18:58:40 by tafocked         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME	= webserv

FILES	= webserv.cpp \
		server/Server.cpp \
		server/Cluster.cpp \
		config/Config.cpp \
			
CC		= c++
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