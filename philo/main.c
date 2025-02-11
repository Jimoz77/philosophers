#include "philo.h"

t_philo *init_philo(int nb)
{
	t_philo *philo;
	int i;

	philo = malloc(sizeof(t_philo) * nb);
	if (!philo)
		return (NULL);
	i = 0;
	while (i < nb)
	{
		philo[i].id = i;
		philo[i].left_fork = i;
		philo[i].right_fork = (i + 1) % nb;
		philo[i].eat_count = 0;
		i++;
	}
	return (philo);
}
// chaque philo doit avoir un thread

int main(int ac, char **av)
{
	if(ac == 5 || ac == 6)
	{
		init_philo(av[1]);
	}
}