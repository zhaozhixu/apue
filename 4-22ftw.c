#include "apue.h"
#include <dirent.h>
#include <limits.h>

/* function type that is called for each filename */
typedef int Myfunc(const char *, const struct stat *, int);

static Myfunc myfunc;
static int myftw(char *, Myfunc *);
static int dopath(Myfunc *);
static long nreg, ndir, nblk, nchr, nfifo, nslink, nsock, ntot;

int
main(int argc, char *argv[])
{
     int ret;
     if (argc != 2)
          err_quit("usage: ftw <starting-pathname>");
     ret = myftw(argv[1], myfunc); /* does it all */
     ntot = nreg + ndir + nblk + nchr + nfifo + nslink + nsock;
     if (ntot == 0)
          ntot = 1;             /* avoid divide by 0; print 0 for all counts */
     printf("regular files = %7ld, %5.2f %%\n", nreg, nreg*100.0/ntot);
     printf("directories = %7ld, %5.2f %%\n", ndir, ndir*100.0/ntot);
     printf("block special = %7ld, %5.2f %%\n", nblk, nblk*100.0/ntot);
     printf("char special = %7ld, %5.2f %%\n", nchr, nchr*100.0/ntot);
     printf("FIFOs = %7ld, %5.2f %%\n", nfifo, nfifo*100.0/ntot);
     printf("symbolic links = %7ld, %5.2f %%\n", nslink, nslink*100.0/ntot);
     printf("sockets = %7ld, %5.2f %%\n", nsock, nsock*100.0/ntot);
     exit(ret);
}

/*
 * Desend through the hierarchy, starting at "pathname".
 * The caller's func() is called for every file
 */

#define FTW_F 1                 /* file other than directory */
#define FTW_D 2                 /* directory */
#define FTW_DNR 3               /* directory that can't be read */
#define FTW_NS 4                /* file that we can's stat */

static char *fullpath;          /* contains full pathname for every file */
static size_t pathlen;

static int                      /* we return whatever func() returns */
myftw(char *pathname, Myfuc *func)
{
     fullpath = path_alloc(&pathlen); /* malloc PATH_MAX+1 bytes */
     if (pathlen <= strlen(pathname)) {
          pathlen = strlen(pathname) * 2;
          if ((fullpath = realloc(fullpath, pathlen)) == null)
               err_sys("realloc failed");
     }
     strcpy(fullpath, pathname);
     return(dopath(func));
}

/*
 * Descend through the hierarchy, starting at "fullpath",
 * if "fullpath" is anything other than a directory, we lstat() it,
 * call func(), and return. For a directory, we call ourself
 * recursively for each time int the directory.
 */

static int                      /* we return whatever func() returns */
dopath(Myfunc *func)
{
     struct stat statbuf;
     struct dirent *dirp;
     DIR *dp;
     int ret, n;
     if (lstat(fullpath, &statbuf) < 0) /* stat error */
          return(func(fullpath, &statbuf, FTW_NS));
     if (S_ISDIR(statbuf.st_mode) == 0) /* not a directory */
          return(func(fullpath, &statbuf, FTW_F));
     /*
      * It's a directory. First call func() for the directory,
      * then process each filename int the directory.
      */
     if ((ret = func(fullpath, &statbuf, FTW_D)) != 0)
          return(ret);
     n = strlen(fullpath);
     if (n + NAME_MAX + 2 > pathlen) { /* expand path buffer */
          pathlen *= 2;
          if ((fullpath = realloc(fullpath, pathlen)) == NULL)
               err_sys("realloc failed");
     }

}
