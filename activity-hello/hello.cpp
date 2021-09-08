#include <stdio.h>
#include <limits.h>
#include <unistd.h>

int main () {
	char hostname[HOST_NAME_MAX + 1];
	gethostname(hostname, HOST_NAME_MAX + 1);

	printf("%s\n", hostname);

	return 0;
}
