/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_philo.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jimpa <jimpa@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/28 19:59:44 by jimpa             #+#    #+#             */
/*   Updated: 2025/07/28 20:30:37 by jimpa            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

pthread_mutex_t	*init_forks(int nb)
{
	pthread_mutex_t	*forks;
	int				i;

	forks = malloc(sizeof(pthread_mutex_t) * nb);
	if (!forks)
		return (NULL);
	i = 0;
	while (i < nb)
	{
		pthread_mutex_init(&forks[i], NULL);
		i++;
	}
	return (forks);
}

int	init_shared_resources(t_init_vars *v2)
{
	v2->shared_stop_mutex = malloc(sizeof(pthread_mutex_t));
	v2->shared_print_mutex = malloc(sizeof(pthread_mutex_t));
	v2->shared_simulation_stop = malloc(sizeof(int));
	if (!v2->shared_stop_mutex || !v2->shared_simulation_stop
		|| !v2->shared_print_mutex)
		return (0);
	pthread_mutex_init(v2->shared_stop_mutex, NULL);
	pthread_mutex_init(v2->shared_print_mutex, NULL);
	*v2->shared_simulation_stop = 0;
	return (1);
}

void	configure_philosopher(t_philo *philo, t_main_vars v,
								t_init_vars *v2, int index)
{
	philo[index].id = index + 1;
	philo[index].left_fork = index;
	philo[index].right_fork = (index + 1) % v.nb;
	philo[index].philo_count = v.nb;
	philo[index].eat_count = 0;
	philo[index].time_to_eat = v.time_to_eat;
	philo[index].time_to_sleep = v.time_to_sleep;
	philo[index].time_to_die = v.die_time;
	philo[index].forks = v2->forks;
	philo[index].launch_time = v2->global_launch_time;
	philo[index].last_meal = 0;
	philo[index].must_eat = v.must_eat;
	philo[index].stop_mutex = v2->shared_stop_mutex;
	philo[index].print_mutex = v2->shared_print_mutex;
	philo[index].simulation_stop = v2->shared_simulation_stop;
	pthread_mutex_init(&philo[index].eat_mutex, NULL);
}

void	init_philosophers_data(t_philo *philo, t_main_vars v,
									t_init_vars *v2)
{
	int	i;

	i = 0;
	while (i < v.nb)
	{
		configure_philosopher(philo, v, v2, i);
		i++;
	}
}

void	create_threads(t_philo *philo, t_main_vars v, t_init_vars *v2)
{
	int	i;

	pthread_create(&v2->monitor_thread, NULL, global_monitor, philo);
	i = 0;
	while (i < v.nb)
	{
		pthread_create(&philo[i].thread, NULL, philo_life, &philo[i]);
		i++;
	}
}
