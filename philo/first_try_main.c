/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jimpa <jimpa@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/12 17:19:38 by jimpa             #+#    #+#             */
/*   Updated: 2025/07/04 16:48:33 by jimpa            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

int someone_dead(t_philo *base_philo, int philo_count)
{
    int i = 0;
    int result = 0;
    
    while(i < philo_count)
    {
        pthread_mutex_lock(&base_philo[i].dead_mutex);
        if (base_philo[i].dead == 1)
        {
            result = 1;
            pthread_mutex_unlock(&base_philo[i].dead_mutex);
            break;
        }
        pthread_mutex_unlock(&base_philo[i].dead_mutex);
        i++;
    }
    return (result);
}

void *is_dead(void *arg)
{
	t_philo *philo = (t_philo *)arg;
	int i = 0;

	while(i < (philo->philo_count))
	{
		pthread_mutex_lock(&philo[i].dead_mutex);
		philo[i].dead = 1;
		pthread_mutex_unlock(&philo[i].dead_mutex);
		i++;
	}
	return (NULL);
}

int checkif_end(void *arg)
{
    t_philo *philo = (t_philo *)arg;
    int i = 0;
    int result = 0;
    
    while (i < philo->philo_count)
    {
        // Si vous avez un mutex pour 'end', sinon supprimez ces lignes
        if (philo[i].end == 1)
        {
            result = 1;
            break;
        }
        i++;
    }
    return (result);
}
void *end_of_thread(void *arg)
{
	t_philo *philo = (t_philo *)arg;
	int i = 0;
	int count = philo->philo_count;
	while (i < count)
	{
		//printf("end of thread\n");
		philo[i].end = 1;
		i++;
	}

	/* pthread_join(philo->thread, NULL);
	pthread_join(philo->monitor_thread, NULL); */
	return (NULL);
}

void	mutex_des(t_philo *philo, int nb) // full random
{
	int	i;

	i = 0;
	while (i < nb)
	{
		pthread_mutex_destroy(&philo[i].eat_mutex);
		pthread_mutex_destroy(&philo->forks[i]);
		//free(&philo[i]);
		i++;
	}
	free(philo->forks);
}


long get_current_time_in_ms()
{
	struct timeval current_time;
	gettimeofday(&current_time, NULL);
	return (current_time.tv_sec * 1000 + current_time.tv_usec / 1000);
}
int time_to_die(long last_meal, long time_to_die, long current_time, long launch_time)
{
	long diff;
	diff = (current_time - launch_time) - last_meal;
	//printf("| diff = %ld |\n", diff);
	//printf("| current_time = %ld |\n", current_time - launch_time);
	//printf("| last_meal = %ld |\n", last_meal); */
	return(diff >= time_to_die);
}
void *monitor_philo(void *arg){
    t_philo *philo = (t_philo *)arg;
    long last_meal_time = philo->last_meal;
	if(!philo->launch_time)
		philo->launch_time = get_current_time_in_ms();

    while (checkif_end(arg) == 0 && someone_dead(philo, philo->philo_count) != 1) {
        philo->current_time = get_current_time_in_ms();
		
        if (someone_dead(philo, philo->philo_count) != 1 && time_to_die(philo->last_meal, philo->time_to_die, get_current_time_in_ms(), philo->launch_time)) {
            printf("%011ld %d died\n", philo->current_time - philo->launch_time, philo->id);
            is_dead(philo);
			return NULL; // ATTENTION EXIT VITALE QUI FAUT CHERCHER A ENLEVER 
        }
        if (last_meal_time != philo->last_meal) {
            //printf("Philosopher %d started eating at %ld ms\n", philo->id, philo->last_meal);
            last_meal_time = philo->last_meal;
        }
        usleep(1000); // Attendre 10ms avant de vérifier à nouveau
    }
	return NULL;
}


