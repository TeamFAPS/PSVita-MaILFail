#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <psp2/ctrl.h>

#include "graphics.h"
#include "utils.h"
#include "sqlite3.h"

#include "installer_sqlite.h"

#define printf psvDebugScreenPrintf

int sql_get_max(sqlite3 *db, const char *sql) {
    int id = 1;
    int ret = 0;
    sqlite3_stmt *stmt;
    ret = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);

    if (ret != SQLITE_OK) {
        printf("Failed to execute %s, error %s\n", sql, sqlite3_errmsg(db));
        goto fail;
    }

    if (sqlite3_step(stmt) == SQLITE_ROW)
        id = sqlite3_column_int(stmt, 0) + 1;

    sqlite3_finalize(stmt);

    return id;

fail:
    sqlite3_close(db);
    press_exit();
    return 0;
}

void sql_simple_exec(sqlite3 *db, const char *sql) {
    char *error = NULL;
    int ret = 0;
    ret = sqlite3_exec(db, sql, NULL, NULL, &error);

    if (error) {
        printf("Failed to execute %s: %s\n", sql, error);
        sqlite3_free(error);
        goto fail;
    }

    return;

fail:
    sqlite3_close(db);
    press_exit();
}