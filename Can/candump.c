#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <ctype.h>
#include <libgen.h>
#include <time.h>
#include <errno.h>

#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/uio.h>
#include <net/if.h>

#include <linux/can.h>
#include <linux/can/raw.h>

#include "candump.h"
#include "terminal.h"
#include "lib.h"

/* for hardware timestamps - since Linux 2.6.30 */
#ifndef SO_TIMESTAMPING
#define SO_TIMESTAMPING 37
#endif

/* from #include <linux/net_tstamp.h> - since Linux 2.6.30 */
#define SOF_TIMESTAMPING_SOFTWARE (1<<4)
#define SOF_TIMESTAMPING_RX_SOFTWARE (1<<3)
#define SOF_TIMESTAMPING_RAW_HARDWARE (1<<6)

#define MAXSOCK 16    /* max. number of CAN interfaces given on the cmdline */ //即最大借口数为16 就是最多是can16
#define MAXIFNAMES 30 /* size of receive name index to omit ioctls */
#define MAXCOL 6      /* number of different colors for colorized output */
#define ANYDEV "any"  /* name of interface to receive from any CAN interface */ //接收如何canbus的数据
#define ANL "\r\n"    /* newline in ASC mode */                                 //回车换行

#define SILENT_INI 42 /* detect user setting on commandline */
#define SILENT_OFF 0  /* no silent mode */
#define SILENT_ANI 1  /* silent mode with animation */
#define SILENT_ON  2  /* silent mode (completely silent) */

#define BOLD    ATTBOLD
#define RED     ATTBOLD FGRED
#define GREEN   ATTBOLD FGGREEN
#define YELLOW  ATTBOLD FGYELLOW
#define BLUE    ATTBOLD FGBLUE
#define MAGENTA ATTBOLD FGMAGENTA
#define CYAN    ATTBOLD FGCYAN

const char col_on [MAXCOL][19] = {BLUE, RED, GREEN, BOLD, MAGENTA, CYAN}; //各个模块的颜色Can总线的基本颜色: 橙色;Can-Low总是棕色; Can-High:驱动系统(黑色);舒适系统(绿色); 信息系统(紫色);
const char col_off [] = ATTRESET;

static char *cmdlinename[MAXSOCK];
static __u32 dropcnt[MAXSOCK];
static __u32 last_dropcnt[MAXSOCK];
static char devname[MAXIFNAMES][IFNAMSIZ+1];
static int  dindex[MAXIFNAMES];
static int  max_devname_len; /* to prevent frazzled device name output */
const int canfd_on = 1;

#define MAXANI 4
const char anichar[MAXANI] = {'|', '/', '-', '\\'};
const char extra_m_info[4][4] = {"- -", "B -", "- E", "B E"};

extern int optind, opterr, optopt;

static volatile int running = 1;

