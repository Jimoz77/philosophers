/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo_life.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jimpa <jimpa@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/28 18:24:42 by jimpa             #+#    #+#             */
/*   Updated: 2025/07/28 19:41:00 by jimpa            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

int	time_to_die(long last_meal, long time_to_die,
	long current_time, long launch_time)
{
	long	diff;

	diff = (current_time - launch_time) - last_meal;
	return (diff >= time_to_die);
}

void	philo_think(t_philo *philo)
{
	if (!simulation_stopped(philo))
	{
		usleep(200);
		safe_printf(philo, "is thinking");
	}
	return ;
}

void	philo_sleep(t_philo *philo)
{
	if (!simulation_stopped(philo))
	{
		safe_printf(philo, "is sleeping");
		usleep(philo->time_to_sleep * 1000);
	}
	return ;
}

void	philo_eat(t_philo *philo)
{
	int	first_fork;
	int	second_fork;

	if (philo->philo_count == 1)
	{
		handle_single_philo(philo);
		return ;
	}
	determine_fork_order(philo, &first_fork, &second_fork);
	if (!simulation_stopped(philo))
	{
		if (acquire_forks(philo, first_fork, second_fork))
			perform_eating(philo, first_fork, second_fork);
	}
}

void	*philo_life(void *arg)
{
	t_philo	*philo;

	philo = (t_philo *)arg;
	pthread_mutex_lock(&philo->eat_mutex);
	philo->last_meal = 0;
	pthread_mutex_unlock(&philo->eat_mutex);
	while (!simulation_stopped(philo)
		&& (philo->eat_count < philo->must_eat || philo->must_eat == 0))
	{
		if (simulation_stopped(philo))
			break ;
		philo_eat(philo);
		if (simulation_stopped(philo))
			break ;
		philo_sleep(philo);
		if (simulation_stopped(philo))
			break ;
		philo_think(philo);
	}
	return (NULL);
}
