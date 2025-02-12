#define PHILO_H
#define PHILO_H

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int	ft_atoi(const char *str);

typedef struct s_philo
{
	int id;
	int philo_count;
	int left_fork;
	int right_fork;
	pthread_mutex_t	*forks;
	int eat_count;
	pthread_mutex_t *eat_count_mutex;
	pthread_t thread;
} t_philo;