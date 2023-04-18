#pragma once

int file_exists(const char *path);

void copy_file(const char *src, const char *dst);

void delete_file(const char *path);

void mkdirs(const char *dir);

void remove_exploit_files();

int get_key(void);

void press_exit(void);

void pause(void);

void draw_rect(int x, int y, int width, int height, int fg_color);

static unsigned buttons[] = {
        SCE_CTRL_SELECT,
        SCE_CTRL_START,
        SCE_CTRL_UP,
        SCE_CTRL_RIGHT,
        SCE_CTRL_DOWN,
        SCE_CTRL_LEFT,
        SCE_CTRL_LTRIGGER,
        SCE_CTRL_RTRIGGER,
        SCE_CTRL_TRIANGLE,
        SCE_CTRL_CIRCLE,
        SCE_CTRL_CROSS,
        SCE_CTRL_SQUARE,
};

enum {
    LINE_SIZE = 960,
    PROGRESS_BAR_WIDTH = 400,
    PROGRESS_BAR_HEIGHT = 10,
};
