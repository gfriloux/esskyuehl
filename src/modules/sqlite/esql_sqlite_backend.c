/*
 * Copyright 2012 Mike Blumenkrantz <michael.blumenkrantz@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation, either version 2.1 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "esql_module.h"
#include <sqlite3.h>

typedef struct _Esql_Sqlite_Res
{
   sqlite3_stmt *stmt;
} Esql_Sqlite_Res;

static const char *esql_sqlite_error_get(Esql *e);
static void esql_sqlite_disconnect(Esql *e);
static int esql_sqlite_fd_get(Esql *e);
static int esql_sqlite_connect(Esql *e);
static int esql_sqlite_io(Esql *e);
static void esql_sqlite_setup(Esql *e, const char *addr, const char *user, const char *passwd);
static void esql_sqlite_query(Esql *e, const char *query, unsigned int len);
static void esql_sqlite_res_free(Esql_Res *res);
static void esql_sqlite_res(Esql_Res *res);
static char *esql_sqlite_escape(Esql *e, unsigned int *len, const char *fmt, va_list args);
static void esql_sqlite_row_add(Esql_Res *res);
static void esql_sqlite_free(Esql *e);


static const char *
esql_sqlite_error_get(Esql *e)
{
   if (sqlite3_errcode(e->backend.db))
     return sqlite3_errmsg(e->backend.db);
   return NULL;
}

static void
esql_sqlite_disconnect(Esql *e)
{
   if (!e->backend.db) return;
   while (sqlite3_close(e->backend.db));
   e->backend.db = NULL;
   free(e->backend.conn_str);
   e->backend.conn_str = NULL;
   e->backend.conn_str_len = 0;
}

static int
esql_sqlite_fd_get(Esql *e EINA_UNUSED)
{
   /* dummy return */
   return -1;
}

static void
esql_sqlite_thread_end_cb(Esql *e, Ecore_Thread *et EINA_UNUSED)
{
   DBG("(e=%p)", e);
   e->backend.thread = NULL;
   if (e->backend.stmt) sqlite3_finalize(e->backend.stmt);
   e->backend.stmt = NULL;
   esql_call_complete(e);
}

static void
esql_sqlite_thread_cancel_cb(Esql *e, Ecore_Thread *et EINA_UNUSED)
{
   DBG("(e=%p)", e);
   e->backend.thread = NULL;
   esql_event_error(e);
}

static void
esql_sqlite_connect_cb(Esql *e, Ecore_Thread *et)
{
   if (sqlite3_open_v2(e->backend.conn_str, (struct sqlite3 **)&e->backend.db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_NOMUTEX, NULL))
     {
        ERR("Could not open %s: %s",
            e->backend.conn_str, esql_sqlite_error_get(e));
        ecore_thread_cancel(et);
        e->backend.thread = NULL;
     }
   else
     e->current = ESQL_CONNECT_TYPE_INIT;
}

static int
esql_sqlite_connect(Esql *e)
{
   e->backend.thread = ecore_thread_run((Ecore_Thread_Cb)esql_sqlite_connect_cb,
                                      (Ecore_Thread_Cb)esql_sqlite_thread_end_cb,
                                      (Ecore_Thread_Cb)esql_sqlite_thread_cancel_cb, e);
   EINA_SAFETY_ON_NULL_RETURN_VAL(e->backend.thread, ECORE_FD_ERROR);
   return ECORE_FD_READ | ECORE_FD_WRITE;
}

static void
esql_module_setup_cb(Esql_Sqlite_Res *res, int col, Eina_Value_Struct_Member *m)
{
   m->name = eina_stringshare_add(sqlite3_column_name(res->stmt, col));

   switch (sqlite3_column_type(res->stmt, col))
     {
      case SQLITE_TEXT:
         m->type = EINA_VALUE_TYPE_STRING;
         break;
      case SQLITE_INTEGER:
         m->type = EINA_VALUE_TYPE_INT64;
         break;
      case SQLITE_FLOAT:
         m->type = EINA_VALUE_TYPE_DOUBLE;
         break;
      default:
         m->type = EINA_VALUE_TYPE_BLOB;
     }
}

static Eina_Bool
esql_sqlite_res_init(Esql *e)
{
   Esql_Sqlite_Res *res;

   e->res = esql_res_calloc(1);
   if (!e->res) return EINA_FALSE;

   res = calloc(1, sizeof(Esql_Sqlite_Res));
   if (!res)
     {
        esql_res_free(NULL, e->res);
        return EINA_FALSE;
     }

   res->stmt = e->backend.stmt;
   e->res->backend.res = res;

   e->res->e = e;
   e->res->desc = esql_module_desc_get(sqlite3_column_count(e->backend.stmt), (Esql_Module_Setup_Cb)esql_module_setup_cb, e->res);
   e->res->affected = sqlite3_changes(e->backend.db);

   return EINA_TRUE;
}

static void
esql_sqlite_query_cb(Esql *e, Ecore_Thread *et)
{
   int ret = -1, tries = 0;

   while (++tries < 1000)
     {
        if (!e->backend.stmt)
          return;
        ret = sqlite3_step(e->backend.stmt);
        switch (ret)
          {
           case SQLITE_BUSY:
             WARN("SQLite is busy, retry...");
             break;
           case SQLITE_DONE:
             if (!e->res)
               {
                  if (!esql_sqlite_res_init(e)) goto out;
               }
             sqlite3_finalize(e->backend.stmt);
             e->backend.stmt = NULL;
             e->res->id = sqlite3_last_insert_rowid(e->backend.db);
             return;
           case SQLITE_ROW:
             if (!e->res)
               {
                  if (!esql_sqlite_res_init(e)) goto out;
               }
             esql_sqlite_row_add(e->res);
             tries = 0;
             break;

           default:
             ERR("Unexpected return from sqlite3_step(): %d", ret);
             goto out;
          }
     }
   /* something crazy is going on */
out:
   ERR("Query failed. Tries %d, ret %d", tries, ret);
   sqlite3_finalize(e->backend.stmt);
   e->backend.stmt = NULL;
   ecore_thread_cancel(et);
}

