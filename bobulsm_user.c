/*
 *  NOTICE:
 *              This is User mode program.
 *  Usage:  
 *              ./bobulsm_user  first_program login_shell_program
 *
 *              first_program:          first program of bobulsm check (e.g. /bin/foo )
 *              login_shell_program:    login shell (e.g. /bin/bash )
 *
 */

#include<stdio.h>
#include<string.h>
#include<sys/stat.h>
#include<fcntl.h>

#define CONF_DIR  "/etc/bobulsm"
#define CONF_FILE "/etc/bobulsm/policy"
#define POLICY    "/sys/kernel/security/bobulsm/policy"
#define EXEC	  "exec"
#define BUFLEN    512

/*
 * analyze filename program and its child program
 *
 */
void analyze(FILE **wfp, char *filename, int depth)
{
	FILE *rfp;
	char buf[BUFLEN];
	char *p,*q;
	int i,size;
	
	/* write filename to CONF_FILE */
	for(i=0;i<depth;i++)
		buf[i] = '*';
	buf[i] = '\0';
	fprintf(*wfp,"%s%s\n",buf,filename);

	/* analyze child file */
	if(!(rfp = fopen(filename,"r"))){
		return;
	}
	while(fgets(buf,BUFLEN,rfp)){
		size = strlen(buf)-1;
		buf[size] = '\0';
		
		p = buf;
		while(*p){
			if(!strncmp(p,EXEC,strlen(EXEC))){
				p += strlen(EXEC);
				while(*p==' ') p++;
				q = p;
				while(*q!=' ' && *q!='0') q++;
				*q = '\0';
				analyze(wfp,p,depth+1);
				p = q;
			}
			p++;
		}
	}
	fclose(rfp);
}

int main(int argc, char **argv)
{
	FILE *rfp,*wfp;
	int fd;
	char buf[BUFLEN];
	int size;
	int i;
	struct stat st;
	
	if(argc!=1 && argc!= 3){
		fprintf(stderr,"Usage: ./bobulsm_user first_program login_shell_progra\n");
		exit(1);
	}

	/* make CONF_DIR directory*/
	if(stat(CONF_DIR,&st)){
		if(mkdir(CONF_DIR,0755)){
			fprintf(stderr,"bobulsm_user: Can't make directory \"%s\".\n",
					CONF_DIR);
			exit(1);
		}
	}

	/* analyze program file */
	if(argc == 3){
		if(wfp = fopen(CONF_FILE,"w")){
			analyze(&wfp,argv[1],1);
			fclose(wfp);
		}else{
			fprintf(stderr,"bobulsm_user: Failure to open \"%s\".\n",CONF_FILE);
			exit(1);
		}
	}

	/* write policy from CONF_FILE to POLICY simply */
	if(rfp = fopen(CONF_FILE,"r")){
		while(fgets(buf,BUFLEN,rfp)){
			size = strlen(buf);

			write(1,buf,size);

			fd = open(POLICY,O_WRONLY);
			if(fd<0){
				fprintf(stderr,"bobulsm_user: Failure to open \"%s\".\n",POLICY);
				exit(1);
			}
			write(fd,buf,size);
			close(fd);

		}
		fclose(rfp);
	}else{
		fprintf(stderr,"bobulsm_user: Failure to open \"%s\".\n",CONF_FILE);
		exit(1);
	}

	return 0;
}