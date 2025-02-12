/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jimpa <jimpa@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/12 17:19:38 by jimpa             #+#    #+#             */
/*   Updated: 2025/02/12 20:11:38 by jimpa            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

void mutex_des(t_philo *philo, int nb)
{
	int	i;

	i = 0;
	while (i < nb)
	{
		pthread_mutex_destroy(philo[i].forks);
		free(philo[i].forks);
		i++;
	}
	free(philo);
}

/* void	thread_join(t_philo *philo, int nb)
{
	int	i;

} */


void	philo_eat(t_philo *philo)
{
	printf("%d is eating\n", philo->id);
	philo->eat_count++;
}


void	*philo_life(void *arg)
{
	t_philo	*philo = (t_philo *)arg;

	philo = (t_philo *)arg;
	while (1)
	{
		pthread_mutex_lock(&philo->forks[philo->left_fork]);
		pthread_mutex_lock(&philo->forks[philo->right_fork]);
		philo_eat(philo);
		pthread_mutex_unlock(&philo->forks[philo->left_fork]);
		pthread_mutex_unlock(&philo->forks[philo->right_fork]);
		/* philo_sleep(philo);
		philo_think(philo); */
	}
}

t_philo	*init_philo(int nb)
{
	t_philo	*philo;
	int		i;

	i = 0;
	philo = malloc(sizeof(t_philo) * nb);
	if (!philo)
		return (NULL);
	while (i < nb)
	{
		philo[i].id = i;
		philo[i].left_fork = i;
		philo[i].right_fork = (i + 1) % nb;
		philo[i].forks = malloc(sizeof(pthread_mutex_t));
		if (!philo[i].forks)
		{
			while(i >= 0)
				free(philo[--i].forks);
			free(philo);
			return (NULL);
		}
		pthread_mutex_init(philo[i].forks, NULL);
		philo[i].eat_count = 0;
		pthread_create(&philo[i].thread, NULL, philo_life, &philo[i]);
		i++;
	}
	i = 0;
	while (i < nb)
	{
		pthread_join(philo[i].thread, NULL);
		i++;
	}
	return (philo);
}
// chaque philo doit avoir un thread

int	main(int ac, char **av)
{
	t_philo	*philo;

	if (ac == 2) // a changer
	{
		philo = init_philo(ft_atoi(av[1]));
		//thread_join(philo, ft_atoi(av[1]));
		if (!philo)
			return (1);
		mutex_des(philo, ft_atoi(av[1]));
		free(philo);
	}
}
