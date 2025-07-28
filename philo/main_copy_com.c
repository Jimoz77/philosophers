/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main_copy.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jimpa <jimpa@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/12 17:19:38 by jimpa             #+#    #+#             */
/*   Updated: 2025/07/28 16:46:22 by jimpa            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"


//Récupère le temps actuel en millisecondes en utilisant gettimeofday().
//Convertit les secondes en millisecondes et ajoute les microsecondes converties.

long	get_current_time_in_ms(void)
{
	struct timeval	current_time;

	gettimeofday(&current_time, NULL);
	return (current_time.tv_sec * 1000 + current_time.tv_usec / 1000);
}


// Vérifie si la simulation doit s'arrêter de manière thread-safe.
//Utilise un mutex pour accéder à la variable partagée simulation_stop
//et retourne son état.
int	simulation_stopped(t_philo *philo)
{
	int	result;

	pthread_mutex_lock(philo->stop_mutex);
	result = *(philo->simulation_stop);  // Déréférencer le pointeur
	pthread_mutex_unlock(philo->stop_mutex);
	return (result);
}


//Affiche les messages de manière thread-safe.
//Vérifie d'abord si la simulation est arrêtée, puis affiche le timestamp relatif,
//l'ID du philosophe et le message.
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
//Vérifie si un philosophe devrait mourir.
//Calcule le temps écoulé depuis le dernier repas et le compare avec time_to_die.

int	time_to_die(long last_meal, long time_to_die,
	long current_time, long launch_time)
{
	long	diff;

	diff = (current_time - launch_time) - last_meal;
	return (diff >= time_to_die);
}

//Arrête la simulation en mettant la variable partagée simulation_stop à 1,
//de manière thread-safe avec un mutex.
void	stop_simulation(t_philo *philo)
{
	pthread_mutex_lock(philo->stop_mutex);
	*(philo->simulation_stop) = 1;  // Déréférencer le pointeur
	pthread_mutex_unlock(philo->stop_mutex);
}

// Monitor centralisé - SANS variables globales
//Thread de surveillance centralisé qui :

//Vérifie la mort : Parcourt tous les philosophes et vérifie si l'un d'eux doit mourir
//Vérifie si tous ont assez mangé : Si must_eat > 0, vérifie si tous les philosophes ont atteint leur quota
//Arrête la simulation dès qu'une condition d'arrêt est remplie
//Vérifie toutes les 1ms avec usleep(1000)
void *global_monitor(void *arg)
{
    t_philo *philos = (t_philo *)arg;
    int philo_count = philos[0].philo_count;
    int all_ate_enough = 0;
    
    while (!simulation_stopped(&philos[0])) {
        // Vérifier si un philosophe est mort
        for (int i = 0; i < philo_count; i++) {
            pthread_mutex_lock(&philos[i].eat_mutex);
            long current_time = get_current_time_in_ms();
            
            if (time_to_die(philos[i].last_meal, philos[i].time_to_die, 
                           current_time, philos[i].launch_time)) {
                printf("%ld %d died\n", current_time - philos[i].launch_time, philos[i].id);
                pthread_mutex_unlock(&philos[i].eat_mutex);
                stop_simulation(&philos[0]);
                return NULL;
            }
            pthread_mutex_unlock(&philos[i].eat_mutex);
        }
        
        // Vérifier si tous ont assez mangé (si must_eat > 0)
        if (philos[0].must_eat > 0) {
            all_ate_enough = 1;
            for (int i = 0; i < philo_count; i++) {
                pthread_mutex_lock(&philos[i].eat_mutex);
                if (philos[i].eat_count < philos[i].must_eat) {
                    all_ate_enough = 0;
                    pthread_mutex_unlock(&philos[i].eat_mutex);
                    break;
                }
                pthread_mutex_unlock(&philos[i].eat_mutex);
            }
            if (all_ate_enough) {
                stop_simulation(&philos[0]);
                return NULL;
            }
        }
        
        usleep(1000); // Vérifier toutes les 1ms
    }
    return NULL;
}

//Action de réflexion : attend 200 microsecondes et affiche le message "is thinking".
void	philo_think(t_philo *philo)
{
	if (!simulation_stopped(philo))
	{
		usleep(200);
		safe_printf(philo, "is thinking");
	}
	return ;
}
//Action de sommeil : affiche "is sleeping" puis dort pendant time_to_sleep millisecondes.
void 	philo_sleep(t_philo *philo)
{
	if (!simulation_stopped(philo))
	{
		safe_printf(philo, "is sleeping");
		usleep(philo->time_to_sleep * 1000);
	}
	return ;
}

//Action la plus complexe :

//Cas spécial : Si un seul philosophe, il meurt immédiatement
//Évite les deadlocks : Prend toujours les fourchettes dans l'ordre (plus petit index d'abord)
//Prend les deux fourchettes avec les mutex appropriés
//Met à jour les données : last_meal et eat_count de manière thread-safe
//Mange pendant time_to_eat millisecondes
//Libère les fourchettes
void	philo_eat(t_philo *philo)
{
	int	first_fork;
	int	second_fork;

	if (philo->philo_count == 1)
	{
		printf("%ld %d died\n", philo->time_to_die, philo->id);
		stop_simulation(philo);
		return ;
	}
	// Ordre des fourchettes pour éviter les deadlocks
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
	if (!simulation_stopped(philo))
	{
		// Prendre les fourchettes
		pthread_mutex_lock(&philo->forks[first_fork]);
		pthread_mutex_lock(&philo->forks[second_fork]);
		if (simulation_stopped(philo))
		{
			pthread_mutex_unlock(&philo->forks[first_fork]);
			pthread_mutex_unlock(&philo->forks[second_fork]);
			return ;
		}
		philo->current_time = get_current_time_in_ms();
		safe_printf(philo, "has taken a fork");
		safe_printf(philo, "has taken a fork");
		safe_printf(philo, "is eating");
		// Mettre à jour les données de repas
		pthread_mutex_lock(&philo->eat_mutex);
		philo->last_meal = philo->current_time - philo->launch_time;
		philo->eat_count++;
		pthread_mutex_unlock(&philo->eat_mutex);
		usleep(philo->time_to_eat * 1000);
		// Libérer les fourchettes
		pthread_mutex_unlock(&philo->forks[first_fork]);
		pthread_mutex_unlock(&philo->forks[second_fork]);
	}
}


//Fonction principale exécutée par chaque thread de philosophe :

//Initialise last_meal à 0
//Boucle tant que la simulation continue ET que le quota de repas n'est pas atteint
//Exécute le cycle : manger → dormir → penser
//Vérifie à chaque étape si la simulation doit s'arrêter

void	*philo_life(void *arg)
{
	t_philo	*philo;

	philo = (t_philo *)arg;
	pthread_mutex_lock(&philo->eat_mutex);
	philo->last_meal = 0;
	// launch_time déjà initialisé dans init_philo
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


//Détruit tous les mutex et libère la mémoire allouée dynamiquement.
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

//Fonction d'initialisation :

//Allocation mémoire pour les philosophes et fourchettes
//Initialisation des mutex (fourchettes, stop, print)
//Configuration de chaque philosophe avec ses paramètres
//Variables partagées : stop_mutex, print_mutex, simulation_stop
//Création des threads (monitoring + philosophes)
//Attente de fin avec pthread_join
t_philo	*init_philo(int nb, long time_to_die, long time_to_eat, long time_to_sleep, int must_eat)
{
	t_philo	*philo;
	int		i;
	pthread_mutex_t *forks;
	pthread_t monitor_thread;
	long global_launch_time;

	// Temps de lancement unique pour tous
	global_launch_time = get_current_time_in_ms();

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
	// Initialiser les fourchettes
	while (i < nb)
	{
    	pthread_mutex_init(&forks[i], NULL);
		i++;
	}
	// Initialiser le mutex de stop (partagé par tous)
	pthread_mutex_t *shared_stop_mutex = malloc(sizeof(pthread_mutex_t));
	pthread_mutex_t *shared_print_mutex = malloc(sizeof(pthread_mutex_t));
	int *shared_simulation_stop = malloc(sizeof(int));
	if (!shared_stop_mutex || !shared_simulation_stop || !shared_print_mutex) {
		free(philo);
		free(forks);
		return (NULL);
	}
	pthread_mutex_init(shared_stop_mutex, NULL);
	pthread_mutex_init(shared_print_mutex, NULL);
	*shared_simulation_stop = 0;
	// Initialiser les philosophes
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
		philo[i].launch_time = global_launch_time;  // Même temps pour tous
		philo[i].last_meal = 0;
		philo[i].must_eat = must_eat;
		philo[i].stop_mutex = shared_stop_mutex;       // Pointeur partagé
		philo[i].print_mutex = shared_print_mutex;     // Pointeur partagé
		philo[i].simulation_stop = shared_simulation_stop;  // Pointeur partagé
		pthread_mutex_init(&philo[i].eat_mutex, NULL);
		i++;
	}
	// Créer le thread de monitoring centralisé
	pthread_create(&monitor_thread, NULL, global_monitor, philo);
	// Créer les threads des philosophes
	i = 0;
	while (i < nb)
	{
		pthread_create(&philo[i].thread, NULL, philo_life, &philo[i]);
		i++;
	}
	// Attendre la fin du monitoring
	pthread_join(monitor_thread, NULL);
	// Attendre la fin de tous les philosophes
	i = 0;
	while (i < nb)
	{
		pthread_join(philo[i].thread, NULL);
		i++;
	}
	return (philo);
}

int	main(int ac, char **av)
{
	t_philo	*philo;
	int		nb;
	long	die_time;
	long	time_to_eat;
	long	time_to_sleep;
	int		must_eat;

	if (ac == 5 || ac == 6)
	{
		must_eat = 0;
		nb = ft_atoi(av[1]);
		die_time = ft_atoi(av[2]);
		time_to_eat = ft_atoi(av[3]);
		time_to_sleep = ft_atoi(av[4]);
		if (ac == 6)
		{
			must_eat = ft_atoi(av[5]);
		}

		philo = init_philo(nb, die_time, time_to_eat, time_to_sleep, must_eat);
		if (!philo)
			return (1);

		mutex_des(philo, nb);
		free(philo);
	}
	return (0);
}