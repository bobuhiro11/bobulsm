/*
 *  NOTICE:
 *              This is user mode program.
 *              All program file use absolute path.
 *  Usage:  
 *              ./bobulsm_user  first_program login_shell_program
 *
 *              first_program:          first program of bobulsm check (e.g. /bin/foo )
 *              login_shell_program:    login shell (e.g. /bin/bash )
 *
 */

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<fcntl.h>

#define CONF_DIR  "/etc/bobulsm"
#define CONF_FILE "/etc/bobulsm/policy"
#define POLICY    "/sys/kernel/security/bobulsm/policy"
#define SSHD	  "/usr/sbin/sshd"
#define EXEC	  "exec"
#define BUFLEN    512

/*
 * get absolute path 
 * return 0 if success
 *             negative value otherwise. 
 */
int absolute_path(char *buf, char *pwd){
}

/*
 * analyze filename program and its child program
 *
 */
void analyze(FILE **wfp, const char *filename, int depth, const char *shell)
{
	FILE *rfp;
	char buf[BUFLEN];
	char *p,*q;
	int i,size;
	
	/* write filename to CONF_FILE */
	for(i=0;i<depth;i++)
		buf[i] = '*';
	buf[i] = '\0';
	if(depth>2 && !strcmp(filename,shell)){
		fprintf(*wfp,"%s%s%s\n",buf,filename,"&end&");
		return;
	}
	fprintf(*wfp,"%s%s\n",buf,filename);


	/* analyze child file */
	if(!(rfp = fopen(filename,"r"))){
		return;
	}
	while(fgets(buf,BUFLEN,rfp)){
		size = strlen(buf)-1;
		if(size>=0)
			buf[size] = '\0';
		else
			buf[0] = '\0';
		
		p = buf;
		while(*p){
			if(!strncmp(p,EXEC,strlen(EXEC))){
				p += strlen(EXEC);
				while(*p==' ') p++;
				q = p;
				while(*q!=' ' && *q!='0') q++;
				*q = '\0';
				analyze(wfp,p,depth+1,shell);
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

	/* mount /sys */
	if(lstat("/sys/kernel/security",&st))
		mount("/sys","/sys","sysfs",0,NULL);
	/* mount /sys/kernel/security */
	if(lstat("/sys/kernel/security/bobulsm",&st))
		mount("none","/sys/kernel/security","securityfs",0,NULL);
		

	/* make CONF_DIR directory*/
	if(stat(CONF_DIR,&st)){
		if(mkdir(CONF_DIR,0755)){
			fprintf(stderr,"bobulsm_user: Can't make directory \"%s\".\n",
					CONF_DIR);
			exit(1);
		}
	}

        /* make CONF_FILE */
        if(stat(CONF_FILE,&st)){
                if(wfp = fopen(CONF_FILE,"w")){
                        fclose(wfp);
                }else{
			fprintf(stderr,"bobulsm_user: Failure to make \"%s\".\n",CONF_FILE);
			exit(1);
                }
        }


	/* analyze program file */
	if(argc == 3){
		if(wfp = fopen(CONF_FILE,"w")){
                        fprintf(wfp,"1\n");
			fprintf(wfp,"*%s\n",SSHD);
			fprintf(wfp,"**%s\n",SSHD);
			fprintf(wfp,"***%s\n",argv[2]);  /* login shell  */
                        fprintf(wfp,"****%s\n", "/usr/bin/id");
                        fprintf(wfp,"****%s\n", "/usr/bin/dircolors");
                        fprintf(wfp,"****%s\n", "/usr/bin/clear_console");
                        fprintf(wfp,"****%s\n", "/usr/lib/openssh/sftp-server");
			analyze(&wfp,argv[1],4,argv[2]);
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
