/// ///////////////////////////////////////////////////////////////////////////////////////////////
/// HEADIR Large Scale Retrieval System
///
/// Sockets, TCP Connections, Threads, I/O buffering
///
/// Leonidas Akritidis, November 7th, 2011
/// ///////////////////////////////////////////////////////////////////////////////////////////////

/// Linux with glibc:
/// _REENTRANT to grab thread-safe libraries
/// _POSIX_SOURCE to get POSIX semantics
#ifdef __linux__
#define _REENTRANT
//#define _POSIX_SOURCE
#endif

/// Hack for LinuxThreads
#ifdef __linux__
#define _P __P
#endif
#ifndef	__unp_h
#define	__unp_h

#include "SocketConfig.h"

/// If anything changes in the following list of #includes, must change acsite.m4 also, for
/// configure's tests.

#include <sys/types.h>	/// basic system data types
#include <sys/socket.h>	/// basic socket definitions
#include <sys/time.h>	/// timeval{} for select()
#include <time.h>		/// timespec{} for pselect()
#include <netinet/in.h>	/// sockaddr_in{} and other Internet defns
#include <arpa/inet.h>	/// inet(3) functions
#include <errno.h>
#include <fcntl.h>		/// for nonblocking
#include <netdb.h>
#include <signal.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>	/// for S_xxx file mode constants
#include <sys/uio.h>	/// for iovec{} and readv/writev
#include <unistd.h>
#include <sys/wait.h>
#include <sys/un.h>		/// for Unix domain sockets
#include <assert.h>
#include <ftw.h>
#include <math.h>

#ifdef HAVE_SYS_SELECT_H
#include <sys/select.h>	/// for convenience
#endif

#ifdef HAVE_SYS_SYSCTL_H
#include <sys/sysctl.h>
#endif

#ifdef HAVE_POLL_H
#include <poll.h>		/// for convenience
#endif

#ifdef HAVE_SYS_EVENT_H
#include <sys/event.h>	/// for kqueue
#endif

#ifdef HAVE_STRINGS_H
#include <strings.h>	/// for convenience
#endif

/// Three headers are normally needed for socket/file ioctl's: <sys/ioctl.h>, <sys/filio.h>, and
/// <sys/sockio.h>
#ifdef	HAVE_SYS_IOCTL_H
# include <sys/ioctl.h>
#endif
#ifdef	HAVE_SYS_FILIO_H
# include <sys/filio.h>
#endif
#ifdef	HAVE_SYS_SOCKIO_H
#include <sys/sockio.h>
#endif

//#ifdef HAVE_PTHREAD_H
#include <pthread.h>
//#endif

#ifdef HAVE_NET_IF_DL_H
#include <net/if_dl.h>
#endif

#ifdef HAVE_NETINET_SCTP_H
#include	<netinet/sctp.h>
#endif

/// OSF/1 actually disables recv() and send() in <sys/socket.h>
#ifdef	__osf__
#undef	recv
#undef	send
#define	recv(a,b,c,d)	recvfrom(a,b,c,d,0,0)
#define	send(a,b,c,d)	sendto(a,b,c,d,0,0)
#endif

#ifndef	INADDR_NONE
#define	INADDR_NONE	0xffffffff	/// should have been in <netinet/in.h>
#endif

#ifndef	SHUT_RD					/// these three POSIX names are new
#define	SHUT_RD		0			/// shutdown for reading
#define	SHUT_WR		1			/// shutdown for writing
#define	SHUT_RDWR	2			/// shutdown for reading and writing
#endif

#ifndef INET_ADDRSTRLEN
#define	INET_ADDRSTRLEN		16	/// "ddd.ddd.ddd.ddd\0" 1234567890123456
#endif

/// Define following even if IPv6 not supported, so we can always allocate an adequately sized
/// buffer without #ifdefs in the code.
#ifndef INET6_ADDRSTRLEN
#define	INET6_ADDRSTRLEN	46	/** max size of IPv6 address string:
					"xxxx:xxxx:xxxx:xxxx:xxxx:xxxx:xxxx:xxxx" or
					"xxxx:xxxx:xxxx:xxxx:xxxx:xxxx:ddd.ddd.ddd.ddd\0"
					1234567890123456789012345678901234567890123456 */
#endif

/// Define bzero() as a macro if it's not in standard C library.
#ifndef	HAVE_BZERO
#define	bzero(ptr,n)		memset(ptr, 0, n)
#endif

