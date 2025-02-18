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
	int launch_time; // heure de lancement du philo en ms
	int time_to_die; // temps en ms avant que le philo meurt
	int philo_count; // fonctionnel (jsuis lboss)
	int left_fork; // il faut peut utiliser pthread_mutex_t a la place de int
	int right_fork; // une fois les deux fork modifié il faut aussi adapter le code
	int eat_count; 
	pthread_mutex_t eat_mutex; // jsp ski branle la mais c est surement important
	pthread_mutex_t	*forks; // sera surement useless
	pthread_t thread; // c est le bordel mais insh on va y arriver
} t_philo;

