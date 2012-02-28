#include <gtk/gtk.h>

#include <sqlite3.h>

#include "fd-common.h"


#define DB_FILE			"../data/fd.db3"


static sqlite3 *db;

int fd_user_dict_open()
{
	int rc;

	rc = sqlite3_open(DB_FILE, &db);
	if (rc) {
		g_print("failed to open database %s!(%s)\n",
			DB_FILE, sqlite3_errmsg(db));
		return -1;
	}

	return 0;
}

void fd_user_dict_close()
{
	if (db) {
		sqlite3_close(db);
		db = NULL;
	}
}

static int select_callback(void *data, int argc, char **argv, char **colname)
{
	int i;
	struct fd_user_dict_record *r = (struct fd_user_dict_record *)data;

#if 0
	g_print("sqlite3 select callback\n");
	for (i = 0; i < argc; i++) {
		g_print("\t%s = %s\n", colname[i], argv[i] ? argv[i] : "NULL");
	}
#endif

	r->Time = atoi(argv[0]);
	r->Context = g_strdup(argv[1]);
	r->Word = g_strdup(argv[2]);
	r->Answer = g_strdup(argv[3]);
	r->Count = atoi(argv[4]);

	return 0;
}

void fd_user_dict_record_free(struct fd_user_dict_record *rec)
{
	if (rec->Context)
		g_free(rec->Context);
	if (rec->Word)
		g_free(rec->Word);
	if (rec->Answer)
		g_free(rec->Answer);
}

static int update_callback(void *data, int argc, char **argv, char *colname)
{
	g_print("sqlite3 update callback\n");

	return 0;
}

static int insert_callback(void *data, int argc, char **argv, char *colname)
{
	g_print("sqlite3 insert callback\n");

	return 0;
}

gboolean fd_user_dict_lookup(gchar *word, struct fd_user_dict_record *r)
{
	int rc;
	char sql[1024];
	char *errmsg = NULL;

	r->Count = 0;

	sprintf(sql, "SELECT * FROM UserDict WHERE Word=\'%s\'", word);
	rc = sqlite3_exec(db, sql, select_callback, r, &errmsg);
	if (rc != SQLITE_OK) {
		g_print("sql exec select failed! (%s)\n", errmsg);
		sqlite3_free(errmsg);
		return FALSE;
	}

	/* FIXME: not exist if Count value invalidate! */
	if (r->Count <= 0)
		return FALSE;

	return TRUE;
}

int fd_user_dict_add(gchar *word, gchar *answer, gchar *context)
{
	int rc;
	int count;
	char sql[1024];
	char *errmsg = NULL;
	struct fd_user_dict_record r;

	if (!db)
		return -1;

	if (fd_user_dict_lookup(word, &r)) {
		r.Count++;
		sprintf(sql, "UPDATE UserDict SET Time=%d,Context=\'%s\',Answer=\'%s\',Count=%d WHERE Word=\'%s\'",
				time(NULL), context, answer, r.Count, word);
		fd_user_dict_record_free(&r);
		rc = sqlite3_exec(db, sql, update_callback, 0, &errmsg);
		if (rc != SQLITE_OK) {
			g_print("sql exec update failed! (%s)\n", errmsg);
			sqlite3_free(errmsg);
			return -1;
		}
	} else {
		sprintf(sql, "INSERT INTO UserDict (Time,Context,Word,Answer,Count)"
				" VALUES (%d,\'%s\',\'%s\',\'%s\',1)",
				time(NULL), context, word, answer);
		rc = sqlite3_exec(db, sql, insert_callback, 0, &errmsg);
		if (rc != SQLITE_OK) {
			g_print("sql exec insert failed! (%s)\n", errmsg);
			sqlite3_free(errmsg);
			return -1;
		}
	}

	return 0;
}