/// Older resolvers do not have gethostbyname2()
#ifndef	HAVE_GETHOSTBYNAME2
#define	gethostbyname2(host,family)		gethostbyname((host))
#endif

/// The structure returned by recvfrom_flags()
struct unp_in_pktinfo {
	struct in_addr	ipi_addr;	/// dst IPv4 address
	int ipi_ifindex;			/// received interface index
};

/// We need the newer CMSG_LEN() and CMSG_SPACE() macros but few implementations support them today
/// These two macros really need an ALIGN() macro, but each implementation does this differently.
#ifndef	CMSG_LEN
#define	CMSG_LEN(size)		(sizeof(struct cmsghdr) + (size))
#endif
#ifndef	CMSG_SPACE
#define	CMSG_SPACE(size)	(sizeof(struct cmsghdr) + (size))
#endif

/// POSIX requires the SUN_LEN() macro, but not all implementations DefinE it (yet). Note that this
/// 4.4BSD macro works regardless whether there is a length field or not.
#ifndef	SUN_LEN
// $$.Im SUN_LEN$$
# define	SUN_LEN(su) \
	(sizeof(*(su)) - sizeof((su)->sun_path) + strlen((su)->sun_path))
#endif

/// POSIX renames "Unix domain" as "local IPC." Not all systems DefinE AF_LOCAL and PF_LOCAL (yet).
#ifndef	AF_LOCAL
#define AF_LOCAL	AF_UNIX
#endif
#ifndef	PF_LOCAL
#define PF_LOCAL	PF_UNIX
#endif

/// POSIX requires that an #include of <poll.h> DefinE INFTIM, but many systems still DefinE it in
/// <sys/stropts.h>. We don't want to include all the STREAMS stuff if it's not needed, so we just
/// DefinE INFTIM here. This is the standard value, but there's no guarantee it is -1.
#ifndef INFTIM
#define INFTIM          (-1)    /// infinite poll timeout
#ifdef	HAVE_POLL_H
#define	INFTIM_UNPH				/// tell unpxti.h we defined it
#endif
#endif

/// Following could be derived from SOMAXCONN in <sys/socket.h>, but many kernels still #define it
/// as 5, while actually supporting many more
#define	LISTENQ		1024	    /// 2nd argument to listen()

/// Miscellaneous constants
#define	MAXLINE		1000000	    /// max text line length
#define	BUFFSIZE	8192	    /// buffer size for reads and writes

/// Define some port number that can be used for our examples
#define	SERV_PORT		 9877			/// TCP and UDP
#define	SERV_PORT_STR	"9877"			/// TCP and UDP
#define	UNIXSTR_PATH	"/tmp/unix.str"	/// Unix domain stream
#define	UNIXDG_PATH		"/tmp/unix.dg"	/// Unix domain datagram

/// Following shortens all the typecasts of pointer arguments:
#define	SA	struct sockaddr

#define HAVE_STRUCT_SOCKADDR_STORAGE
#ifndef HAVE_STRUCT_SOCKADDR_STORAGE

/// RFC 3493: protocol-independent placeholder for socket addresses
#define	__SS_MAXSIZE	128
#define	__SS_ALIGNSIZE	(sizeof(int64_t))
#ifdef HAVE_SOCKADDR_SA_LEN
#define	__SS_PAD1SIZE	(__SS_ALIGNSIZE - sizeof(u_char) - sizeof(sa_family_t))
#else
#define	__SS_PAD1SIZE	(__SS_ALIGNSIZE - sizeof(sa_family_t))
#endif
#define	__SS_PAD2SIZE	(__SS_MAXSIZE - 2*__SS_ALIGNSIZE)

struct sockaddr_storage {
#ifdef HAVE_SOCKADDR_SA_LEN
	u_char		ss_len;
#endif
	sa_family_t	ss_family;
	char		__ss_pad1[__SS_PAD1SIZE];
	int64_t		__ss_align;
	char		__ss_pad2[__SS_PAD2SIZE];
};
#endif

/// default file access permissions for new files
#define	FILE_MODE	(S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)

/// default permissions for new directories
#define	DIR_MODE	(FILE_MODE | S_IXUSR | S_IXGRP | S_IXOTH)

/// for signal handlers
typedef	void	Sigfunc(int);


#define HAVE_ADDRINFO_STRUCT
#ifndef	HAVE_ADDRINFO_STRUCT
# include	"lib/addrinfo.h"
#endif

