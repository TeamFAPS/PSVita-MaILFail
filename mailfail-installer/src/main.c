#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <psp2/types.h>
#include <psp2/ctrl.h>
#include <psp2/io/fcntl.h>
#include <psp2/kernel/processmgr.h>
#include <psp2/io/stat.h>
#include <psp2/power.h>

#include "sqlite3.h"
#include "graphics.h"
#include "installer_sqlite.h"
#include "utils.h"
#include "vita_net.h"

#define printf psvDebugScreenPrintf

#define MAIL_DB_SOURCE "ux0:app/MLFI00001/data/mail.db"
#define MAILFAIL_HTML_SOURCE "ux0:app/MLFI00001/data/mailfail.html"
#define MAILFAIL_HTML_URL "http://quickhen.xyz/mailfail/index.html"
#define MAILFAIL_HTML_TMP_PATH "ux0:data/mailfail.html"
#define MAIL_DB "ux0:email/message/mail.db"
#define MAILFAIL_HTML "ux0:email/message/00/00/mailfail.html"

void remove_mailfail_files() {
	delete_file(MAILFAIL_HTML);
}

void resetEmail() {
	remove_mailfail_files();
	delete_file(MAIL_DB);
	mkdirs("ux0:email/message");
	copy_file(MAIL_DB_SOURCE, MAIL_DB);
}

int uninstall_mailfail() {
	int ret = 0;
	sqlite3 *db;
	if (file_exists(MAIL_DB) <= 0) {
		mkdirs("ux0:email/message");
		copy_file(MAIL_DB_SOURCE, MAIL_DB);
	}
	ret = sqlite3_open(MAIL_DB, &db);
	printf("Uninstalling the mailfail...\n");
	if (ret) {
		printf("Failed to open the mail.db database: %s\n", sqlite3_errmsg(db));
		goto fail;
	}
	int smtpid = -1;
	sqlite3_stmt *stmt2;
	char *cmd2 = "SELECT PrimarySmtpID FROM mt_account WHERE AccountName=\"MailFail Offline\"";
	ret = sqlite3_prepare_v2(db, cmd2, -1, &stmt2, 0);
	if (ret != SQLITE_OK) {
		psvDebugScreenSetFgColor(COLOR_RED);
		printf("Failed to execute select accountid stmt2: %s\n", sqlite3_errmsg(db));
		psvDebugScreenSetFgColor(COLOR_WHITE);
		goto fail;
	}
	if (sqlite3_step(stmt2) == SQLITE_ROW)
		smtpid = sqlite3_column_int(stmt2, 0);
	sqlite3_stmt *stmt;
	char *cmd = "SELECT AccountID FROM mt_account WHERE AccountName=\"MailFail Offline\"";
	ret = sqlite3_prepare_v2(db, cmd, -1, &stmt, 0);
	if (ret != SQLITE_OK) {
		psvDebugScreenSetFgColor(COLOR_RED);
		printf("Failed to execute select accountid stmt: %s\n", sqlite3_errmsg(db));
		psvDebugScreenSetFgColor(COLOR_WHITE);
		goto fail;
	}
	if (sqlite3_step(stmt) == SQLITE_ROW) {
		int aid = sqlite3_column_int(stmt, 0);
		printf("Deleting email account #%d...\n");
		for (const char **table = &tables[0]; *table; ++table) {
			char sql[256] = {0};
			char *error = NULL;
			printf("Deleting from %s...\n", *table);
			snprintf(sql, sizeof(sql), "DELETE FROM %s WHERE AccountID = %d", *table, aid);
			sqlite3_exec(db, sql, NULL, NULL, &error);
			if (error) {
				psvDebugScreenSetFgColor(COLOR_RED);
				printf("Error deleting from %s: %s\n", *table, error);
				psvDebugScreenSetFgColor(COLOR_WHITE);
				sqlite3_free(error);
				goto fail;
			}
		}
	}
	sqlite3_finalize(stmt);
	if (smtpid != -1) {
		printf("Deleting email SMTP #%d...\n");
		char sql[256] = {0};
		char *error = NULL;
		printf("Deleting from mt_smtp...\n");
		snprintf(sql, sizeof(sql), "DELETE FROM mt_smtp WHERE SmtpID = %d", smtpid);
		sqlite3_exec(db, sql, NULL, NULL, &error);
		if (error) {
			psvDebugScreenSetFgColor(COLOR_RED);
			printf("Error deleting from mt_smtp: %s\n", error);
			psvDebugScreenSetFgColor(COLOR_WHITE);
			sqlite3_free(error);
			goto fail;
		}
	}
	sqlite3_finalize(stmt2);
	remove_mailfail_files();
	sqlite3_close(db);
	return 0;
fail:
	sqlite3_close(db);
	remove_mailfail_files();
	press_exit();
	return 1;
}

