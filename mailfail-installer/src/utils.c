#include <stdio.h>
#include <string.h>

#include <psp2/types.h>
#include <psp2/ctrl.h>
#include <psp2/io/fcntl.h>
#include <psp2/kernel/threadmgr.h>
#include <psp2/kernel/processmgr.h>
#include <psp2/io/stat.h>

#include "graphics.h"

#include "utils.h"

#define printf psvDebugScreenPrintf

int file_exists(const char *path) {
    int ret = 0;
    int fd = sceIoOpen(path, SCE_O_RDONLY, 0);
    ret = fd;
    sceIoClose(fd);
    return ret;
}

void copy_file(const char *src, const char *dst) {
    unsigned char buffer[128 * 128];
    int read = 0;
    int fsrc = sceIoOpen(src, SCE_O_RDONLY, 0777);
    int fdst  = sceIoOpen(dst, SCE_O_WRONLY | SCE_O_CREAT, 0777);
    while ((read = sceIoRead(fsrc, buffer, sizeof(buffer))) > 0)
        sceIoWrite(fdst, buffer, read);
    return;
}

void delete_file(const char *path) {
    if (file_exists(path) > 0) {
        int ret = sceIoRemove(path);
        printf("Deleting file %s... ", path);
        if (ret < 0)
            printf("fail\n");
        else
            printf("ok\n");
    }
}

void mkdirs(const char *dir) {
    char dir_copy[0x400] = {0};
    char *c;
    snprintf(dir_copy, sizeof(dir_copy) - 2, "%s", dir);
    dir_copy[strlen(dir_copy)] = '/';
    for (c = dir_copy; *c; ++c) {
        if (*c == '/') {
            *c = '\0';
            sceIoMkdir(dir_copy, 0777);
            *c = '/';
        }
    }
}

int get_key(void) {
    static unsigned prev = 0;
    SceCtrlData pad;
    while (1) {
        memset(&pad, 0, sizeof(pad));
        sceCtrlPeekBufferPositive(0, &pad, 1);
        unsigned next = prev ^ (pad.buttons & prev);
        prev = pad.buttons;
        for (int i = 0; i < sizeof(buttons)/sizeof(*buttons); ++i)
            if (next & buttons[i])
                return buttons[i];
        sceKernelDelayThread(1000); // 1ms
    }
}

void press_exit(void) {
    printf("Press any key to exit.\n");
    get_key();
    sceKernelExitProcess(0);
}

void pause(void) {
    printf("Press any key to continue...\n");
    get_key();
}

void draw_rect(int x, int y, int width, int height, int fg_color) {
    void *base = psvDebugScreenGetVram();
    for (int j = y; j < y + height; ++j)
        for (int i = x; i < x + width; ++i)
            ((unsigned*)base)[j * LINE_SIZE + i] = fg_color;
}