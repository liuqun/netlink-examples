#if 0
/*
 * 文件名：dump-ip-addr.c
 * 功能：通过Netlink套接字从内核中得到所有网口的IP地址。
 * 原作者：louyang
 * 原作链接：https://www.jianshu.com/p/073bcd9c3b08
 * 來源：简书
 * 简书著作权归作者所有，任何形式的转载都请联系作者获得授权并注明出处。
 *
 * 修改者：阿群1986（ github.com/liuqun 或 jianshu.com/u/b170a0cdcc72 ）
 *
 * 编译：
 *     $ make
 *
 * 运行：
 *     $ ./dump-ip-addr
 */
#endif

/*
 *  Display all IPv4 addresses
 */
#include <stdio.h>            //printf, perror
#include <string.h>           //memset, strlen
#include <stdlib.h>           //exit
#include <unistd.h>           //close
#include <sys/socket.h>       //msghdr
#include <arpa/inet.h>        //inet_ntop
#include <linux/netlink.h>    //sockaddr_nl
#include <linux/rtnetlink.h>  //rtgenmsg,ifinfomsg

#define BUFSIZE 8192

struct nl_req_s {
  struct nlmsghdr hdr;
  struct rtgenmsg gen;
};

void die(char *s)
{
    perror(s);
    exit(1);
}

void rtnl_print_addr(struct nlmsghdr * h)
{
    struct ifaddrmsg * addr;
    struct rtattr * attr;
    int len;

    addr = NLMSG_DATA(h);
    len = RTM_PAYLOAD(h);

    /* loop over all attributes for the NEWLINK message */
    for (attr = IFLA_RTA(addr); RTA_OK(attr, len); attr = RTA_NEXT(attr, len))
    {
        switch (attr->rta_type)
        {
        case IFA_LABEL:
            printf("Interface  : %s\n", (char *)RTA_DATA(attr));
            break;
        case IFA_LOCAL:
        {
            int ip = *(int*)RTA_DATA(attr);
            unsigned char bytes[4];
            bytes[0] = ip & 0xFF;
            bytes[1] = (ip >> 8) & 0xFF;
            bytes[2] = (ip >> 16) & 0xFF;
            bytes[3] = (ip >> 24) & 0xFF;
            printf("IP Address : %d.%d.%d.%d\n", bytes[0], bytes[1], bytes[2], bytes[3]);
            break;
        }
        default:
            break;
        }
    }
}

int main(void)
{
    struct sockaddr_nl kernel;
    int s, end=0, len;
    struct msghdr msg;
    struct nl_req_s req;
    struct iovec io;
    char buf[BUFSIZE];

    //build kernel netlink address
    memset(&kernel, 0, sizeof(kernel));
    kernel.nl_family = AF_NETLINK;
    kernel.nl_groups = 0;

    //create a Netlink socket
    if ((s=socket(AF_NETLINK, SOCK_RAW, NETLINK_ROUTE)) < 0)
    {
        die("socket");
    }

    //build netlink message
    memset(&req, 0, sizeof(req));
    req.hdr.nlmsg_len = NLMSG_LENGTH(sizeof(struct rtgenmsg));
    req.hdr.nlmsg_type = RTM_GETADDR;
    req.hdr.nlmsg_flags = NLM_F_REQUEST | NLM_F_DUMP;
    req.hdr.nlmsg_seq = 1;
    req.hdr.nlmsg_pid = getpid();
    req.gen.rtgen_family = AF_INET;

    memset(&io, 0, sizeof(io));
    io.iov_base = &req;
    io.iov_len = req.hdr.nlmsg_len;

    memset(&msg, 0, sizeof(msg));
    msg.msg_iov = &io;
    msg.msg_iovlen = 1;
    msg.msg_name = &kernel;
    msg.msg_namelen = sizeof(kernel);

    //send the message
    if (sendmsg(s, &msg, 0) < 0)
    {
        die("sendmsg");
    }

    //parse reply
    while (!end)
    {
        memset(buf, 0, BUFSIZE);
        msg.msg_iov->iov_base = buf;
        msg.msg_iov->iov_len = BUFSIZE;
        if ((len=recvmsg(s, &msg, 0)) < 0)
        {
            die("recvmsg");
        }
        for (struct nlmsghdr * msg_ptr = (struct nlmsghdr *)buf;
             NLMSG_OK(msg_ptr, len); msg_ptr = NLMSG_NEXT(msg_ptr, len))
        {
            switch (msg_ptr->nlmsg_type)
            {
            case NLMSG_DONE:
                end++;
                break;
            case RTM_NEWADDR:
                rtnl_print_addr(msg_ptr);
                break;
            default:
                printf("Ignored msg: type=%d, len=%d\n", msg_ptr->nlmsg_type, msg_ptr->nlmsg_len);
                break;
            }
        }
    }

    close(s);
    return 0;
}

// Source code available from Github:
// - https://github.com/liuqun/netlink-examples
