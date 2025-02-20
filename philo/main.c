/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jimpa <jimpa@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/12 17:19:38 by jimpa             #+#    #+#             */
/*   Updated: 2025/02/20 20:28:22 by jimpa            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

long get_current_time_in_ms()
{
	struct timeval current_time;
	gettimeofday(&current_time, NULL);
	return (current_time.tv_sec * 1000 + current_time.tv_usec / 1000);
}
int time_to_die(long launch_time, long time_to_die)
{
	long current_time;

	current_time = get_current_time_in_ms();
	//printf("| diff = %ld |\n",(current_time - launch_time));
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

int set_hungriest(t_philo *philos) // fonctionne mais engendre data race 
{ // va savoir pourquoi mais l'algo trie deja les plus affamés sans avoir a utiliser cette fonction
    int i = 0;
    int hungriest = 0;
    int small_count;
	int philo_count;

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

    //printf("Hungriest philo ID = %d\n", philos[hungriest].id);
    return philos[hungriest].id;
}
void   philo_think(t_philo *philo) // la meilleur fonc du projet
{
	philo->current_time = get_current_time_in_ms();
	printf("%ldms %d is thinking\n",philo->current_time - philo->launch_time, philo->id);
}
void  philo_sleep(t_philo *philo) // béton
{
	philo->current_time = get_current_time_in_ms();
	printf("%ldms %d is sleeping\n",philo->current_time - philo->launch_time, philo->id); // verifier si mettre apres le usleep
	usleep(philo->time_to_sleep * 1000);
}


void	philo_eat(t_philo *philo) // attention probleme de décalage d'une ms
{
	if(philo->id % 2 == 0) // fork de gauche en premier si id pair
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
	philo->current_time = get_current_time_in_ms();
	printf("%ldms %d has taken a fork\n", philo->current_time - philo->launch_time, philo->id);
	printf("%ldms %d has taken a fork\n", philo->current_time - philo->launch_time, philo->id);
	printf("%ldms %d is eating       (%d time)\n", philo->current_time - philo->launch_time, philo->id, philo->eat_count);
	philo->last_meal = philo->current_time;
	philo->eat_count++;
	usleep(philo->time_to_eat * 1000);
	//pthread_mutex_unlock(&philo->eat_mutex);
	pthread_mutex_unlock(&philo->forks[philo->left_fork]);
	pthread_mutex_unlock(&philo->forks[philo->right_fork]);
}


void	*philo_life(void *arg) // gro delbor jcomprend r // surement plus simple avec des tableaux
{
	t_philo	*philo;

	philo = (t_philo *)arg;
	pthread_mutex_lock(&philo->eat_mutex); // a tester
	philo->last_meal = get_current_time_in_ms();
	if(!philo->launch_time)
		philo->launch_time = get_current_time_in_ms();
	pthread_mutex_unlock(&philo->eat_mutex); // a tester
	 while (!time_to_die(philo->last_meal, philo->time_to_die))
	{
		philo_eat(philo);
		philo_sleep(philo);
		philo_think(philo);
	}
	printf("%d is dead\n", philo->id);
	exit(0); // peut etre interdit
}
t_philo	*init_philo(int nb, long time_to_die, long time_to_eat, long time_to_sleep) // pue la merde
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
		philo[i].time_to_eat = time_to_eat;
		philo[i].time_to_sleep = time_to_sleep;
		philo[i].time_to_die = time_to_die;
		philo[i].forks = forks;
		pthread_mutex_init(&philo[i].eat_mutex, NULL);
		pthread_create(&philo[i].thread, NULL, philo_life, &philo[i]);
		i++;
	}
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
	long time_to_eat;
	long time_to_sleep;

	if (ac == 5) // a changer
	{
		nb = ft_atoi(av[1]);
		die_time = ft_atoi(av[2]); // atoi doit peut etre gerer les long pour les ms
		time_to_eat = ft_atoi(av[3]);
		time_to_sleep = ft_atoi(av[4]);
		philo = init_philo(nb, die_time,time_to_eat, time_to_sleep);
		if (!philo)
			return (1);
		mutex_des(philo, nb); // mdr comme si ca leakait pas 
		//free(philo); // spiderman holding the trains by the strings
	}
} // minimalisme a la steve jobs
