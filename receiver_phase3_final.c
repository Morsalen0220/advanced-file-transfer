#include <winsock2.h>
#include <windows.h>
#include <shlobj.h>
#include <stdio.h>
#include <string.h>

#pragma comment(lib,"ws2_32.lib")

#define DISCOVERY_PORT 7777
#define TCP_PORT 9000
#define BUF 65536

int recv_all(SOCKET s, char *buf, int len){
    int r=0;
    while(r<len){
        int n=recv(s,buf+r,len-r,0);
        if(n<=0) return -1;
        r+=n;
    }
    return r;
}

int main(){
    WSADATA w;
    WSAStartup(MAKEWORD(2,2), &w);

    char pcname[64];
    DWORD psz=sizeof(pcname);
    GetComputerNameA(pcname,&psz);

    char save_dir[MAX_PATH];
    BROWSEINFO bi={0};
    LPITEMIDLIST id=SHBrowseForFolder(&bi);
    if(!id) return 0;
    SHGetPathFromIDList(id,save_dir);

    /* UDP socket */
    SOCKET udp=socket(AF_INET,SOCK_DGRAM,0);
    struct sockaddr_in u={0};
    u.sin_family=AF_INET;
    u.sin_port=htons(DISCOVERY_PORT);
    u.sin_addr.s_addr=INADDR_ANY;
    bind(udp,(SOCKADDR*)&u,sizeof(u));

    /* TCP server */
    SOCKET srv=socket(AF_INET,SOCK_STREAM,0);
    int opt=1, bufsize=8*1024*1024;
    setsockopt(srv,SOL_SOCKET,SO_REUSEADDR,(char*)&opt,sizeof(opt));
    setsockopt(srv,SOL_SOCKET,SO_RCVBUF,(char*)&bufsize,sizeof(bufsize));

    struct sockaddr_in t={0};
    t.sin_family=AF_INET;
    t.sin_port=htons(TCP_PORT);
    t.sin_addr.s_addr=INADDR_ANY;
    bind(srv,(SOCKADDR*)&t,sizeof(t));
    listen(srv,5);

    /* 🔥 CRITICAL FIX */
    u_long mode=1;
    ioctlsocket(srv,FIONBIO,&mode); // non-blocking accept

    while(1){
        /* UDP discovery */
        fd_set fds;
        FD_ZERO(&fds);
        FD_SET(udp,&fds);
        struct timeval tv={0,0};

        if(select(0,&fds,NULL,NULL,&tv)>0){
            char buf[64];
            struct sockaddr_in sender;
            int sl=sizeof(sender);
            int n=recvfrom(udp,buf,sizeof(buf)-1,0,
                           (SOCKADDR*)&sender,&sl);
            if(n>0){
                buf[n]=0;
                if(strcmp(buf,"UF_DISCOVER")==0){
                    sendto(udp,pcname,strlen(pcname),0,
                           (SOCKADDR*)&sender,sl);
                }
            }
        }

        /* TCP accept (non-blocking now) */
        SOCKET cli=accept(srv,NULL,NULL);
        if(cli==INVALID_SOCKET){
            Sleep(10);
            continue;
        }

        int fname_len;
        recv_all(cli,(char*)&fname_len,sizeof(int));

        char fname[MAX_PATH];
        recv_all(cli,fname,fname_len);

        long long size;
        recv_all(cli,(char*)&size,sizeof(long long));

        char full[MAX_PATH];
        sprintf(full,"%s\\%s",save_dir,fname);

        FILE *f=fopen(full,"wb");
        long long rcv=0;
        char buf[BUF];

        while(rcv<size){
            int n=recv(cli,buf,BUF,0);
            if(n<=0) break;
            fwrite(buf,1,n,f);
            rcv+=n;
        }

        fclose(f);
        closesocket(cli);
    }
}
