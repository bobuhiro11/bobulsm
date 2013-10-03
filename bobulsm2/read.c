/*
 * parser 
 *
 * FROM: "FILENAME"
 *
 * F 0 root
 * F 1 app
 * F 2 assets
 * F 3 images
 *
 * TO:   STDOUT
 *
 * F root
 * F |-- app
 * F |   |-- assets
 * F |   |   |-- images
 * F |   |   |-- javascripts
 * F |   |   `-- stylesheets
 * F |   |-- controllers
 *
 */
#include<stdio.h>
#include<string.h>
#include<stdlib.h>

#define FILENAME "data2.dat"
#define BUFSIZE  256
#define NBUF  	 1024



void parse(void)
{
	FILE *rfp=NULL;
	int depth=0, old_depth=0;
	char bufs[NBUF][BUFSIZE];
	char tmp[BUFSIZE];
	char *buf;
	int i,len,j,n;

	unsigned int flag = 0x00000000;	// appear

	if(!(rfp = fopen(FILENAME,"r"))){
		fprintf(stderr, "file \"%s\" can't open.\n", FILENAME);
		exit(EXIT_FAILURE);
	}

	i = 0;
	while(fgets(bufs[i],BUFSIZE-1,rfp)) i++;


	n = i;
	i--;
	while(i>=0){
		buf = bufs[i];
		depth = buf[2] - '0';		

		if( strlen(buf) + depth*4 > BUFSIZE) {
			fprintf(stderr, "buffer error.\n");
			exit(EXIT_FAILURE);
		}

		sprintf(tmp, "%c ", buf[0]);
		for(j=1;j<depth;j++){
			if( (flag>>j)  & 0x00000001){
				strcat(tmp,"|   ");
			}else{
				strcat(tmp,"    ");
			}
		}
		if( depth==0){

		}else if( (flag>>depth)  & 0x00000001){
			strcat(tmp,"|-- ");
		}else{
			strcat(tmp,"`-- ");
		}
		strcat(tmp, buf+4);
		strcpy(buf, tmp);

		flag &= ~(0xFFFFFFFF << (depth+1));
		flag |=   0x00000001 << depth;
		i--;
	}

	for(i=0;i<n;i++){
		printf("%s",bufs[i]);
	}
}

int main(void)
{
	parse();
	return 0;
}
