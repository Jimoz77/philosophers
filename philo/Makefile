# Nom de l'exécutable
NAME = philo

# Compilateur et options de compilation
CC = gcc
CFLAGS = -O0 -g3 -Wall -Wextra -Werror


# attention au wildcard qui ne sont pas accepté et dangereux
SRCS = main.c ft_atoi.c utils.c philo_life.c philo_eat.c init_philo.c monitor.c

# Générer les noms des fichiers objets correspondants
OBJS = $(SRCS:.c=.o)

# Règle par défaut
all: $(NAME)

# Règle pour créer l'exécutable
$(NAME): $(OBJS)
	$(CC) $(CFLAGS) -o $(NAME) $(OBJS)

# Règle pour compiler les fichiers objets
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Règle pour nettoyer les fichiers objets et l'exécutable
clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

# Règle pour recompiler tout
re: fclean all

.PHONY: all clean fclean re