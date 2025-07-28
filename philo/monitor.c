/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jimpa <jimpa@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/28 20:19:58 by jimpa             #+#    #+#             */
/*   Updated: 2025/07/28 20:20:28 by jimpa            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

int	check_philosopher_death(t_philo *philos, t_monitor_vars *v)
{
	pthread_mutex_lock(&philos[v->i].eat_mutex);
	v->current_time = get_current_time_in_ms();
	if (time_to_die(philos[v->i].last_meal, philos[v->i].time_to_die,
			v->current_time, philos[v->i].launch_time))
	{
		printf("%ld %d died\n",
			v->current_time - philos[v->i].launch_time, philos[v->i].id);
		pthread_mutex_unlock(&philos[v->i].eat_mutex);
		stop_simulation(&philos[0]);
		return (1);
	}
	pthread_mutex_unlock(&philos[v->i].eat_mutex);
	return (0);
}

int	monitor_death_loop(t_philo *philos, t_monitor_vars *v)
{
	v->i = 0;
	while (v->i < v->philo_count)
	{
		if (check_philosopher_death(philos, v))
			return (1);
		v->i++;
	}
	return (0);
}

int	check_individual_eat_count(t_philo *philos, t_monitor_vars *v)
{
	pthread_mutex_lock(&philos[v->i].eat_mutex);
	if (philos[v->i].eat_count < philos[v->i].must_eat)
	{
		v->all_ate_enough = 0;
		pthread_mutex_unlock(&philos[v->i].eat_mutex);
		return (0);
	}
	pthread_mutex_unlock(&philos[v->i].eat_mutex);
	return (1);
}

int	check_all_ate_enough(t_philo *philos, t_monitor_vars *v)
{
	if (philos[0].must_eat <= 0)
		return (0);
	v->all_ate_enough = 1;
	v->i = 0;
	while (v->i < v->philo_count)
	{
		if (!check_individual_eat_count(philos, v))
			break ;
		v->i++;
	}
	if (v->all_ate_enough)
	{
		stop_simulation(&philos[0]);
		return (1);
	}
	return (0);
}
