/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jimpa <jimpa@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/12 17:19:38 by jimpa             #+#    #+#             */
/*   Updated: 2025/02/18 20:15:52 by jimpa            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

long get_current_time_in_ms()
{
	struct timeval current_time;
	gettimeofday(&current_time, NULL);
	return ((current_time.tv_sec * 1000) + (current_time.tv_usec / 1000));
}
int time_to_die(long launch_time, long time_to_die)
{
	long current_time;
	//long elapsed_time;

	current_time = get_current_time_in_ms();
	//elapsed_time = current_time - launch_time;
	//if (elapsed_time > time_to_die)
	//	return (1);
	//return (0);
	printf("current_time = %ld | launch_time = %ld | diff = %ld |time_to_die = %ld\n", current_time, launch_time,(current_time - launch_time),time_to_die);
	return ((current_time - launch_time) > time_to_die);
}

void	mutex_des(t_philo *philo, int nb) // full random
{
	int	i;

	i = 0;
	while (i < nb)
	{
		pthread_mutex_destroy(philo[i].forks);
		pthread_mutex_destroy(&philo[i].eat_mutex);
		i++;
	}
	//free(philo->forks);
	free(philo);
}

int set_hungriest(t_philo *philos) // fonc qui peut servir mais pue actuellement la merde
{
    int i = 0;
    int hungriest = 0;
    int small_count;

    // Initialiser small_count avec le eat_count du premier philosophe
    pthread_mutex_lock(&philos[0].eat_mutex);
    small_count = philos[0].eat_count;
    pthread_mutex_unlock(&philos[0].eat_mutex);

    // Parcourir tous les philosophes pour trouver le plus affamé
    for (i = 1; i < philos[i].philo_count; i++)
    {
        pthread_mutex_lock(&philos[i].eat_mutex);
        if (philos[i].eat_count < small_count)
        {
            small_count = philos[i].eat_count;
            hungriest = i;
        }
        pthread_mutex_unlock(&philos[i].eat_mutex);
    }

    //printf("hungriest = %d | id = %d\n", hungriest, philos[hungriest].id);
    return philos[hungriest].id;
}
void   philo_think(t_philo *philo) // la meilleur fonc du projet
{
	printf("%d is thinking\n", philo->id);
	usleep(1000000);
}
void  philo_sleep(t_philo *philo) // béton
{
	printf("%d is sleeping\n", philo->id);
	usleep(1000000);
}


void	philo_eat(t_philo *philo)
{
	if(philo->id % 2 == 0)
	{
		pthread_mutex_lock(&philo->forks[philo->left_fork]);
		pthread_mutex_lock(&philo->forks[philo->right_fork]);
	}
	else
	{
		pthread_mutex_lock(&philo->forks[philo->right_fork]);
		pthread_mutex_lock(&philo->forks[philo->left_fork]);
	}
	//pthread_mutex_lock(&philo->eat_mutex);
	philo->eat_count++;
	printf("%d is eating       (%d time)\n", philo->id, philo->eat_count);
	usleep(1000000);
	philo->launch_time = get_current_time_in_ms();
	//pthread_mutex_unlock(&philo->eat_mutex);
	pthread_mutex_unlock(&philo->forks[philo->left_fork]);
	pthread_mutex_unlock(&philo->forks[philo->right_fork]);
}


void	*philo_life(void *arg) // gro delbor jcomprend r // surement plus simple avec des tableaux
{
	t_philo	*philo;

	philo = (t_philo *)arg;
	while (!time_to_die(philo->launch_time, philo->time_to_die))
	{
		if (philo->id == set_hungriest(philo)) // c est la que ca se passe (mdr rien du tout)
		{
		 	philo_eat(philo);
		} 
		//philo_eat(philo);
		philo_sleep(philo);
		//usleep(1000000);
		philo_think(philo);
		//usleep(1000000);
	}
	printf("%d is dead\n", philo->id);
}

t_philo	*init_philo(int nb, long time_to_die) // pue la merde
{
	t_philo	*philo;
	int		i;
	long	launch_time;
	pthread_mutex_t *forks;

	i = 0;
	launch_time = get_current_time_in_ms();
	philo = malloc(sizeof(t_philo) * nb);
	if (!philo)
		return (NULL);
	forks = malloc(sizeof(pthread_mutex_t) * nb);
	if (!forks)
	{
		free(philo);
		return (NULL);
	}
	while (i < nb)
	{
    	pthread_mutex_init(&forks[i], NULL);
		i++;
	}
	i = 0;	
	while (i < nb)
	{
		philo[i].id = i;
		philo[i].left_fork = i;
		philo[i].right_fork = (i + 1) % nb;
		philo[i].philo_count = nb;
		philo[i].eat_count = 0;
		philo[i].launch_time = launch_time;
		philo[i].time_to_die = time_to_die;
		philo[i].forks = forks;
		pthread_mutex_init(&philo[i].eat_mutex, NULL);
		pthread_create(&philo[i].thread, NULL, philo_life, &philo[i]);
		i++;
	}
	philo[i].eat_count = 0; // ATTENTION J ai du rajouter ca ici psq sinon derniere valeur de eat_count etait random surment d autre probleme avec les autres valeurs
	i = 0;
	while (i < nb)
	{
		pthread_join(philo[i].thread, NULL); // surement pas comme ca qu on l utilise
		i++;
	}
	return (philo);
}
// chaque philo doit avoir un thread

int	main(int ac, char **av)// main qui pue la classe
{
	t_philo	*philo;
	int nb;
	long die_time;

	if (ac == 3) // a changer
	{
		nb = ft_atoi(av[1]);
		die_time = ft_atoi(av[2]); // atoi doit peut etre gerer les long pour les ms
		philo = init_philo(nb, die_time);
		//thread_join(philo, ft_atoi(av[1]));
		if (!philo)
			return (1);
		mutex_des(philo, nb); // mdr comme si ca leakait pas 
		//free(philo); // spiderman holding the trains by the strings
	}
} // minimalisme a la steve jobs