void print_usage(char *prg)
{
	fprintf(stderr, "\nUsage: %s [options] <CAN interface>+\n", prg);
	fprintf(stderr, "  (use CTRL-C to terminate %s)\n\n", prg);
	fprintf(stderr, "Options: -t <type>   (timestamp: (a)bsolute/(d)elta/(z)ero/(A)bsolute w date)\n"); //设定时间
	fprintf(stderr, "         -H          (read hardware timestamps instead of system timestamps)\n");  //（读取硬件时间戳来代替系统的时间戳）\n”）；
	fprintf(stderr, "         -c          (increment color mode level)\n");                             //（增加颜色模式级别）“n”）；
	fprintf(stderr, "         -i          (binary output - may exceed 80 chars/line)\n");               //（二进制输出-可能超过80个字符/行）；
	fprintf(stderr, "         -a          (enable additional ASCII output)\n");                         //（启用额外的ASCII输出）“n”）；
	fprintf(stderr, "         -S          (swap byte order in printed CAN data[] - marked with '%c' )\n", SWAP_DELIMITER);//交换字节顺序打印数据[]标有“%c”）”，swap_delimiter）；
	fprintf(stderr, "         -s <level>  (silent mode - %d: off (default) %d: animation %d: silent)\n", SILENT_OFF, SILENT_ANI, SILENT_ON);//（无声模式：关闭（默认）%d %d %d：动画：无声）”，silent_off，silent_ani，silent_on）；
	fprintf(stderr, "         -b <can>    (bridge mode - send received frames to <can>)\n");            //桥接模式
	fprintf(stderr, "         -B <can>    (bridge mode - like '-b' with disabled loopback)\n");         //桥接模式 不使能回环
	fprintf(stderr, "         -u <usecs>  (delay bridge forwarding by <usecs> microseconds)\n");        //延时桥接模式
	fprintf(stderr, "         -l          (log CAN-frames into file. Sets '-s %d' by default)\n", SILENT_ON);//把canbus消息输入到文件中
	fprintf(stderr, "         -L          (use log file format on stdout)\n");                          //打印logfile
	fprintf(stderr, "         -n <count>  (terminate after receiption of <count> CAN frames)\n");       //接收多少个帧之后关闭
	fprintf(stderr, "         -r <size>   (set socket receive buffer to <size>)\n");                    //发送socket接收到的数据
	fprintf(stderr, "         -D          (Don't exit if a \"detected\" can device goes down.\n");      //检测到canbus 就不要退出
	fprintf(stderr, "         -d          (monitor dropped CAN frames)\n");                             //打印丢弃的canbus帧
	fprintf(stderr, "         -e          (dump CAN error frames in human-readable format)\n");         //打印canbus error帧
	fprintf(stderr, "         -x          (print extra message infos, rx/tx brs esi)\n");               //打印额外信息
	fprintf(stderr, "         -T <msecs>  (terminate after <msecs> without any reception)\n");          //多少秒没有回复后关闭 设备
	fprintf(stderr, "\n");
	fprintf(stderr, "Up to %d CAN interfaces with optional filter sets can be specified\n", MAXSOCK);
	fprintf(stderr, "on the commandline in the form: <ifname>[,filter]*\n");
	fprintf(stderr, "\nComma separated filters can be specified for each given CAN interface:\n");
	fprintf(stderr, " <can_id>:<can_mask> (matches when <received_can_id> & mask == can_id & mask)\n");
	fprintf(stderr, " <can_id>~<can_mask> (matches when <received_can_id> & mask != can_id & mask)\n");
	fprintf(stderr, " #<error_mask>       (set error frame filter, see include/linux/can/error.h)\n");
	fprintf(stderr, " [j|J]               (join the given CAN filters - logical AND semantic)\n");
	fprintf(stderr, "\nCAN IDs, masks and data content are given and expected in hexadecimal values.\n");
	fprintf(stderr, "When can_id and can_mask are both 8 digits, they are assumed to be 29 bit EFF.\n");
	fprintf(stderr, "Without any given filter all data frames are received ('0:0' default filter).\n");
	fprintf(stderr, "\nUse interface name '%s' to receive from all CAN interfaces.\n", ANYDEV);
	fprintf(stderr, "\nExamples:\n");
	fprintf(stderr, "%s -c -c -ta can0,123:7FF,400:700,#000000FF can2,400~7F0 can3 can8\n", prg);
	fprintf(stderr, "%s -l any,0~0,#FFFFFFFF    (log only error frames but no(!) data frames)\n", prg);
	fprintf(stderr, "%s -l any,0:0,#FFFFFFFF    (log error frames and also all data frames)\n", prg);
	fprintf(stderr, "%s vcan2,92345678:DFFFFFFF (match only for extended CAN ID 12345678)\n", prg);
	fprintf(stderr, "%s vcan2,123:7FF (matches CAN ID 123 - including EFF and RTR frames)\n", prg);
	fprintf(stderr, "%s vcan2,123:C00007FF (matches CAN ID 123 - only SFF and non-RTR frames)\n", prg);
	fprintf(stderr, "\n");
}