#ifndef	HAVE_IF_NAMEINDEX_STRUCT
struct if_nameindex {
	uint32_t if_index;  /// 1, 2, ...
	char *if_name;   /// null-terminated name: "le0", ...
};
#endif

#define HAVE_TIMESPEC_STRUCT
#ifndef	HAVE_TIMESPEC_STRUCT
struct timespec {
	time_t tv_sec;  /// seconds
	long tv_nsec;	/// and nanoseconds
};
#endif


uint16_t in_cksum(uint16_t *, int);

#include <stdarg.h>
#include <syslog.h>

int	daemon_proc;		// set nonzero by daemon_init()

static void	err_doit(int, int, const char *, va_list);

// Nonfatal error related to system call. Print message and return.
void err_ret(const char *fmt, ...) {
	va_list ap;

	va_start(ap, fmt);
	err_doit(1, LOG_INFO, fmt, ap);
	va_end(ap);
	return;
}

// Fatal error related to system call. Print message and terminate.
void err_sys(const char *fmt, ...) {
	va_list ap;

	va_start(ap, fmt);
	err_doit(1, LOG_ERR, fmt, ap);
	va_end(ap);
	exit(1);
}

// Fatal error related to system call. Print message, dump core, and terminate.
void err_dump(const char *fmt, ...) {
	va_list ap;

	va_start(ap, fmt);
	err_doit(1, LOG_ERR, fmt, ap);
	va_end(ap);
	abort();
	exit(1);
}

// Nonfatal error unrelated to system call. Print message and return.
void err_msg(const char *fmt, ...) {
	va_list ap;

	va_start(ap, fmt);
	err_doit(0, LOG_INFO, fmt, ap);
	va_end(ap);
	return;
}

// Fatal error unrelated to system call. Print message and terminate.
void err_quit(const char *fmt, ...) {
	va_list ap;

	va_start(ap, fmt);
	err_doit(0, LOG_ERR, fmt, ap);
	va_end(ap);
	exit(1);
}

// Print message and return to caller. Caller specifies "errnoflag" and "level".
static void err_doit(int32_t errnoflag, int32_t level, const char *fmt, va_list ap) {
	int32_t errno_save, n;
	char buf[MAXLINE + 1];

	errno_save = errno;		/* value caller might want printed */

	#ifdef HAVE_VSNPRINTF
		vsnprintf(buf, MAXLINE, fmt, ap);	/* safe */
	#else
		vsprintf(buf, fmt, ap);					/* not safe */
	#endif
	n = strlen(buf);
	if (errnoflag) {
		snprintf(buf + n, MAXLINE - n, ": %s", strerror(errno_save));
	}
	strcat(buf, "\n");

	if (daemon_proc) {
		syslog(level, "%s\n", buf);
	} else {
		fflush(stdout);		/* in case stdout and stderr are the same */
		fputs(buf, stderr);
		fflush(stderr);
	}
	return;
}

ssize_t writen(int32_t fd, const void *vptr, size_t n) {
	size_t nleft;
	ssize_t nwritten;
	const char *ptr;

	ptr = (const char*)vptr;

	nleft = n;
	while (nleft > 0) {
		if ( (nwritten = write(fd, ptr, nleft)) <= 0) {
			if (nwritten < 0 && errno == EINTR) {
				nwritten = 0;
			} else {
				return(-1);
			}
		}

		nleft -= nwritten;
		ptr   += nwritten;
	}
	return(n);
}

void Writen(int32_t fd, void *ptr, ssize_t nbytes) {
	if (writen(fd, ptr, nbytes) != nbytes) {
		err_sys("writen error");
	}
}

void *Malloc(size_t size) {
	void *ptr;

	if ((ptr = malloc(size)) == NULL) {
		err_sys("malloc error");
	}
	return(ptr);
}

void *Calloc(size_t n, size_t size) {
	void *ptr;
	if ((ptr = calloc(n, size)) == NULL) {
		err_sys("calloc error");
	}
	return(ptr);
}

void Close(int32_t fd) {
	if (close(fd) == -1) {
		err_sys("close error");
	}
}

