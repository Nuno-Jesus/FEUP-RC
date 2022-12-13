#include "utils.h"

//!GET_LINE

char	*get_line(int fd)
{
	size_t	size = 1;
	ssize_t	bytes = 1;
	char	*line;
	char	c;

	line = calloc(1, size);
	if (!line)
		return (NULL);

	while (bytes > 0 && !strchr(line, '\n'))
	{
		bytes = read(fd, &c, 1);
		if (bytes == 0)
			break ;
		if (bytes == -1)
		{
			free(line);
			return (NULL);
		}
		line = realloc(line, ++size);
		line[size - 2] = c;
		line[size - 1] = '\0';
	}
	return (line);
}


//!STRMAP

char	*strmap(char const *s, char (*f)(unsigned int, char))
{
	char	*mapped;
	size_t	i;

	if (!s || !f)
		return (NULL);
	i = 0;
	mapped = (char *)malloc(strlen(s) + 1);
	if (!mapped)
		return (NULL);
	while (s[i])
	{
		mapped[i] = f(i, s[i]);
		i++;
	}
	mapped[i] = '\0';
	return (mapped);
}

//! SPLIT
static size_t	strlen_delim(char const *str, char delim)
{
	int	i;

	i = 0;
	while (str[i] && str[i] != delim)
		i++;
	return (i);
}

static int	count_words(char const *s, char delim)
{
	size_t	i;
	size_t	len;
	int		n;

	i = 0;
	n = 0;
	while (s[i])
	{
		while (s[i] == delim)
			i++;
		len = strlen_delim(s + i, delim);
		i += len;
		if (len > 0)
			n++;
	}
	return (n);
}

void	*delete_matrix(char **mat)
{
	int	i;

	i = 0;
	while (mat[i])
		free(mat[i++]);
	free(mat);
	return (NULL);
}

char	**split(char const *s, char c)
{
	size_t	i;
	int		k;
	int		num;
	char	**words;

	if (!s)
		return (NULL);
	i = 0;
	k = -1;
	num = count_words(s, c);
	words = (char **)malloc((num + 1) * sizeof(char *));
	if (!words)
		return (NULL);
	while (++k < num)
	{
		while (s[i] == c)
			i++;
		words[k] = substr(s, i, strlen_delim(s + i, c));
		if (!words[k])
			return (delete_matrix(words));
		i += strlen_delim(s + i, c);
	}
	words[num] = NULL;
	return (words);
}

//! SUBSTR
char	*substr(char const *s, unsigned int start, size_t len)
{
	size_t	i;
	char	*res;

	if (!s)
		return (NULL);
	i = 0;
	res = (char *)malloc(len + 1);
	if (res == NULL)
		return (NULL);
	if (start <= strlen(s))
	{
		while (i < len && s[i + start])
		{
			res[i] = s[i + start];
			i++;
		}
	}
	res[i] = '\0';
	return (res);
}