void install_mailfail() {
	int ret;
	int tempts = 0;
	char sql[0x1000];
	char *error = NULL;
	const char *def = "offline";
	const char *select_max_smtpid_sql = "SELECT MAX(SmtpID) FROM mt_smtp";
	const char *select_max_aid_sql = "SELECT MAX(AccountID) FROM mt_account";
	const char *select_max_fuid_sql = "SELECT MAX(FolderUID) FROM dt_folder";
	const char *select_max_mid_sql = "SELECT MAX(MessageID) FROM dt_message_list";
	const char *select_max_muid_sql = "SELECT MAX(MessageUID) FROM dt_message_list";
	const char *select_max_settingid_sql = "SELECT MAX(SettingID) FROM dt_setting";
	const char *mt_smtp_sql = "INSERT INTO mt_smtp (HostName, Port, EncryptedUserID, EncryptedPassword, UseSSL, AuthType) VALUES ('smtp.example.com', 25, X'00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000', X'00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000', 0, 0)";
	const char *mt_account_sql = "INSERT INTO mt_account (AccountID, Enable, Type, AccountName, Name, Address, HostName, Port, EncryptedUserID, EncryptedPassword, UseSSL, AuthType, ViewOrder, UnreadCount, PrimarySmtpID, LeaveMessageFlag, IMAPPathPrefix, UpdateDate) VALUES (%d, 1, 0, 'MailFail Offline', 'MailFail', 'mailfail@mailfail.xyz', 'mail.example.com', 143, X'00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000', X'00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000', 0, 5, 0, 0, %d, 0, NULL, NULL)";
	const char *dt_folder_sql = "INSERT INTO dt_folder (AccountID, FolderID, FolderUID, FolderName, ParentFolderID, FolderPath, UnreadCount, UnreadCountDisplayFlag, Flag, MaxMessageUID, UpdateDate) VALUES(%d, -1, %d, 'MAILFAIL', 0, 'MAILFAIL', 0, 1, 0, NULL, NULL)";
	const char *dt_folder_outbox_sql = "INSERT INTO dt_folder (AccountID, FolderID, FolderUID, FolderName, ParentFolderID, FolderPath, UnreadCount, UnreadCountDisplayFlag, Flag, MaxMessageUID, UpdateDate) VALUES(%d, -3, NULL, NULL, 0, NULL, 0, 1, 0, NULL, NULL)";
	const char *dt_message_list_sql = "INSERT INTO dt_message_list (AccountID, FolderID, MessageID, MessageUID, MessageIDHeader, 'From', OriginalFrom, 'To', OriginalTo, Cc, OriginalCc, Bcc, OriginalBcc, ReplyTo, OriginalReplyTo, InReplyTo, 'References', Subject, OriginalSubject, SentDate, ReceiveDate, Priority, PreviewBody, AttachmentCount, Flag, DownloadedFlag, MessageSize, StatusFlag, ReplyMessageID, ForwardMessageID, Boundary) VALUES(%d, -1, %d, %d, '<mailfail@mailfail.xyz>', '\"MailFail\" <mailfail@mailfail.xyz>', '\"MailFail\" <mailfail@mailfail.xyz>', '\"MailFail\" <mailfail@mailfail.xyz>', '\"MailFail\" <mailfail@mailfail.xyz>', '', '', '', '', '', '', '', '', 'MailFail Offline', 'MailFail Offline', '2010-01-01 09:00:00', '2010-01-01 09:00:00', 3, NULL, 0, 1, 1, 0, 0, 0, 0, NULL)";
	const char *dt_message_part_sql = "INSERT INTO dt_message_part (AccountID, FolderID, MessageID, PartIndex, Type, OriginalHeader, MimeType, Charset, Encoding, FileName, CID, Section, FilePath, DownloadedFlag, StatusFlag) VALUES (%d, -1, %d, %d, %d, '', '%s', 'utf-8', '7BIT', '', '', %d, '%s', %d, 0)";
	const char *dt_setting_sql = "INSERT INTO dt_setting (SettingID, AutoFetch, Signature, BccMe, DefaultAccountID, NewMessageTemplate, ReplyMessageTemplate, ForwardMessageTemplate, BgReceivedFlag, UpdateDate) VALUES (1, NULL, NULL, NULL, %d, NULL, NULL, NULL, 0, NULL)";
	printf("Installing the mailfail\n");
	if (file_exists(MAIL_DB) <= 0) {
		mkdirs("ux0:email/message");
		copy_file(MAIL_DB_SOURCE, MAIL_DB);
	}
	sqlite3 *db;
	ret = sqlite3_open(MAIL_DB, &db);
	do {
		if (ret) {
			resetEmail();
			ret = sqlite3_open(MAIL_DB, &db);
		}
		tempts++;
	} while (!ret && tempts < 3);
	if (ret) {
		psvDebugScreenSetFgColor(COLOR_RED);
		printf("\n\nFailed to open the mail.db database: %s\nMake sure that KaiHEN unsafe mode is on!!\n\n", sqlite3_errmsg(db));
		psvDebugScreenSetFgColor(COLOR_WHITE);
		goto fail;
	}
	// make SMTP: mt_smtp
	int smtpid = sql_get_max(db, select_max_smtpid_sql);
	printf("Creating SMTP settings %d\n", smtpid);
	sql_simple_exec(db, mt_smtp_sql);
	// make account: mt_account
	int aid = sql_get_max(db, select_max_aid_sql);
	printf("Creating account ID %d\n", aid);
	snprintf(sql, sizeof(sql), mt_account_sql, aid, smtpid);
	sql_simple_exec(db, sql);
	// make mailfail folder: dt_folder
	int fuid = sql_get_max(db, select_max_fuid_sql);
	printf("Creating mailfail folder %d\n", fuid);
	snprintf(sql, sizeof(sql), dt_folder_sql, aid, fuid);
	sql_simple_exec(db, sql);
	// make outbox folder: dt_folder
	printf("Creating outbox folder %d\n");
	snprintf(sql, sizeof(sql), dt_folder_outbox_sql, aid);
	sql_simple_exec(db, sql);
	// make message: dt_message_list
	int mid = sql_get_max(db, select_max_mid_sql);
	int muid = sql_get_max(db, select_max_muid_sql);
	printf("Creating message %d:%d\n", mid, muid);
	snprintf(sql, sizeof(sql), dt_message_list_sql, aid, mid, muid);
	sql_simple_exec(db, sql);
	// make message: dt_message_part
	printf("Creating message part2\n");
	snprintf(sql, sizeof(sql), dt_message_part_sql, aid, mid, 1, 1, "TEXT/HTML", 1, MAILFAIL_HTML, 1);
	sql_simple_exec(db, sql);
	// install dummy settings: dt_setting
	int sid = sql_get_max(db, select_max_settingid_sql);
	if (sid == 1) {
		printf("Installing email settings\n");
		snprintf(sql, sizeof(sql), dt_setting_sql, aid);
		sql_simple_exec(db, sql);
	}
	sqlite3_close(db);
	db = NULL;
	mkdirs("ux0:email/message/00/00/");
	mkdirs("ux0:picture");
	copy_file(MAILFAIL_HTML_SOURCE, MAILFAIL_HTML);
	psvDebugScreenSetFgColor(COLOR_GREEN);
	printf("MailFail offline was installed successfully!\n");
	return;
fail:
	if (db)
		sqlite3_close(db);
	remove_mailfail_files();
	psvDebugScreenSetFgColor(COLOR_RED);
	printf("\n\nInstallation error!\n\n");
	psvDebugScreenSetFgColor(COLOR_WHITE);
	pause();
}

