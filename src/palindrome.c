#include <stdio.h>
int main() {
	int n;
	char s[10];
	scanf("%d", &n);
	int i=0;
	while(n)
	{
		int now=n%10;
		s[i]='0'+now;
		n=n/10;
		i++;
	}
	int l=0,r=i-1;
	int flag=0;
	while(l<=r)
	{
		if(s[l]!=s[r])
		{
			flag=1;
			break;
		}
		l++;
		r--;
	}
	if (flag==0) {
		printf("Y\n");
	} else {
		printf("N\n");
	}
	return 0;
}
