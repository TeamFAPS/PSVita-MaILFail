#pragma once

int sql_get_max(sqlite3 *db, const char *sql);

void sql_simple_exec(sqlite3 *db, const char *sql);

static const char* tables[] = {
        "dt_folder",
        "dt_message_list",
        "dt_message_part",
        "dt_received_uid_list",
        "dt_task",
        "mt_account",
        NULL
};
