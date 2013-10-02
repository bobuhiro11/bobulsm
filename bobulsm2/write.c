/*
 * parser 
 *
 * FROM: "FILENAME"
 *
 * F root
 * F |-- app
 * F |   |-- assets
 * F |   |   |-- images
 * F |   |   |-- javascripts
 * F |   |   `-- stylesheets
 * F |   |-- controllers
 *
 * TO:   STDOUT
 *
 * F 0 root
 * F 1 app
 * F 2 assets
 * F 3 images
 *
 */
#include<stdio.h>
#include<string.h>

#define FILENAME "data.dat"
#define BUFSIZE  256

void parse(void){
	FILE *rfp=NULL;
	int depth=0;
	char buf[BUFSIZE];
	int i;

	if(!(rfp = fopen(FILENAME,"r"))){
		printf("file not found.\n");
		return;
	}

	while(fgets(buf,BUFSIZE-1,rfp)){
		depth = 0;

		printf("%c ",buf[0]);
		for(i=1; buf[i]!='\0'; i++){
			if(buf[i]=='|' || buf[i]=='`' || buf[i]==' ' || buf[i]=='-'){
				depth++;
			}else{
				break;
			}
		}
		depth = depth/4;
		printf("%d %s", depth, buf+i);
	}
}

int main(void)
{
	parse();
	return 0;
}
