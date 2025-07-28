/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo_eat.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jimpa <jimpa@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/28 19:40:15 by jimpa             #+#    #+#             */
/*   Updated: 2025/07/28 19:45:45 by jimpa            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

void	handle_single_philo(t_philo *philo)
{
	printf("%ld %d died\n", philo->time_to_die, philo->id);
	stop_simulation(philo);
}

void	determine_fork_order(t_philo *philo, int *first, int *second)
{
	if (philo->left_fork < philo->right_fork)
	{
		*first = philo->left_fork;
		*second = philo->right_fork;
	}
	else
	{
		*first = philo->right_fork;
		*second = philo->left_fork;
	}
}

int	acquire_forks(t_philo *philo, int first, int second)
{
	pthread_mutex_lock(&philo->forks[first]);
	pthread_mutex_lock(&philo->forks[second]);
	if (simulation_stopped(philo))
	{
		pthread_mutex_unlock(&philo->forks[first]);
		pthread_mutex_unlock(&philo->forks[second]);
		return (0);
	}
	return (1);
}

void	update_eating_data(t_philo *philo)
{
	pthread_mutex_lock(&philo->eat_mutex);
	philo->last_meal = philo->current_time - philo->launch_time;
	philo->eat_count++;
	pthread_mutex_unlock(&philo->eat_mutex);
}

void	perform_eating(t_philo *philo, int first, int second)
{
	philo->current_time = get_current_time_in_ms();
	safe_printf(philo, "has taken a fork");
	safe_printf(philo, "has taken a fork");
	safe_printf(philo, "is eating");
	update_eating_data(philo);
	usleep(philo->time_to_eat * 1000);
	pthread_mutex_unlock(&philo->forks[first]);
	pthread_mutex_unlock(&philo->forks[second]);
}
