#include "libmx.h"

int mx_strcmp(const char *s1, const char *s2) {
	int i = 0;
	while (s1[i] != '\0' && s2[i] != '\0') {
		if (s1[i] != s2[i]) break;
		i++;
	}

	return s1[i] - s2[i];
}

/*int main() {
	printf("%d\n", mx_strcmp("a", "a"));
	printf("%d\n", mx_strcmp("a", "aa"));
	printf("%d\n", mx_strcmp("b", "a"));
}*/
