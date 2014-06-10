
#include <stdio.h>
#include <stdlib.h>
#include <libpq-fe.h>

static int
pq_query (const char *hostaddr, const char *port, const char *dbname, const char *query) {

  // opts
  const char *keys[] = {
    "hostaddr", "port", "dbname", NULL
  };

  const char *values[] = {
    hostaddr, port, dbname, NULL
  };

  // connect
  PGconn *conn = PQconnectdbParams(keys, values, 0);
  if (NULL == conn) { return 1; }

  // status
  ConnStatusType status = PQstatus(conn);

  // ensure okay
  if (CONNECTION_BAD == status) {
    fprintf(stderr, "error: %s\n", PQerrorMessage(conn));
    PQfinish(conn);
    return 1;
  }

  // query
  PGresult *res = PQexec(conn, query);

  switch (PQresultStatus(res)) {
    case PGRES_EMPTY_QUERY:
    case PGRES_COMMAND_OK:
    case PGRES_COPY_OUT:
    case PGRES_COPY_IN:
    case PGRES_COPY_BOTH:
      break;

    case PGRES_NONFATAL_ERROR:
    case PGRES_BAD_RESPONSE:
    case PGRES_FATAL_ERROR:
      fprintf(stderr, "error: %s\n", PQerrorMessage(conn));
      if (NULL == res) {
        PQfinish(conn);
        return 1;
      }
      break;

    case PGRES_SINGLE_TUPLE:
    case PGRES_TUPLES_OK:
      {
        int i = 0; // tuple
        int j = 0; // row
        int ntuples = PQntuples(res);
        int nfields = PQnfields(res);

        printf("got %d tuples with %d fields\n",
            ntuples, nfields);

        for (; i < ntuples; ++i) {
          for (j = 0 ; j < nfields; ++j) {
            printf("  >column: %s\n", PQfname(res, j));
            printf("    >value: %s\n", PQgetvalue(res, i, j));
          }
        }

      }

      break;
  }

  // clear query
  PQclear(res);

  // close connection
  PQfinish(conn);
  return 0;
}

int
main (void) {
  return pq_query("127.0.0.1", "5432", "test",
      "SELECT * FROM foos");
}
