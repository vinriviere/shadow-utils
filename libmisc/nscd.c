/* club nscd so it invalidates its cache */
/* notting@redhat.com 19990408           */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>


int hup_nscd() {
    FILE *pidfile;
    char *pidbuf=calloc(256,sizeof(char));
    int pid;
    
    if ((pidfile=fopen("/var/run/nscd.pid","r"))!=NULL) {
	pidbuf=fgets(pidbuf,256,pidfile);
	fclose(pidfile);
	if (pidbuf!=NULL) {
	    pid=atoi(pidbuf);
	    free(pidbuf);
	    if (pid>0) {
		return kill(pid,SIGHUP);
	    }
	}
    }
    return -1;
}