t_philo *find_philo_to_eat(t_philo *philos, int philo_count) {
    t_philo *philo_to_eat = NULL;
    long min_time_to_die = 0;
    int min_eat_count = 0;

    for (int i = 0; i < philo_count; i++) {
        long time_to_die = philos[i].time_to_die - (get_current_time_in_ms() - philos[i].last_meal);
        if (time_to_die < min_time_to_die || (time_to_die == min_time_to_die && philos[i].eat_count < min_eat_count)) {
            min_time_to_die = time_to_die;
            min_eat_count = philos[i].eat_count;
            philo_to_eat = &philos[i];
        }
    }
    return philo_to_eat;
}








/* int set_hungriest(t_philo *philos) // fonctionne mais engendre data race 
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
} */
void   philo_think(t_philo *philo) // la meilleur fonc du projet
{
	if (someone_dead(philo, philo->philo_count) != 1 && philo->dead != 1)
	{
		philo->current_time = get_current_time_in_ms();
		usleep(100);
		printf("%ld %d is thinking\n",philo->current_time - philo->launch_time, philo->id);
	}
	return;
}
void  philo_sleep(t_philo *philo) // béton
{
	if (someone_dead(philo, philo->philo_count) != 1 && philo->dead != 1)
	{
		philo->current_time = get_current_time_in_ms();
		printf("%ld %d is sleeping\n",philo->current_time - philo->launch_time, philo->id); // verifier si mettre apres le usleep
		usleep(philo->time_to_sleep * 1000);
	}
	return;
}


void	philo_eat(t_philo *philo) // attention probleme de décalage d'une ms
{
	int first_fork;
	int second_fork;

	if(!philo->last_used_fork)
		philo->last_used_fork = -1;

	if (philo->philo_count == 1)
	{
		printf("%ld %d died\n", philo->time_to_die, philo->id);
		is_dead(philo);
		return;
	}
	if (philo->left_fork < philo->right_fork)
	{
		first_fork = philo->left_fork;
		second_fork = philo->right_fork;
	}
	else
	{
		first_fork = philo->right_fork;
		second_fork = philo->left_fork;
	}
	/* if (philo->last_used_fork == first_fork)
	{
		int temp = first_fork;
		first_fork = second_fork;
		second_fork = temp;
	} */
	

	/* if(philo->left_fork < philo->right_fork) // fork de gauche en premier si id pair
	{
		pthread_mutex_lock(&philo->forks[philo->left_fork]);
		pthread_mutex_lock(&philo->forks[philo->right_fork]);
	}
	else
	{
		pthread_mutex_lock(&philo->forks[philo->right_fork]);
		pthread_mutex_lock(&philo->forks[philo->left_fork]);
	}
 */
	if (someone_dead(philo, philo->philo_count) != 1 && philo->dead != 1)
	{
		pthread_mutex_lock(&philo->forks[first_fork]);
		pthread_mutex_lock(&philo->forks[second_fork]);
		//philo->last_used_fork = second_fork;
		//pthread_mutex_lock(&philo->eat_mutex); 
		if(someone_dead(philo, philo->philo_count) == 1)
		{
			pthread_mutex_unlock(&philo->forks[first_fork]);
			pthread_mutex_unlock(&philo->forks[second_fork]);
			return;
		}
		
		philo->current_time = get_current_time_in_ms();
		/* if(time_to_die(philo->last_meal, philo->time_to_die, get_current_time_in_ms(), philo->launch_time))
			{
				printf("%011ld %d died\n", philo->current_time - philo->launch_time,philo->id);
				mutex_des(philo, philo->philo_count);
				exit(0);
			} */
		printf("%ld %d has taken a fork\n", philo->current_time - philo->launch_time, philo->id);
		printf("%ld %d has taken a fork\n", philo->current_time - philo->launch_time, philo->id);
		printf("%ld %d is eating\n", philo->current_time - philo->launch_time, philo->id);
		
		pthread_mutex_lock(&philo->eat_mutex);
		philo->last_meal = philo->current_time - philo->launch_time;
		philo->eat_count++;
		usleep(philo->time_to_eat * 1000);
		pthread_mutex_unlock(&philo->eat_mutex);


		philo->last_used_fork = second_fork;
		pthread_mutex_unlock(&philo->forks[first_fork]);
		pthread_mutex_unlock(&philo->forks[second_fork]);
	}

	/* pthread_mutex_unlock(&philo->forks[philo->left_fork]);
	pthread_mutex_unlock(&philo->forks[philo->right_fork]); */
}


