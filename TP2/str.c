#include "str.h"

char	*strtrim(char const *s1, char const *set)
{
	if (!s1 || !set)
		return (NULL);
		
	size_t	end;
	size_t	start;

	start = 0;
	end = strlen(s1);
	while (strchr(set, s1[start]) && s1[start])
		start++;
	while (strchr(set, s1[end]) && end > start)
		end--;
	return (substr(s1, start, (end - start) + 1));
}

//!GET_LINE
char	*get_line(int fd)
{
	size_t	size = 1;
	ssize_t	bytes = 1;
	char	*line;
	char	c;

	if (!(line = calloc(1, size)))
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
	if (line[0] == '\0')
	{
		free(line);
		return NULL;
	}
	return (line);
}

//!STRMAP
char	*strmap(char const *s, char (*f)(unsigned int, char))
{
	if (!s || !f)
		return (NULL);

	char	*mapped;
	size_t	i;

	if (!(mapped = malloc(strlen(s) + 1)))
		return (NULL);
		
	for (i = 0; s[i]; i++)
		mapped[i] = f(i, s[i]);
	mapped[i] = '\0';

	return (mapped);
}

//! SPLIT
size_t	strlen_delim(char const *str, char delim)
{
	int	i;

	i = 0;
	while (str[i] && str[i] != delim)
		i++;
	return (i);
}

int	count_words(char const *s, char delim)
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

void	delete_matrix(char **mat)
{
	int	i;

	i = 0;
	while (mat[i])
		free(mat[i++]);
	free(mat);
}

char	**split(char const *s, char c)
{
	if (!s)
		return (NULL);

	char **words;
	size_t i = 0;
	int	num = count_words(s, c);

	if (!(words = malloc((num + 1) * sizeof(char *))))
		return (NULL);

	for (int k = 0; k < num; k++)
	{
		while (s[i] == c)
			i++;
		words[k] = substr(s, i, strlen_delim(s + i, c));
		if (!words[k])
		{
			delete_matrix(words);
			return (NULL);
		}
		i += strlen_delim(s + i, c);
	}
	words[num] = NULL;
	return (words);
}

//! SUBSTR
char	*substr(char const *s, unsigned int start, size_t len)
{
	if (!s)
		return (NULL);

	size_t	i = 0;
	char	*res;

	if (!(res = malloc(len + 1)))
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