// myinfo : affiche des informations système utiles (via un parsing de /proc) sur 3 lignes :
// <hostname> <kernel-version> <processor-architecture>
// <time> up <nb> days, <hh>:<mm>
// Load : <1-min> - <5-min> - <15-min>
 
// Exemple :
// tncy 6.1.0 x86_64
// 14:56:52 up 10 days, 22:49
// Load : 0.32 - 0.20 - 0.21

//hostname -> /proc/sys/kernel/hostname

// <processor-architecture> -> /proc/sys/kernel/arch
//kernel-version -> /proc/sys/kernel/osrelease

//up time -> /proc/uptime

#include "cmd.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define BUFF 256
#define N 4

int myinfo(void)
{
    /* files to read: hostname, kernel version, arch, uptime */
    const char *f_add[N] = {
        "/proc/sys/kernel/hostname",
        "/proc/sys/kernel/osrelease",
        "/proc/sys/kernel/arch",
        "/proc/uptime"
    };

    char var[N][BUFF];

    for (int i = 0; i < N; i++) {
        FILE *fd = fopen(f_add[i], "r");
        if (fd == NULL) {
            perror(f_add[i]);
            return -1;
        }
        if (fgets(var[i], BUFF, fd) == NULL) {
            perror("fgets");
            fclose(fd);
            return -1;
        }
        fclose(fd);
        /* strip newline */
        var[i][strcspn(var[i], "\r\n")] = '\0';
    }

    /* Line 1: hostname kernel-version arch */
    printf("%s %s %s\n", var[0], var[1], var[2]);

    /* Line 2: current time and uptime */
    time_t rawtime;
    struct tm *timeinfo;
    char timestr[64];

    time(&rawtime);
    timeinfo = localtime(&rawtime);
    if (timeinfo == NULL) {
        perror("localtime");
        return -1;
    }
    if (strftime(timestr, sizeof(timestr), "%T", timeinfo) == 0) {
        strcpy(timestr, "??:??:??");
    }

    /* uptime is first token of /proc/uptime and is a floating number (seconds) */
    var[3][strcspn(var[3], " ")] = '\0';
    double uptime_seconds = atof(var[3]);
    int seconds_total = (int)uptime_seconds;
    int days = seconds_total / (60 * 60 * 24);
    int hours = (seconds_total % (60 * 60 * 24)) / (60 * 60);
    int minutes = (seconds_total % (60 * 60)) / 60;

    printf("%s, up %d days, %02d:%02d\n", timestr, days, hours, minutes);

    /* Line 3: load averages from /proc/loadavg (first three fields) */
    {
        FILE *lf = fopen("/proc/loadavg", "r");
        if (lf != NULL) {
            char loadbuf[BUFF];
            if (fgets(loadbuf, BUFF, lf) != NULL) {
                double l1 = 0.0, l5 = 0.0, l15 = 0.0;
                int pos=0, end=0, len=strlen(loadbuf);
                for(int i=0;pos<len && i<3;i++){
                    char numbuf[32];
                    //searches for the next space " "
                    end=strcspn(&(loadbuf[pos]), " ");
                    //copies the load average value
                    strncpy(numbuf, &(loadbuf[pos]), end);
                    //marks end of string
                    loadbuf[end]='\0';
                    //updates position
                    pos+=end+1;
                    //transforms string to float
                    if(i==0) l1=atof(numbuf);
                    else if(i==1) l5=atof(numbuf);
                    else if(i==2) l15=atof(numbuf);
                }
                printf("Load : %.2f - %.2f - %.2f\n", l1, l5, l15);
            }
            fclose(lf);
        } else {
            perror("/proc/loadavg");
        }
        
    }

    return 0;
}


