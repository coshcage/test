/* This is a calendar.
 */
#include <stdio.h>
#include <time.h>

const char * szwtitle = " Sun Mon Tue Wed Thu Fri Sat";

int DayOfWeek(int y, int m, int d)	/* Sakamoto's methods. */
{
	static int t[] = { 0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4 };
	if (m < 3)
	{
		y -= 1;
	}
	return (y + y / 4 - y / 100 + y / 400 + t[m - 1] + d) % 7;
}

void DrawCalendar(struct tm * buf)
{
	int i, e, f;
	switch (buf->tm_mon + 1)
	{
	case 1: case  3: case  5: case 7:
	case 8: case 10: case 12: e = 31;
		break;
	case 2:
		if (0 == buf->tm_year % 4 || 0 == buf->tm_year % 400 || (0 == buf->tm_year % 3200 && 0 == buf->tm_year % 172800))
			e = 29;
		else
			e = 28;
		break;
	default:
		e = 30;
	}
	printf("%s\n", szwtitle);
	for (i = 0, f = (DayOfWeek(buf->tm_year + 1900, buf->tm_mon, buf->tm_mday) + 1) % 7; i < f; ++i)
	{
		printf("    ");
	}
	for (i = 1; i <= e; ++i)
	{
		if (i == buf->tm_mday)
			printf("(%2d)", i);
		else
			printf(" %2d ", i);
		if (0 == (i + f) % 7)
			printf("\n");
	}
	printf("\n");
}

int main()
{
	time_t t;

	struct tm buf;
	t = time(NULL);

	printf(" %s\n", asctime(localtime(&t)));
	DrawCalendar(localtime(&t));
	return 0;
}
