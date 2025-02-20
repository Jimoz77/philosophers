
int	ft_atoi(const char *str)
{
	int	i;
	long	is_nega;
	long	res;

	i = 0;
	res = 0;
	is_nega = 1;
	while (str[i] == ' ' || str[i] == '\t' || str[i] == '\n'
		|| str[i] == '\v' || str[i] == '\f' || str[i] == '\r')
		i++;
	if (str[i] == '-' || str[i] == '+')
	{
		if (str[i] == '-')
			is_nega = -1;
		i++;
	}
	while (str[i] >= '0' && str[i] <= '9')
		res = (res * 10) + (str[i++] - '0');
	return (res * is_nega);
}