int32_t tcp_listen(const char *host, const char *serv, socklen_t *addrlenp) {
	int	listenfd, n;
	const int32_t on = 1;
	struct addrinfo	hints, *res, *ressave;

	bzero(&hints, sizeof(struct addrinfo));
	hints.ai_flags = AI_PASSIVE;
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	if ((n = getaddrinfo(host, serv, &hints, &res)) != 0) {
		err_quit("tcp_listen error for %s, %s: %s", host, serv, gai_strerror(n));
	}
	ressave = res;

	do {
		listenfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
		if (listenfd < 0) { continue; } // error, try next one

		setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
		if (bind(listenfd, res->ai_addr, res->ai_addrlen) == 0) {
			break; // success
		}

		close(listenfd);	// bind error, close and try next one
	} while ( (res = res->ai_next) != NULL);

	if (res == NULL) { // errno from final socket() or bind()
		err_sys("tcp_listen error for %s, %s", host, serv);
	}

	listen(listenfd, LISTENQ);

	if (addrlenp) {
		*addrlenp = res->ai_addrlen; // return size of protocol address.
	}

	freeaddrinfo(ressave);

	return(listenfd);
}

int32_t Tcp_listen(const char *host, const char *serv, socklen_t *addrlenp) {
	return(tcp_listen(host, serv, addrlenp));
}

/// THREADS WRAPPER FUNCTIONS

/// Global vars: Mutual Exclusions and Conditional Variables
pthread_mutex_t NDONE_MUTEX = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t NDONE_MUTEX_2 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t PROCESS_MUTEX = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t NDONE_COND = PTHREAD_COND_INITIALIZER;
pthread_cond_t NDONE_COND_2 = PTHREAD_COND_INITIALIZER;
pthread_mutex_t	CRAWLER_MLOCK_FILE = PTHREAD_MUTEX_INITIALIZER;
int32_t NDONE;

void Pthread_create(pthread_t *tid, const pthread_attr_t *attr,
	void * (*func)(void *), void *arg) {
		int	n;

		if ((n = pthread_create(tid, attr, func, arg)) == 0) {
			return;
		}
		errno = n;
		err_sys("pthread_create error");
}

void Pthread_join(pthread_t tid, void **status) {
	int32_t n;

	if ((n = pthread_join(tid, status)) == 0) {
		return;
	}
	errno = n;
	err_sys("pthread_join error");
}

void Pthread_detach(pthread_t tid) {
	int32_t n;

	if ((n = pthread_detach(tid)) == 0) {
		return;
	}
	errno = n;
	err_sys("pthread_detach error");
}

void Pthread_kill(pthread_t tid, int32_t signo) {
	int32_t n;

	if ((n = pthread_kill(tid, signo)) == 0) {
		return;
	}
	errno = n;
	err_sys("pthread_kill error");
}

void Pthread_mutexattr_init(pthread_mutexattr_t *attr) {
	int32_t n;

	if ((n = pthread_mutexattr_init(attr)) == 0) {
		return;
	}
	errno = n;
	err_sys("pthread_mutexattr_init error");
}

#ifdef	_POSIX_THREAD_PROCESS_SHARED
void Pthread_mutexattr_setpshared(pthread_mutexattr_t *attr, int32_t flag) {
	int	n;

	if ((n = pthread_mutexattr_setpshared(attr, flag)) == 0) {
		return;
	}
	errno = n;
	err_sys("pthread_mutexattr_setpshared error");
}
#endif

void Pthread_mutex_init(pthread_mutex_t *mptr, pthread_mutexattr_t *attr) {
	int32_t n;

	if ((n = pthread_mutex_init(mptr, attr)) == 0) {
		return;
	}
	errno = n;
	err_sys("pthread_mutex_init error");
}

// include Pthread_mutex_lock.
void Pthread_mutex_lock(pthread_mutex_t *mptr) {
	int32_t n;

	if ((n = pthread_mutex_lock(mptr)) == 0) {
		return;
	}
	errno = n;
	err_sys("pthread_mutex_lock error");
}

void Pthread_mutex_unlock(pthread_mutex_t *mptr) {
	int32_t n;

	if ((n = pthread_mutex_unlock(mptr)) == 0) {
		return;
	}
	errno = n;
	err_sys("pthread_mutex_unlock error");
}

void Pthread_cond_broadcast(pthread_cond_t *cptr) {
	int32_t n;

	if ((n = pthread_cond_broadcast(cptr)) == 0) {
		return;
	}
	errno = n;
	err_sys("pthread_cond_broadcast error");
}

void Pthread_cond_signal(pthread_cond_t *cptr) {
	int32_t n;

	if ((n = pthread_cond_signal(cptr)) == 0) {
		return;
	}
	errno = n;
	err_sys("pthread_cond_signal error");
}