void sigterm(int signo)
{
	running = 0;
}

int idx2dindex(int ifidx, int socket) {

	int i;
	struct ifreq ifr;

	for (i=0; i < MAXIFNAMES; i++) {
		if (dindex[i] == ifidx)
			return i;
	}

	/* create new interface index cache entry */

	/* remove index cache zombies first */
	for (i=0; i < MAXIFNAMES; i++) {
		if (dindex[i]) {
			ifr.ifr_ifindex = dindex[i];
			if (ioctl(socket, SIOCGIFNAME, &ifr) < 0)
				dindex[i] = 0;
		}
	}

	for (i=0; i < MAXIFNAMES; i++)
		if (!dindex[i]) /* free entry */
			break;

	if (i == MAXIFNAMES) {
		fprintf(stderr, "Interface index cache only supports %d interfaces.\n",
		       MAXIFNAMES);
		exit(1);
	}

	dindex[i] = ifidx;

	ifr.ifr_ifindex = ifidx;
	if (ioctl(socket, SIOCGIFNAME, &ifr) < 0)
		perror("SIOCGIFNAME");

	if (max_devname_len < strlen(ifr.ifr_name))
		max_devname_len = strlen(ifr.ifr_name);

	strcpy(devname[i], ifr.ifr_name);

#ifdef DEBUG
	printf("new index %d (%s)\n", i, devname[i]);
#endif

	return i;
}