static int
esql_sqlite_io(Esql *e)
{
   DBG("(e=%p, thread=%p)", e, e->backend.thread);
   if (e->backend.thread) return ECORE_FD_ERROR;
   e->backend.thread = ecore_thread_run((Ecore_Thread_Cb)esql_sqlite_query_cb,
                                      (Ecore_Thread_Cb)esql_sqlite_thread_end_cb,
                                      (Ecore_Thread_Cb)esql_sqlite_thread_cancel_cb, e);
   EINA_SAFETY_ON_NULL_RETURN_VAL(e->backend.thread, ECORE_FD_ERROR);
   return ECORE_FD_READ | ECORE_FD_WRITE;
}

static void
esql_sqlite_setup(Esql *e, const char *addr, const char *user EINA_UNUSED, const char *passwd EINA_UNUSED)
{
   e->backend.conn_str = strdup(addr);
}

static void
esql_sqlite_query(Esql *e, const char *query, unsigned int len)
{
   EINA_SAFETY_ON_TRUE_RETURN(sqlite3_prepare_v2(e->backend.db, query, len, (struct sqlite3_stmt**)&e->backend.stmt, NULL));
}

static void
esql_sqlite_res_free(Esql_Res *res)
{
   free(res->backend.res);
}

static void
esql_sqlite_res(Esql_Res *res EINA_UNUSED)
{
}

static char *
esql_sqlite_escape(Esql *e EINA_UNUSED, unsigned int *len, const char *fmt, va_list args)
{
   return esql_query_escape(EINA_TRUE, len, fmt, args);
}

static void
esql_sqlite_row_add(Esql_Res *res)
{
   Esql_Row *r;
   Eina_Value *val;
   unsigned int i;

   r = esql_row_calloc(1);
   EINA_SAFETY_ON_NULL_RETURN(r);
   r->res = res;
   res->row_count++;
   res->rows = eina_inlist_append(res->rows, EINA_INLIST_GET(r));

   val = &(r->value);
   eina_value_struct_setup(val, res->desc);

   for (i = 0; i < res->desc->member_count; i++)
     {
        const Eina_Value_Struct_Member *m = res->desc->members + i;
        Eina_Value inv;

        switch (sqlite3_column_type(res->e->backend.stmt, i))
          {
           case SQLITE_TEXT:
             {
                eina_value_setup(&inv, EINA_VALUE_TYPE_STRING);
                eina_value_set(&inv, sqlite3_column_text(res->e->backend.stmt, i));
                break;
             }
           case SQLITE_INTEGER:
             eina_value_setup(&inv, EINA_VALUE_TYPE_INT64);
             eina_value_set(&inv, sqlite3_column_int64(res->e->backend.stmt, i));
             break;

           case SQLITE_FLOAT:
             eina_value_setup(&inv, EINA_VALUE_TYPE_DOUBLE);
             eina_value_set(&inv, sqlite3_column_double(res->e->backend.stmt, i));
             break;

           case SQLITE_BLOB:
             {
                Eina_Value_Blob blob;
                char *tmp;
                int size;

                size = sqlite3_column_bytes(res->e->backend.stmt, i);
                tmp = malloc(size);
                memcpy(tmp, sqlite3_column_blob(res->e->backend.stmt, i), size);

                blob.ops = NULL;
                blob.memory = tmp;
                blob.size = size;
                eina_value_setup(&inv, EINA_VALUE_TYPE_BLOB);
                eina_value_set(&inv, blob);
                break;
             }
           default: continue;
          }

        eina_value_struct_member_value_set(val, m, &inv);
        eina_value_flush(&inv);
     }
}

static void
esql_sqlite_free(Esql *e)
{
   if (!e->backend.db) return;
   while (sqlite3_close(e->backend.db));
   e->backend.db = NULL;
   e->backend.free = NULL;
   free(e->backend.conn_str);
   e->backend.conn_str = NULL;
   e->backend.conn_str_len = 0;
}

static void
esql_sqlite_init(Esql *e)
{
   INFO("Esql type for %p set to SQLite", e);
   e->type = ESQL_TYPE_SQLITE;
   e->backend.connect = esql_sqlite_connect;
   e->backend.disconnect = esql_sqlite_disconnect;
   e->backend.error_get = esql_sqlite_error_get;
   e->backend.setup = esql_sqlite_setup;
   e->backend.io = esql_sqlite_io;
   e->backend.fd_get = esql_sqlite_fd_get;
   e->backend.escape = esql_sqlite_escape;
   e->backend.query = esql_sqlite_query;
   e->backend.res = esql_sqlite_res;
   e->backend.res_free = esql_sqlite_res_free;
   e->backend.free = esql_sqlite_free;
}

EAPI Esql_Type
esql_module_init(Esql *e)
{
   if (e) esql_sqlite_init(e);
   sqlite3_initialize();
   return ESQL_TYPE_SQLITE;
}
