CFLAGS = -Wall -Werror
usage:
	@echo ""
	@echo "Usage :"
	@echo "    On the server computer, start the server:"
	@echo "       ./server port"
	@echo "    On the client computer, start the client:"
	@echo "       ./client ipserver portserver"
	@echo ""
dep : server client usage
all : clean dep
clean :; -rm server client