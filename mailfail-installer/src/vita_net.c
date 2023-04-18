#include <stdio.h>
#include <stdlib.h>

#include <psp2/types.h>
#include <psp2/ctrl.h>
#include <psp2/io/fcntl.h>
#include <psp2/sysmodule.h>
#include <psp2/net/http.h>
#include <psp2/net/net.h>
#include <psp2/net/netctl.h>
#include <psp2/io/stat.h>

#include "graphics.h"
#include "utils.h"

#include "vita_net.h"

#define printf psvDebugScreenPrintf

void netInit() {
    sceSysmoduleLoadModule(SCE_SYSMODULE_NET);

    SceNetInitParam netInitParam;
    int size = 1*1024*1024;
    netInitParam.memory = malloc(size);
    netInitParam.size = size;
    netInitParam.flags = 0;
    sceNetInit(&netInitParam);

    sceNetCtlInit();
}

void httpInit() {
    sceSysmoduleLoadModule(SCE_SYSMODULE_HTTP);

    sceHttpInit(1*1024*1024);
}

int download_file(const char *src, const char *dst) {
    int ret;
    int fg_color;

    printf("downloading %s to %s\n", src, dst);

    int tpl = sceHttpCreateTemplate("henkaku offline", 2, 1);
    if (tpl < 0) {
        printf("sceHttpCreateTemplate: 0x%x\n", tpl);
        return tpl;
    }

    int conn = sceHttpCreateConnectionWithURL(tpl, src, 0);
    if (conn < 0) {
        printf("sceHttpCreateConnectionWithURL: 0x%x\n", conn);
        return conn;
    }

    int req = sceHttpCreateRequestWithURL(conn, 0, src, 0);
    if (req < 0) {
        printf("sceHttpCreateRequestWithURL: 0x%x\n", req);
        return req;
    }

    ret = sceHttpSendRequest(req, NULL, 0);
    if (ret < 0) {
        printf("sceHttpSendRequest: 0x%x\n", ret);
        return ret;
    }

    unsigned char buf[4096] = {0};

    long long length = 0;
    ret = sceHttpGetResponseContentLength(req, &length);

    int fd = sceIoOpen(dst, SCE_O_WRONLY | SCE_O_CREAT | SCE_O_TRUNC, 6);
    int total_read = 0;
    if (fd < 0) {
        printf("sceIoOpen: 0x%x\n", fd);
        return fd;
    }

    // draw progress bar background
    fg_color = 0xFF666666;
    draw_rect(psvDebugScreenGetX(), psvDebugScreenGetY(), PROGRESS_BAR_WIDTH, PROGRESS_BAR_HEIGHT, fg_color);
    fg_color = 0xFFFFFFFF;

    while (1) {
        int read = sceHttpReadData(req, buf, sizeof(buf));
        if (read < 0) {
            printf("sceHttpReadData error! 0x%x\n", read);
            return read;
        }

        if (read == 0)
            break;

        ret = sceIoWrite(fd, buf, read);
        if (ret < 0 || ret != read) {
            printf("sceIoWrite error! 0x%x\n", ret);
            if (ret < 0)
                return ret;
            return -1;
        }

        total_read += read;
        draw_rect(psvDebugScreenGetX() + 1, psvDebugScreenGetY() + 1, (PROGRESS_BAR_WIDTH - 2) * total_read / length, PROGRESS_BAR_HEIGHT - 2, fg_color);
    }

    printf("\n\n");
    ret = sceIoClose(fd);

    if (ret < 0)
        printf("sceIoClose: 0x%x\n", ret);

    return 0;
}