void drawHead(void) {
	psvDebugScreenClear(COLOR_BLACK);
	psvDebugScreenSetFgColor(COLOR_CYAN);
	printf("MailFail offline installer\n\n\n");
	psvDebugScreenSetFgColor(COLOR_WHITE);
	printf("MailFail offline installer lets you install and manage the offline version of MailFail.\n");
	printf("Back up all your email data (ux0:email directory) if you have anything important there.\n");
	printf("Make sure that the Email application is closed, otherwise the installation may fail.\n\n\n");
	printf("\n\n");
}

void drawCredits() {
	int key = 0;
credits:
	psvDebugScreenClear(COLOR_BLACK);
	psvDebugScreenSetFgColor(COLOR_CYAN);
	printf("MailFail offline installer -  by Team FAPS\n\n\n");
	psvDebugScreenSetFgColor(COLOR_WHITE);
	printf("Credits:\n\n");
	printf("xyzz: OfflineInstaller\n");
	printf("Team FAPS: MailFail\n");
	printf("Team FAPS and Team Molecule: KaiHEN\n");
	printf("luck: SwitchKaku\n");
	printf("marek256: background graphics\n");
	printf("\n\n\n\n> Press () to go back\n");
	key = get_key();
	if (key == SCE_CTRL_CIRCLE)
		return;
	goto credits;
}

