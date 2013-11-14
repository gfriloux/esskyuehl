/*
 * Copyright 2012 Gustavo Sverzut Barbieri <barbieri@profusion.mobi>
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "Esskyuehl.h"
#include <Ecore.h>

struct ctx {
   unsigned int conns;
   unsigned int errors;
   unsigned int res;
};
#define INSERTED_ROWS 10

static void
_assert(Eina_Bool expr, const char* file, int line)
{
   if (!expr) EINA_LOG_ERR("%s:%d ds failed miserably", file, line);
}
#define assert(_expr) _assert(_expr, __FILE__, __LINE__);

static void
on_query_results(Esql_Res *res, void *data)
{
   struct ctx *ctx = data;
   const Esql_Row *row;
   Eina_Iterator *itr;
   const char *cname;
   int i;

   assert(esql_res_rows_count(res) == INSERTED_ROWS);
   assert(esql_res_cols_count(res) == 2);

   ctx->res++;
   printf("results %u: rows=%d, columns=%d\n",
          ctx->res,
          esql_res_rows_count(res),
          esql_res_cols_count(res));

   cname = esql_res_col_name_get(res, 0);
   assert(cname!=NULL);
   assert(strcmp(cname, "i") == 0);

   cname = esql_res_col_name_get(res, 1);
   assert(cname!=NULL);
   assert(strcmp(cname, "s") == 0);

   i = 0;
   itr = esql_res_row_iterator_new(res);
   EINA_ITERATOR_FOREACH(itr, row)
     {
        const Eina_Value *val = esql_row_value_struct_get(row);
        const char *str;
        char buf[100];
        int num;

        assert(eina_value_struct_get(val, "i", &num));
        assert(i == num);

        snprintf(buf, sizeof(buf), "some-text-%10d", i);

        assert(eina_value_struct_get(val, "s", &str));
        assert(str!=NULL);
        assert(strcmp(str, buf) == 0);

        i++;
     }
   eina_iterator_free(itr);

   ecore_main_loop_quit();
}

static void
on_query_populate(Esql_Res *res, void *data)
{
   struct ctx *ctx = data;

   assert(esql_res_rows_count(res) == 0);
   assert(esql_res_cols_count(res) == 0);
   ctx->res++;
   printf("populated %u: %s\n", ctx->res, esql_res_query_get(res));

   if (ctx->res == INSERTED_ROWS + 1)
     {
        Esql *e = esql_res_esql_get(res);
        Esql_Query_Id id = esql_query(e, ctx, "SELECT i, s FROM t");
        assert(id > 0);
        esql_query_callback_set(id, on_query_results);
     }
}

static Eina_Bool
on_connect(void *data, int type __UNUSED__, void *event_info)
{
   struct ctx *ctx = data;
   Esql *e = event_info;
   Esql_Query_Id id;
   int i;

   id = esql_query(e, ctx, "CREATE TABLE t (i INTEGER, s VARCHAR(100))");
   assert(id > 0);
   esql_query_callback_set(id, on_query_populate);

   for (i = 0; i < INSERTED_ROWS; i++)
     {
        char buf[100];
        snprintf(buf, sizeof(buf), "some-text-%10d", i);
        id = esql_query_args(e, ctx, "INSERT INTO t (i, s) VALUES (%d, '%s')",
                             i, buf);
        assert(id > 0);
        esql_query_callback_set(id, on_query_populate);
     }

   ctx->conns++;
   printf("connected %u!\n", ctx->conns);
   return EINA_TRUE;
}

static Eina_Bool
on_error(void *data, int type __UNUSED__, void *event_info)
{
   struct ctx *ctx = data;
   Esql *e = event_info;

   ctx->errors++;
   printf("error %u: %s!\n", ctx->errors, esql_error_get(e));
   return EINA_TRUE;
}

int
main(void)
{
   Esql *e;
   struct ctx ctx = {0, 0, 0};

   ecore_init();
   esql_init();

   eina_log_domain_level_set("esskyuehl", EINA_LOG_LEVEL_DBG);

   e = esql_new(ESQL_TYPE_SQLITE);
   assert(e != NULL);

   ecore_event_handler_add(ESQL_EVENT_CONNECT, on_connect, &ctx);
   ecore_event_handler_add(ESQL_EVENT_ERROR, on_error, &ctx);

   assert(esql_connect(e, ":memory:", NULL, NULL));

   ecore_main_loop_begin();
   esql_disconnect(e);

   esql_shutdown();
   ecore_shutdown();

   assert(ctx.conns == 1);
   assert(ctx.errors == 0);
   assert(ctx.res == 2 + INSERTED_ROWS);

   return 0;
}