void Pthread_cond_wait(pthread_cond_t *cptr, pthread_mutex_t *mptr) {
	int32_t n;

	if ((n = pthread_cond_wait(cptr, mptr)) == 0) {
		return;
	}
	errno = n;
	err_sys("pthread_cond_wait error");
}

void Pthread_cond_timedwait(pthread_cond_t *cptr, pthread_mutex_t *mptr,
	const struct timespec *tsptr) {

		int32_t n;

		if ((n = pthread_cond_timedwait(cptr, mptr, tsptr)) == 0) {
			return;
		}
		errno = n;
		err_sys("pthread_cond_timedwait error");
}

void Pthread_once(pthread_once_t *ptr, void (*func)(void)) {
	int32_t n;

	if ((n = pthread_once(ptr, func)) == 0) {
		return;
	}
	errno = n;
	err_sys("pthread_once error");
}

void Pthread_key_create(pthread_key_t *key, void (*func)(void *)) {
	int32_t n;

	if ((n = pthread_key_create(key, func)) == 0) {
		return;
	}
	errno = n;
	err_sys("pthread_key_create error");
}

void Pthread_setspecific(pthread_key_t key, const void *value) {
	int32_t n;

	if ((n = pthread_setspecific(key, value)) == 0) {
		return;
	}
	errno = n;
	err_sys("pthread_setspecific error");
}

int32_t Accept(int32_t fd, struct sockaddr *sa, socklen_t *salenptr) {
	int32_t n;

	again:
		if ((n = accept(fd, sa, salenptr)) < 0) {
			#ifdef	EPROTO
				if (errno == EPROTO || errno == ECONNABORTED)
			#else
				if (errno == ECONNABORTED)
			#endif
					goto again;
				else
					err_sys("accept error");
		}
		return(n);
}


void Bind(int32_t fd, const struct sockaddr *sa, socklen_t salen) {
	if (bind(fd, sa, salen) < 0) {
		err_sys("bind error");
	}
}

void Connect(int32_t fd, const struct sockaddr *sa, socklen_t salen) {
	if (connect(fd, sa, salen) < 0) {
		err_sys("connect error");
	}
}

void Getpeername(int32_t fd, struct sockaddr *sa, socklen_t *salenptr) {
	if (getpeername(fd, sa, salenptr) < 0) {
		err_sys("getpeername error");
	}
}

void Getsockname(int32_t fd, struct sockaddr *sa, socklen_t *salenptr) {
	if (getsockname(fd, sa, salenptr) < 0) {
		err_sys("getsockname error");
	}
}

void Getsockopt(int32_t fd, int32_t level, int32_t optname, void *optval, socklen_t *optlenptr) {
	if (getsockopt(fd, level, optname, optval, optlenptr) < 0) {
		err_sys("getsockopt error");
	}
}

Sigfunc *signal(int32_t signo, Sigfunc *func) {
	struct sigaction act, oact;

	act.sa_handler = func;
	sigemptyset(&act.sa_mask);
	act.sa_flags = 0;
	if (signo == SIGALRM) {
		#ifdef SA_INTERRUPT
			act.sa_flags |= SA_INTERRUPT;	/* SunOS 4.x */
		#endif
			} else {
		#ifdef SA_RESTART
			act.sa_flags |= SA_RESTART;		/* SVR4, 44BSD */
		#endif
	}
	if (sigaction(signo, &act, &oact) < 0) {
		return(SIG_ERR);
	}
	return(oact.sa_handler);
}

Sigfunc *Signal(int32_t signo, Sigfunc *func) {
	Sigfunc	*sigfunc;

	if ((sigfunc = signal(signo, func)) == SIG_ERR) {
		err_sys("signal error");
	}
	return(sigfunc);
}

/*
void pr_cpu_time(void) {
	double user, sys;
	struct rusage myusage, childusage;

	if (getrusage(RUSAGE_SELF, &myusage) < 0) {
		err_sys("getrusage error");
	}
	if (getrusage(RUSAGE_CHILDREN, &childusage) < 0) {
		err_sys("getrusage error");
	}

	user = (double) myusage.ru_utime.tv_sec + myusage.ru_utime.tv_usec/1000000.0;
	user += (double) childusage.ru_utime.tv_sec + childusage.ru_utime.tv_usec/1000000.0;
	sys = (double) myusage.ru_stime.tv_sec + myusage.ru_stime.tv_usec/1000000.0;
	sys += (double) childusage.ru_stime.tv_sec + childusage.ru_stime.tv_usec/1000000.0;

	printf("\nuser time = %g, sys time = %g\n", user, sys);
}
*/
void Write(int32_t fd, void *ptr, ssize_t nbytes) {
	if (write(fd, ptr, nbytes) != nbytes) {
		err_sys("write error");
	}
}