void	*philo_life(void *arg) 
{
	t_philo	*philo;
	static int first = 0;

	philo = (t_philo *)arg;
	//pthread_mutex_lock(&philo->eat_mutex); // a tester
	philo->last_meal = 0;
	if(!philo->launch_time)
		philo->launch_time = get_current_time_in_ms();
	//pthread_mutex_unlock(&philo->eat_mutex); // a tester
	 while (someone_dead(philo, philo->philo_count) != 1  && (philo->eat_count < philo->must_eat || philo->must_eat == 0))
	{
		if(someone_dead(philo, philo->philo_count) != 1 && first == 0)
			philo_eat(philo);
		else
			return (NULL);
		if (someone_dead(philo, philo->philo_count) != 1 && first == 0)
			philo_sleep(philo);
		else
			return (NULL);
		if(someone_dead(philo, philo->philo_count) != 1 && first == 0)
			philo_think(philo);
		else
			return (NULL);
	}

	if (philo->end == 0 && someone_dead(philo, philo->philo_count) != 1 && first == 0)
	{
		first = 1;
		end_of_thread(philo);
	}
	//mutex_des(philo, philo->philo_count);
	//exit(0); // peut etre interdit
	return (NULL);
}
t_philo	*init_philo(int nb, long time_to_die, long time_to_eat, long time_to_sleep, int must_eat) // pue la merde
{
	t_philo	*philo;
	int		i;
	pthread_mutex_t *forks;

	i = 0;

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
		philo[i].id = i + 1;
		philo[i].left_fork = i;
		philo[i].right_fork = (i + 1) % nb;
		philo[i].philo_count = nb;
		philo[i].eat_count = 0;
		philo[i].time_to_eat = time_to_eat;
		philo[i].time_to_sleep = time_to_sleep;
		philo[i].time_to_die = time_to_die;
		philo[i].forks = forks;
		philo[i].launch_time = 0;
		philo[i].last_meal = 0;
		philo[i].last_used_fork = -1;
		philo[i].must_eat = must_eat;
		philo[i].end = 0;
		philo[i].dead = 0;
		pthread_mutex_init(&philo[i].eat_mutex, NULL);
		pthread_mutex_init(&philo[i].dead_mutex, NULL);
		i++;
	}
	i = 0;
	while (i < nb)
	{
		pthread_create(&philo[i].thread, NULL, philo_life, &philo[i]);
		pthread_create(&philo[i].monitor_thread, NULL, monitor_philo, &philo[i]);
		i++;
	}
	i = 0;
	while (i < nb)
	{
		pthread_join(philo[i].thread, NULL); // surement pas comme ca qu on l utilise
		pthread_join(philo[i].monitor_thread, NULL);
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
	int must_eat;

	if (ac == 5 || ac == 6) // a changer
	{
		must_eat = 0;
		nb = ft_atoi(av[1]);
		die_time = ft_atoi(av[2]); // atoi doit peut etre gerer les long pour les ms
		time_to_eat = ft_atoi(av[3]);
		time_to_sleep = ft_atoi(av[4]);
		if (ac == 6)
		{
			must_eat = ft_atoi(av[5]);
		}

		philo = init_philo(nb, die_time,time_to_eat, time_to_sleep, must_eat);
		if (!philo)
			return (1);

	
		mutex_des(philo, nb); // mdr comme si ca leakait pas 
		free(philo); // spiderman holding the trains by the strings
	}
} // minimalisme a la steve jobs
