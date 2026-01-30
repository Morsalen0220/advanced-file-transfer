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
    int r = 0;
    while(r < len){
        int n = recv(s, buf + r, len - r, 0);
        if(n <= 0) return -1;
        r += n;
    }
    return r;
}

/* ---------- UDP DISCOVERY THREAD ---------- */
DWORD WINAPI udp_discovery(LPVOID p){
    char pcname[64];
    DWORD psz = sizeof(pcname);
    GetComputerNameA(pcname, &psz);

    SOCKET udp = socket(AF_INET, SOCK_DGRAM, 0);

    struct sockaddr_in u = {0};
    u.sin_family = AF_INET;
    u.sin_port   = htons(DISCOVERY_PORT);
    u.sin_addr.s_addr = INADDR_ANY;

    bind(udp, (SOCKADDR*)&u, sizeof(u));

    while(1){
        char buf[64];
        struct sockaddr_in sender;
        int sl = sizeof(sender);

        int n = recvfrom(udp, buf, sizeof(buf)-1, 0,
                         (SOCKADDR*)&sender, &sl);
        if(n > 0){
            buf[n] = 0;
            if(strcmp(buf,"UF_DISCOVER") == 0){
                sendto(udp, pcname, strlen(pcname), 0,
                       (SOCKADDR*)&sender, sl);
            }
        }
    }
    return 0;
}

int main(){
    WSADATA w;
    WSAStartup(MAKEWORD(2,2), &w);

    /* Select save directory */
    char save_dir[MAX_PATH];
    BROWSEINFO bi = {0};
    LPITEMIDLIST id = SHBrowseForFolder(&bi);
    if(!id){
        WSACleanup();
        return 0;
    }
    SHGetPathFromIDList(id, save_dir);

    /* Start UDP discovery thread */
    CreateThread(NULL,0,udp_discovery,NULL,0,NULL);

    /* ---------- TCP SERVER ---------- */
    SOCKET srv = socket(AF_INET, SOCK_STREAM, 0);

    int opt = 1;
    setsockopt(srv, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof(opt));

    struct sockaddr_in t = {0};
    t.sin_family = AF_INET;
    t.sin_port   = htons(TCP_PORT);
    t.sin_addr.s_addr = INADDR_ANY;

    if(bind(srv,(SOCKADDR*)&t,sizeof(t)) != 0){
        MessageBox(0,"TCP bind failed","Receiver",MB_OK);
        return 0;
    }

    if(listen(srv,5) != 0){
        MessageBox(0,"TCP listen failed","Receiver",MB_OK);
        return 0;
    }

    MessageBox(0,"Receiver ready","Receiver",MB_OK);

    /* ---------- ACCEPT LOOP ---------- */
    while(1){
        SOCKET cli = accept(srv, NULL, NULL);
        if(cli == INVALID_SOCKET) continue;

        int fname_len;
        if(recv_all(cli,(char*)&fname_len,sizeof(int)) < 0){
            closesocket(cli);
            continue;
        }

        char fname[MAX_PATH];
        recv_all(cli, fname, fname_len);

        long long size;
        recv_all(cli,(char*)&size,sizeof(long long));

        char full[MAX_PATH];
        sprintf(full,"%s\\%s",save_dir,fname);

        FILE *f = fopen(full,"wb");
        if(!f){
            closesocket(cli);
            continue;
        }

        char buf[BUF];
        long long rcv = 0;

        while(rcv < size){
            int n = recv(cli,buf,BUF,0);
            if(n <= 0) break;
            fwrite(buf,1,n,f);
            rcv += n;
        }

        fclose(f);
        closesocket(cli);
    }
}