int32_t tcp_connect(const char *host, const char *serv) {
	int32_t sockfd, n;
	struct addrinfo	hints, *res, *ressave;

	bzero(&hints, sizeof(struct addrinfo));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	if ((n = getaddrinfo(host, serv, &hints, &res)) != 0) {
		printf("\nTCP connection error for %s, %s: %s\n", host, serv, gai_strerror(n));
		return -1;
	}
	ressave = res;

	do {
		sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
		if (sockfd < 0) {
			continue;
		}

		if (connect(sockfd, res->ai_addr, res->ai_addrlen) == 0) {
			break;
		}

		Close(sockfd);
	} while ( (res = res->ai_next) != NULL);

	if (res == NULL) {
		err_sys("tcp_connect error for %s, %s", host, serv);
	}

	freeaddrinfo(ressave);

	return(sockfd);
}

int32_t Tcp_connect(const char *host, const char *serv) {
	return(tcp_connect(host, serv));
}

int32_t Socket(int32_t family, int32_t type, int32_t protocol) {
	int32_t n;

	if ( (n = socket(family, type, protocol)) < 0) {
		err_sys("socket error");
	}
	return(n);
}

char *Fgets(char *ptr, int32_t n, FILE *stream) {
	char *rptr;

	if ((rptr = fgets(ptr, n, stream)) == NULL && ferror(stream)) {
		err_sys("fgets error");
	}

	return (rptr);
}

void Fputs(const char *ptr, FILE *stream) {
	if (fputs(ptr, stream) == EOF) {
		err_sys("fputs error");
	}
}

void Shutdown(int32_t fd, int32_t how) {
	if (shutdown(fd, how) < 0) {
		err_sys("shutdown error");
	}
}

static pthread_key_t rl_key;
static pthread_once_t rl_once = PTHREAD_ONCE_INIT;

static void readline_destructor(void *ptr) {
	free(ptr);
}

static void readline_once(void) {
	Pthread_key_create(&rl_key, readline_destructor);
}

typedef struct {
	int32_t rl_cnt;
	char *rl_bufptr;
	char rl_buf[MAXLINE];
} Rline;

static ssize_t my_read(Rline *tsd, int32_t fd, char *ptr) {

	if (tsd->rl_cnt <= 0) {
		again:

		if ((tsd->rl_cnt = read(fd, tsd->rl_buf, MAXLINE)) < 0) {
			if (errno == EINTR) {
				goto again;
			}
			return(-1);
		} else if (tsd->rl_cnt == 0) {
			return(0);
		}
		tsd->rl_bufptr = tsd->rl_buf;
	}
	tsd->rl_cnt--;
	*ptr = *tsd->rl_bufptr++;

	return(1);
}

ssize_t readline(int32_t fd, void *vptr, size_t maxlen) {
	size_t n, rc;
	char c, *ptr;
	Rline *tsd;

	Pthread_once(&rl_once, readline_once);
	if ((tsd = (Rline*)pthread_getspecific(rl_key)) == NULL) {
		tsd = (Rline*)Calloc(1, sizeof(Rline));
		Pthread_setspecific(rl_key, tsd);
	}

	ptr = (char*)vptr;
	for (n = 1; n < maxlen; n++) {
		if ((rc = my_read(tsd, fd, &c)) == 1) {
			*ptr++ = c;
			if (c == '\n') {
				break;
			}
		} else if (rc == 0) {
			*ptr = 0;
			return(n - 1);
		} else {
			return(-1);
		}
	}

	*ptr = 0;
	return(n);
}

ssize_t Readline(int32_t fd, void *ptr, size_t maxlen) {
	ssize_t n;

	if ((n = readline(fd, ptr, maxlen)) < 0) {
		err_sys("readline error");
	}
	return(n);
}

/*
void sig_int(int32_t signo) {
	void pr_cpu_time(void);
	pr_cpu_time();
	exit(0);
}
*/
int32_t unlink_cb(const char *fpath, const struct stat *sb, int32_t typeflag, struct FTW *ftwbuf) {
	int32_t rv = remove(fpath);

	if (rv)
		perror(fpath);

	return rv;
}

int32_t rmrf(char *path) {
	return nftw(path, unlink_cb, 64, FTW_DEPTH | FTW_PHYS);
}

#endif
