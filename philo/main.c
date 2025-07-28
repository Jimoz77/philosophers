/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main_copy.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jimpa <jimpa@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/12 17:19:38 by jimpa             #+#    #+#             */
/*   Updated: 2025/07/28 20:23:08 by jimpa            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

void	*global_monitor(void *arg)
{
	t_philo			*philos;
	t_monitor_vars	v;

	philos = (t_philo *)arg;
	v.philo_count = philos[0].philo_count;
	v.all_ate_enough = 0;
	while (!simulation_stopped(&philos[0]))
	{
		if (monitor_death_loop(philos, &v))
			return (NULL);
		if (check_all_ate_enough(philos, &v))
			return (NULL);
		usleep(1000);
	}
	return (NULL);
}

t_philo	*init_philo(t_main_vars v)
{
	t_philo		*philo;
	t_init_vars	v2;

	v2.global_launch_time = get_current_time_in_ms();
	philo = malloc(sizeof(t_philo) * v.nb);
	if (!philo)
		return (NULL);
	v2.forks = init_forks(v.nb);
	if (!v2.forks)
	{
		free(philo);
		return (NULL);
	}
	if (!init_shared_resources(&v2))
	{
		free(philo);
		free(v2.forks);
		return (NULL);
	}
	init_philosophers_data(philo, v, &v2);
	create_threads(philo, v, &v2);
	join_threads(philo, v, &v2);
	return (philo);
}

int	main(int ac, char **av)
{
	t_philo			*philo;
	t_main_vars		v;

	if (ac == 5 || ac == 6)
	{
		v.must_eat = 0;
		v.nb = ft_atoi(av[1]);
		v.die_time = ft_atoi(av[2]);
		v.time_to_eat = ft_atoi(av[3]);
		v.time_to_sleep = ft_atoi(av[4]);
		if (ac == 6)
		{
			v.must_eat = ft_atoi(av[5]);
		}
		philo = init_philo(v);
		if (!philo)
			return (1);
		mutex_des(philo, v.nb);
		free(philo);
	}
	return (0);
}