int canrecv(char **argv)
{
    int argc = 2;
    //#ifdef DEBUG
    printf("*** canrecv argv[0] is %s\n",argv[0]);
    printf("*** canrecv argv[1] is %s\n",argv[1]);
    //#endif // DEBUG

	fd_set rdfs;
	int s[MAXSOCK];
	int bridge = 0;
	useconds_t bridge_delay = 0;
	unsigned char timestamp = 0;
	unsigned char hwtimestamp = 0;
	unsigned char down_causes_exit = 1;
	unsigned char dropmonitor = 0;
	unsigned char extra_msg_info = 0;
	unsigned char silent = SILENT_INI;
	unsigned char silentani = 0;
	unsigned char color = 0;
	unsigned char view = 0;
	unsigned char log = 0;
	unsigned char logfrmt = 0;
	int count = 0;
	int rcvbuf_size = 0;
	int opt, ret;
	int currmax, numfilter;
	int join_filter;
	char *ptr, *nptr;
	struct sockaddr_can addr;
	char ctrlmsg[CMSG_SPACE(sizeof(struct timeval) + 3*sizeof(struct timespec) + sizeof(__u32))];
	struct iovec iov;
	struct msghdr msg;
	struct cmsghdr *cmsg;
	struct can_filter *rfilter;
	can_err_mask_t err_mask;
	struct canfd_frame frame;
	int nbytes, i, maxdlen;
	struct ifreq ifr;
	struct timeval tv, last_tv;
	struct timeval timeout, timeout_config = { 0, 0 }, *timeout_current = NULL;

/*START*/
	int nbytes_1;                                //socket套接字和发送字节数目
	struct can_frame frame_1;                       //接收数据的buffer
/*END*/
	FILE *logfile = NULL;

	signal(SIGTERM, sigterm);
	signal(SIGHUP, sigterm);
	signal(SIGINT, sigterm);

	last_tv.tv_sec  = 0;
	last_tv.tv_usec = 0;

	while ((opt = getopt(argc, argv, "t:HciaSs:b:B:u:lDdxLn:r:heT:?")) != -1) {//获取命令行 参数选项
		switch (opt) {
		case 't':
			timestamp = optarg[0];
			if ((timestamp != 'a') && (timestamp != 'A') &&
			    (timestamp != 'd') && (timestamp != 'z')) {
				fprintf(stderr, "%s: unknown timestamp mode '%c' - ignored\n",
				       basename(argv[0]), optarg[0]);
				timestamp = 0;
			}
			break;

		case 'H':
			hwtimestamp = 1;
			break;

		case 'c':
			color++;
			break;

		case 'i':
			view |= CANLIB_VIEW_BINARY;
			break;

		case 'a':
			view |= CANLIB_VIEW_ASCII;
			break;

		case 'S':
			view |= CANLIB_VIEW_SWAP;
			break;

		case 'e':
			view |= CANLIB_VIEW_ERROR;
			break;

		case 's':
			silent = atoi(optarg);
			if (silent > SILENT_ON) {
				print_usage(basename(argv[0]));
				exit(1);
			}
			break;

		case 'b':
		case 'B':
			if (strlen(optarg) >= IFNAMSIZ) {
				fprintf(stderr, "Name of CAN device '%s' is too long!\n\n", optarg);
				return 1;
			} else {
				bridge = socket(PF_CAN, SOCK_RAW, CAN_RAW);
				if (bridge < 0) {
					perror("bridge socket");
					return 1;
				}
				addr.can_family = AF_CAN;
				strcpy(ifr.ifr_name, optarg);
				if (ioctl(bridge, SIOCGIFINDEX, &ifr) < 0)
					perror("SIOCGIFINDEX");
				addr.can_ifindex = ifr.ifr_ifindex;

				if (!addr.can_ifindex) {
					perror("invalid bridge interface");
					return 1;
				}

				/* disable default receive filter on this write-only RAW socket */
				setsockopt(bridge, SOL_CAN_RAW, CAN_RAW_FILTER, NULL, 0);

				if (opt == 'B') {
					const int loopback = 0;

					setsockopt(bridge, SOL_CAN_RAW, CAN_RAW_LOOPBACK,
						   &loopback, sizeof(loopback));
				}

				if (bind(bridge, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
					perror("bridge bind");
					return 1;
				}
			}
			break;

		case 'u':
			bridge_delay = (useconds_t)strtoul(optarg, (char **)NULL, 10);
			break;

		case 'l':
			log = 1;
			break;

		case 'D':
			down_causes_exit = 0;
			break;

		case 'd':
			dropmonitor = 1;
			break;

		case 'x':
			extra_msg_info = 1;
			break;

		case 'L':
			logfrmt = 1;
			break;

		case 'n':
			count = atoi(optarg);
			if (count < 1) {
				print_usage(basename(argv[0]));
				exit(1);
			}
			break;

		case 'r':
			rcvbuf_size = atoi(optarg);
			if (rcvbuf_size < 1) {
				print_usage(basename(argv[0]));
				exit(1);
			}
			break;

		case 'T':
			errno = 0;
			timeout_config.tv_usec = strtol(optarg, NULL, 0);
			if (errno != 0) {
				print_usage(basename(argv[0]));
				exit(1);
			}
			timeout_config.tv_sec = timeout_config.tv_usec / 1000;
			timeout_config.tv_usec = (timeout_config.tv_usec % 1000) * 1000;
			timeout_current = &timeout;
			break;
		default:
			print_usage(basename(argv[0]));
			exit(1);
			break;
		}
	}

	if (optind == argc) {
		print_usage(basename(argv[0]));
		exit(0);
	}

	if (logfrmt && view) {
		fprintf(stderr, "Log file format selected: Please disable ASCII/BINARY/SWAP options!\n");
		exit(0);
	}

	if (silent == SILENT_INI) {
		if (log) {
			fprintf(stderr, "Disabled standard output while logging.\n");
			silent = SILENT_ON; /* disable output on stdout */
		} else
			silent = SILENT_OFF; /* default output */
	}

	currmax = argc - optind; /* find real number of CAN devices */

	if (currmax > MAXSOCK) {
		fprintf(stderr, "More than %d CAN devices given on commandline!\n", MAXSOCK);
		return 1;
	}
    //printf("****currmax is :%d \n",currmax);
	for (i=0; i < currmax; i++) {

		ptr = argv[optind+i];
		nptr = strchr(ptr, ',');

#ifdef DEBUG
		printf("open %d '%s'.\n", i, ptr);
#endif

		s[i] = socket(PF_CAN, SOCK_RAW, CAN_RAW);
		if (s[i] < 0) {
			perror("socket");
			return 1;
		}

		cmdlinename[i] = ptr; /* save pointer to cmdline name of this socket */

		if (nptr)
			nbytes = nptr - ptr;  /* interface name is up the first ',' */
		else
			nbytes = strlen(ptr); /* no ',' found => no filter definitions */

		if (nbytes >= IFNAMSIZ) {
			fprintf(stderr, "name of CAN device '%s' is too long!\n", ptr);
			return 1;
		}

		if (nbytes > max_devname_len)
			max_devname_len = nbytes; /* for nice printing */

		addr.can_family = AF_CAN;

		memset(&ifr.ifr_name, 0, sizeof(ifr.ifr_name));
		strncpy(ifr.ifr_name, ptr, nbytes);

#ifdef DEBUG
		printf("using interface name '%s'.\n", ifr.ifr_name);
#endif

		if (strcmp(ANYDEV, ifr.ifr_name)) {
			if (ioctl(s[i], SIOCGIFINDEX, &ifr) < 0) {
				perror("SIOCGIFINDEX");
				exit(1);
			}
			addr.can_ifindex = ifr.ifr_ifindex;
		} else
			addr.can_ifindex = 0; /* any can interface */

		if (nptr) {

			/* found a ',' after the interface name => check for filters */

			/* determine number of filters to alloc the filter space */
			numfilter = 0;
			ptr = nptr;
			while (ptr) {
				numfilter++;
				ptr++; /* hop behind the ',' */
				ptr = strchr(ptr, ','); /* exit condition */
			}

			rfilter = malloc(sizeof(struct can_filter) * numfilter);
			if (!rfilter) {
				fprintf(stderr, "Failed to create filter space!\n");
				return 1;
			}

			numfilter = 0;
			err_mask = 0;
			join_filter = 0;

			while (nptr) {

				ptr = nptr+1; /* hop behind the ',' */
				nptr = strchr(ptr, ','); /* update exit condition */

				if (sscanf(ptr, "%x:%x",
					   &rfilter[numfilter].can_id,
					   &rfilter[numfilter].can_mask) == 2) {
 					rfilter[numfilter].can_mask &= ~CAN_ERR_FLAG;
					numfilter++;
				} else if (sscanf(ptr, "%x~%x",
						  &rfilter[numfilter].can_id,
						  &rfilter[numfilter].can_mask) == 2) {
 					rfilter[numfilter].can_id |= CAN_INV_FILTER;
 					rfilter[numfilter].can_mask &= ~CAN_ERR_FLAG;
					numfilter++;
				} else if (*ptr == 'j' || *ptr == 'J') {
					join_filter = 1;
				} else if (sscanf(ptr, "#%x", &err_mask) != 1) {
					fprintf(stderr, "Error in filter option parsing: '%s'\n", ptr);
					return 1;
				}
			}

			if (err_mask)
				setsockopt(s[i], SOL_CAN_RAW, CAN_RAW_ERR_FILTER,
					   &err_mask, sizeof(err_mask));

			if (join_filter && setsockopt(s[i], SOL_CAN_RAW, CAN_RAW_JOIN_FILTERS,
						      &join_filter, sizeof(join_filter)) < 0) {
				perror("setsockopt CAN_RAW_JOIN_FILTERS not supported by your Linux Kernel");
				return 1;
			}

			if (numfilter)
				setsockopt(s[i], SOL_CAN_RAW, CAN_RAW_FILTER,
					   rfilter, numfilter * sizeof(struct can_filter));

			free(rfilter);

		} /* if (nptr) */

		/* try to switch the socket into CAN FD mode */
		setsockopt(s[i], SOL_CAN_RAW, CAN_RAW_FD_FRAMES, &canfd_on, sizeof(canfd_on));

		if (rcvbuf_size) {

			int curr_rcvbuf_size;
			socklen_t curr_rcvbuf_size_len = sizeof(curr_rcvbuf_size);

			/* try SO_RCVBUFFORCE first, if we run with CAP_NET_ADMIN */
			if (setsockopt(s[i], SOL_SOCKET, SO_RCVBUFFORCE,
				       &rcvbuf_size, sizeof(rcvbuf_size)) < 0) {
#ifdef DEBUG
				printf("SO_RCVBUFFORCE failed so try SO_RCVBUF ...\n");
#endif
				if (setsockopt(s[i], SOL_SOCKET, SO_RCVBUF,
					       &rcvbuf_size, sizeof(rcvbuf_size)) < 0) {
					perror("setsockopt SO_RCVBUF");
					return 1;
				}

				if (getsockopt(s[i], SOL_SOCKET, SO_RCVBUF,
					       &curr_rcvbuf_size, &curr_rcvbuf_size_len) < 0) {
					perror("getsockopt SO_RCVBUF");
					return 1;
				}

				/* Only print a warning the first time we detect the adjustment */
				/* n.b.: The wanted size is doubled in Linux in net/sore/sock.c */
				if (!i && curr_rcvbuf_size < rcvbuf_size*2)
					fprintf(stderr, "The socket receive buffer size was "
						"adjusted due to /proc/sys/net/core/rmem_max.\n");
			}
		}

		if (timestamp || log || logfrmt) {

			if (hwtimestamp) {
				const int timestamping_flags = (SOF_TIMESTAMPING_SOFTWARE | \
								SOF_TIMESTAMPING_RX_SOFTWARE | \
								SOF_TIMESTAMPING_RAW_HARDWARE);

				if (setsockopt(s[i], SOL_SOCKET, SO_TIMESTAMPING,
						&timestamping_flags, sizeof(timestamping_flags)) < 0) {
					perror("setsockopt SO_TIMESTAMPING is not supported by your Linux kernel");
					return 1;
				}
			} else {
				const int timestamp_on = 1;

				if (setsockopt(s[i], SOL_SOCKET, SO_TIMESTAMP,
					       &timestamp_on, sizeof(timestamp_on)) < 0) {
					perror("setsockopt SO_TIMESTAMP");
					return 1;
				}
			}
		}

		if (dropmonitor) {

			const int dropmonitor_on = 1;

			if (setsockopt(s[i], SOL_SOCKET, SO_RXQ_OVFL,
				       &dropmonitor_on, sizeof(dropmonitor_on)) < 0) {
				perror("setsockopt SO_RXQ_OVFL not supported by your Linux Kernel");
				return 1;
			}
		}

		if (bind(s[i], (struct sockaddr *)&addr, sizeof(addr)) < 0) {
			perror("bind");
			return 1;
		}
	}

	if (log) {
		time_t currtime;
		struct tm now;
		char fname[sizeof("candump-2006-11-20_202026.log")+1];

		if (time(&currtime) == (time_t)-1) {
			perror("time");
			return 1;
		}

		localtime_r(&currtime, &now);

		sprintf(fname, "candump-%04d-%02d-%02d_%02d%02d%02d.log",
			now.tm_year + 1900,
			now.tm_mon + 1,
			now.tm_mday,
			now.tm_hour,
			now.tm_min,
			now.tm_sec);

		if (silent != SILENT_ON)
			printf("\nWarning: console output active while logging!");

		fprintf(stderr, "\nEnabling Logfile '%s'\n\n", fname);

		logfile = fopen(fname, "w");
		if (!logfile) {
			perror("logfile");
			return 1;
		}
	}

	/* these settings are static and can be held out of the hot path */
	iov.iov_base = &frame;
	msg.msg_name = &addr;
	msg.msg_iov = &iov;
	msg.msg_iovlen = 1;
	msg.msg_control = &ctrlmsg;

	while (running) {               //select异步IO

		FD_ZERO(&rdfs);
		for (i=0; i<currmax; i++)
			FD_SET(s[i], &rdfs);

		if (timeout_current)
			*timeout_current = timeout_config;

		if ((ret = select(s[currmax-1]+1, &rdfs, NULL, NULL, NULL)) <= 0) {
			//perror("select");
			running = 0;//出错就退出循环
			continue;
		}

		for (i=0; i<currmax; i++) {  /* check all CAN RAW sockets */

			if (FD_ISSET(s[i], &rdfs)) {    //检测那个can设备得到了 数据

				int idx;

				/* these settings may be modified by recvmsg() */
				iov.iov_len = sizeof(frame);
				msg.msg_namelen = sizeof(addr);
				msg.msg_controllen = sizeof(ctrlmsg);
				msg.msg_flags = 0;
                		/////////////////////////
				nbytes = recvmsg(s[i], &msg, 0);    //读取数据
				idx = idx2dindex(addr.can_ifindex, s[i]);
/*START*/
				nbytes_1 = read(s[i], &frame_1, sizeof(frame_1));       //接收报文//显示报文 
				if(nbytes_1 > 0) { 
					printf("ID=0x%X DLC=%d data[0]=0x%X\n", frame_1.can_id, frame_1.can_dlc, frame_1.data[0]); 
				}
/*END*/
				if (nbytes < 0) {
					if ((errno == ENETDOWN) && !down_causes_exit) {
						fprintf(stderr, "%s: interface down\n", devname[idx]);
						continue;
					}
					perror("read");
					return 1;
				}

				if ((size_t)nbytes == CAN_MTU)
					maxdlen = CAN_MAX_DLEN;
				else if ((size_t)nbytes == CANFD_MTU)
					maxdlen = CANFD_MAX_DLEN;
				else {
					fprintf(stderr, "read: incomplete CAN frame\n");
					return 1;
				}

				if (count && (--count == 0))
					running = 0;

				if (bridge) {
					if (bridge_delay)
						usleep(bridge_delay);

					nbytes = write(bridge, &frame, nbytes);
					if (nbytes < 0) {
						perror("bridge write");
						return 1;
					} else if ((size_t)nbytes != CAN_MTU && (size_t)nbytes != CANFD_MTU) {
						fprintf(stderr,"bridge write: incomplete CAN frame\n");
						return 1;
					}
				}

				for (cmsg = CMSG_FIRSTHDR(&msg);
				     cmsg && (cmsg->cmsg_level == SOL_SOCKET);
				     cmsg = CMSG_NXTHDR(&msg,cmsg)) {
					if (cmsg->cmsg_type == SO_TIMESTAMP) {
						memcpy(&tv, CMSG_DATA(cmsg), sizeof(tv));
					} else if (cmsg->cmsg_type == SO_TIMESTAMPING) {

						struct timespec *stamp = (struct timespec *)CMSG_DATA(cmsg);

						/*
						 * stamp[0] is the software timestamp
						 * stamp[1] is deprecated
						 * stamp[2] is the raw hardware timestamp
						 * See chapter 2.1.2 Receive timestamps in
						 * linux/Documentation/networking/timestamping.txt
						 */
						tv.tv_sec = stamp[2].tv_sec;
						tv.tv_usec = stamp[2].tv_nsec/1000;
					} else if (cmsg->cmsg_type == SO_RXQ_OVFL)
						memcpy(&dropcnt[i], CMSG_DATA(cmsg), sizeof(__u32));
				}

				/* check for (unlikely) dropped frames on this specific socket */
				if (dropcnt[i] != last_dropcnt[i]) {

					__u32 frames = dropcnt[i] - last_dropcnt[i];

					if (silent != SILENT_ON)
						printf("DROPCOUNT: dropped %d CAN frame%s on '%s' socket (total drops %d)\n",
						       frames, (frames > 1)?"s":"", devname[idx], dropcnt[i]);

					if (log)
						fprintf(logfile, "DROPCOUNT: dropped %d CAN frame%s on '%s' socket (total drops %d)\n",
							frames, (frames > 1)?"s":"", devname[idx], dropcnt[i]);

					last_dropcnt[i] = dropcnt[i];
				}

				/* once we detected a EFF frame indent SFF frames accordingly */
				if (frame.can_id & CAN_EFF_FLAG)
					view |= CANLIB_VIEW_INDENT_SFF;

				if (log) {
					char buf[CL_CFSZ]; /* max length */

					/* log CAN frame with absolute timestamp & device */
					sprint_canframe(buf, &frame, 0, maxdlen);
					fprintf(logfile, "(%010ld.%06ld) %*s %s\n",
						tv.tv_sec, tv.tv_usec,
						max_devname_len, devname[idx], buf);
				}

				if (logfrmt) {
					char buf[CL_CFSZ]; /* max length */

					/* print CAN frame in log file style to stdout */
					sprint_canframe(buf, &frame, 0, maxdlen);
					printf("(%010ld.%06ld) %*s %s\n",
					       tv.tv_sec, tv.tv_usec,
					       max_devname_len, devname[idx], buf);
					goto out_fflush; /* no other output to stdout */
				}

				if (silent != SILENT_OFF){
					if (silent == SILENT_ANI) {
						printf("%c\b", anichar[silentani%=MAXANI]);
						silentani++;
					}
					goto out_fflush; /* no other output to stdout */
				}

				printf(" %s", (color>2)?col_on[idx%MAXCOL]:"");

				switch (timestamp) {

				case 'a': /* absolute with timestamp */
					printf("(%010ld.%06ld) ", tv.tv_sec, tv.tv_usec);
					break;

				case 'A': /* absolute with date */
				{
					struct tm tm;
					char timestring[25];

					tm = *localtime(&tv.tv_sec);
					strftime(timestring, 24, "%Y-%m-%d %H:%M:%S", &tm);
					printf("(%s.%06ld) ", timestring, tv.tv_usec);
				}
				break;

				case 'd': /* delta */
				case 'z': /* starting with zero */
				{
					struct timeval diff;

					if (last_tv.tv_sec == 0)   /* first init */
						last_tv = tv;
					diff.tv_sec  = tv.tv_sec  - last_tv.tv_sec;
					diff.tv_usec = tv.tv_usec - last_tv.tv_usec;
					if (diff.tv_usec < 0)
						diff.tv_sec--, diff.tv_usec += 1000000;
					if (diff.tv_sec < 0)
						diff.tv_sec = diff.tv_usec = 0;
					printf("(%03ld.%06ld) ", diff.tv_sec, diff.tv_usec);

					if (timestamp == 'd')
						last_tv = tv; /* update for delta calculation */
				}
				break;

				default: /* no timestamp output */
					break;
				}

				printf(" %s", (color && (color<3))?col_on[idx%MAXCOL]:"");
				printf("%*s", max_devname_len, devname[idx]);

				if (extra_msg_info) {

					if (msg.msg_flags & MSG_DONTROUTE)
						printf ("  TX %s", extra_m_info[frame.flags & 3]);
					else
						printf ("  RX %s", extra_m_info[frame.flags & 3]);
				}

				printf("%s  ", (color==1)?col_off:"");

				fprint_long_canframe(stdout, &frame, NULL, view, maxdlen);

				printf("%s", (color>1)?col_off:"");
				printf("\n");
			}

		out_fflush:
			fflush(stdout);
		}
	}

	//关闭！！！！！
	for (i=0; i<currmax; i++)
		close(s[i]);

	if (bridge)
		close(bridge);

	if (log)
		fclose(logfile);

	return 0;
}
