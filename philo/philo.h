#define PHILO_H
#define PHILO_H

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>

int	ft_atoi(const char *str);

typedef struct s_philo
{
	int id;
	long current_time; // temps actuel depuis le dernier repas
	long launch_time; // temps actuel depuis le debut du programme
	long time_to_die; // temps en ms avant que le philo meurt
	long time_to_eat; // temps en ms pour manger
	long time_to_sleep; // temps en ms pour dormir
	int philo_count; // fonctionnel (jsuis lboss)
	int left_fork; // il faut peut utiliser pthread_mutex_t a la place de int
	int right_fork; // une fois les deux fork modifié il faut aussi adapter le code
	int eat_count; 
	pthread_mutex_t eat_mutex; // jsp ski branle la mais c est surement important
	pthread_mutex_t	*forks; // sera surement useless
	pthread_t thread; // c est le bordel mais insh on va y arriver
} t_philo;

