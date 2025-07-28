/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jimpa <jimpa@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/28 18:33:23 by jimpa             #+#    #+#             */
/*   Updated: 2025/07/28 20:24:15 by jimpa            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#define PHILO_H
#define PHILO_H

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>

typedef struct s_philo
{
	int					id;
	int					hungriest;
	long				current_time;
	long				launch_time;
	long				time_to_die;
	long				time_to_eat;
	long				time_to_sleep;
	long				last_meal;
	int					philo_count;
	int					left_fork;
	int					right_fork;
	int					eat_count;
	int					last_used_fork;
	int					must_eat;
	int					end;
	int					dead;
	int					*simulation_stop;
	pthread_mutex_t		meal_mutex;
	pthread_mutex_t		dead_mutex;
	pthread_mutex_t		eat_mutex;
	pthread_mutex_t		*forks;
	pthread_t			monitor_thread;
	pthread_t			thread;
	pthread_mutex_t		*stop_mutex;
	pthread_mutex_t		*print_mutex;
}	t_philo;

typedef struct s_main_vars
{
	int		nb;
	long	die_time;
	long	time_to_eat;
	long	time_to_sleep;
	int		must_eat;
}	t_main_vars;

typedef struct s_monitor_vars
{
	int			philo_count;
	int			all_ate_enough;
	int			i;
	long		current_time;
}	t_monitor_vars;

typedef struct s_init_vars
{
	int					i;
	pthread_mutex_t		*forks;
	pthread_t			monitor_thread;
	long				global_launch_time;
	pthread_mutex_t		*shared_stop_mutex;
	pthread_mutex_t		*shared_print_mutex;
	int					*shared_simulation_stop;
}	t_init_vars;

// utils.c
int				ft_atoi(const char *str);
long			get_current_time_in_ms(void);
int				simulation_stopped(t_philo *philo);
void			safe_printf(t_philo *philo, char *message);
void			mutex_des(t_philo *philo, int nb);
void			stop_simulation(t_philo *philo);

// philo_life.c
void			*philo_life(void *arg);
void			philo_eat(t_philo *philo);
void			philo_sleep(t_philo *philo);
void			philo_think(t_philo *philo);
int				time_to_die(long last_meal, long time_to_die,
					long current_time, long launch_time);

// philo_eat.c
void			handle_single_philo(t_philo *philo);
void			determine_fork_order(t_philo *philo, int *first, int *second);
int				acquire_forks(t_philo *philo, int first, int second);
void			update_eating_data(t_philo *philo);
void			perform_eating(t_philo *philo, int first, int second);

// init_philo.c

pthread_mutex_t	*init_forks(int nb);
int				init_shared_resources(t_init_vars *v2);
void			configure_philosopher(t_philo *philo, t_main_vars v,
					t_init_vars *v2, int index);
void			init_philosophers_data(t_philo *philo, t_main_vars v,
					t_init_vars *v2);
void			create_threads(t_philo *philo, t_main_vars v, t_init_vars *v2);
void			join_threads(t_philo *philo, t_main_vars v, t_init_vars *v2);

// monitor.c
void			*global_monitor(void *arg);
int				check_philosopher_death(t_philo *philos, t_monitor_vars *v);
int				monitor_death_loop(t_philo *philos, t_monitor_vars *v);
int				check_individual_eat_count(t_philo *philos, t_monitor_vars *v);
int				check_all_ate_enough(t_philo *philos, t_monitor_vars *v);