int main(void) {
	int key = 0;
	int ret = 0;
	int isInstalled = 0;
	char *expl = "Unknown";
	netInit();
	httpInit();
	psvDebugScreenInit();
menu:
	drawHead();
	expl = "Unknown";
	isInstalled = 0;
	ret = 0;
	if (file_exists(MAILFAIL_HTML) > 0 && file_exists(MAIL_DB) > 0) {
		expl = "MailFail";
		isInstalled = 1;
	}
	printf("Current offline mailfail: %s\n\n\n\n", expl);
	printf("#### Installation Options ####\n\n");
	if (!isInstalled) {
		printf("> Press START to install MailFail - [Offline]\n");
		printf("> Press /\\ to install/update MailFail - [Online].\n");
	}
	if (isInstalled)
		printf("> Press () to uninstall MailFail offline.\n");
	printf("\n\n\n");
	printf("#### Extra Options ####\n\n");
	printf("> Press SELECT to reset Email application\n");
	printf("> Press [] to remove id.dat (will rebuild database on reboot)\n");
	printf("> Press L to show credits\n");
	printf("> Press R to reboot\n");
	printf("\n");
	key = get_key();
	switch (key) {
		case SCE_CTRL_TRIANGLE:
			if (!isInstalled)
				install_mailfail();
			break;
		case SCE_CTRL_CIRCLE:
			if (isInstalled == 1)
				uninstall_mailfail();
			break;
		case SCE_CTRL_SQUARE:
			delete_file("ux0:/id.dat");
			break;
		case SCE_CTRL_RTRIGGER:
			scePowerRequestColdReset();
			break;
		case SCE_CTRL_LTRIGGER:
			drawCredits();
			break;
		case SCE_CTRL_SELECT:
			resetEmail();
			break;
		case SCE_CTRL_START:
			if (!isInstalled)
				install_mailfail();
			break;
		default:
			psvDebugScreenSetFgColor(COLOR_RED);
			printf("Invalid input, try again.\n\n");
			goto menu;
	}
	goto menu;
}