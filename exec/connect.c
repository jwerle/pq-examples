
#include <stdio.h>
#include <stdlib.h>
#include <libpq-fe.h>

static int
pq_connect (const char *hostaddr, const char *port, const char *dbname) {

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
  return 0;
}

int
main (int argc, char **argv) {
  return pq_connect("127.0.0.1", "5432", "test");
}

