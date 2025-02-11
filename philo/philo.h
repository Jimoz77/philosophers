#define PHILO_H
#define PHILO_H

#include <pthread.h>

typedef struct s_philo
{
	int id;
	int left_fork;
	int right_fork;
	int eat_count;
	pthread_t thread;
} t_philo;