/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jimpa <jimpa@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/28 18:19:54 by jimpa             #+#    #+#             */
/*   Updated: 2025/07/28 18:23:26 by jimpa            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

long	get_current_time_in_ms(void)
{
	struct timeval	current_time;

	gettimeofday(&current_time, NULL);
	return (current_time.tv_sec * 1000 + current_time.tv_usec / 1000);
}

int	simulation_stopped(t_philo *philo)
{
	int	result;

	pthread_mutex_lock(philo->stop_mutex);
	result = *(philo->simulation_stop);
	pthread_mutex_unlock(philo->stop_mutex);
	return (result);
}

void	safe_printf(t_philo *philo, char *message)
{
	pthread_mutex_lock(philo->print_mutex);
	if (!simulation_stopped(philo))
	{
		philo->current_time = get_current_time_in_ms();
		printf("%ld %d %s\n",
			philo->current_time - philo->launch_time, philo->id, message);
	}
	pthread_mutex_unlock(philo->print_mutex);
}

void	mutex_des(t_philo *philo, int nb)
{
	int	i;

	i = 0;
	while (i < nb)
	{
		pthread_mutex_destroy(&philo[i].eat_mutex);
		pthread_mutex_destroy(&philo->forks[i]);
		i++;
	}
	pthread_mutex_destroy(philo[0].stop_mutex);
	pthread_mutex_destroy(philo[0].print_mutex);
	free(philo[0].stop_mutex);
	free(philo[0].print_mutex);
	free(philo[0].simulation_stop);
	free(philo->forks);
}

void	stop_simulation(t_philo *philo)
{
	pthread_mutex_lock(philo->stop_mutex);
	*(philo->simulation_stop) = 1;
	pthread_mutex_unlock(philo->stop_mutex);
}
