#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include <mysql.h>

int main(int argc, char **argv) {
  if (argc != 5) {
    puts("usage: mysqltest 127.0.0.1 3306 root root");
    exit(1);
  }

  char *host = argv[1];
  char *port = argv[2];
  char *username = argv[3];
  char *password = argv[4];

  int portNum = atoi(port);

  srand(time(NULL));

  MYSQL *mysql = NULL;

  mysql = mysql_init(mysql);

  if (!mysql) {
    puts("Init faild, out of memory?");
    return EXIT_FAILURE;
  }

  if (!mysql_real_connect(mysql,       /* MYSQL structure to use */
        host,         /* server hostname or IP address */
        username,         /* mysql user */
        password,          /* password */
        NULL,               /* default database to use, NULL for none */
        portNum,           /* port number, 0 for default */
        NULL,        /* socket file or named pipe name */
        0 /* connection flags */ )) {
    puts("Connect failed\n");
  }

  if (mysql_query(mysql, "SET wsrep_causal_reads=1") != 0) {
    puts("wsrep_set failed");
    exit(1);
  }

  while (1) {
    if (mysql_query(mysql, "START TRANSACTION") != 0) {
      puts("START TRANSACTION failed");
    }

    MYSQL_STMT *stmt;

    stmt = mysql_stmt_init(mysql);

    if (!stmt) {
      printf("Statement init failed: %s\n", mysql_error(mysql));

      continue;
    }

    if (stmt) {
      char *query = "UPDATE `crashtest`.`test` SET `name` = ? WHERE `id` = 1";

      if (mysql_stmt_prepare(stmt, query, strlen(query))) {
        printf("Statement prepare failed: %s\n", mysql_stmt_error(stmt));
      }

      int r = rand();
      char buff[255];
      printf("%d\n", r);
      sprintf(buff, "%d", r);
      my_bool is_null = 0;
      unsigned long length = strlen(buff);
      my_bool err = 0;

      MYSQL_BIND bind[1];
      bind[0].buffer_type = MYSQL_TYPE_STRING;
      bind[0].buffer = (char *)buff;
      bind[0].buffer_length = 255;
      bind[0].is_null= &is_null;
      bind[0].length= &length;
      bind[0].error= &err;

      mysql_stmt_bind_param(stmt, bind);

      if (mysql_stmt_execute(stmt)) {
          printf("Statement execute failed: %s\n", mysql_stmt_error(stmt));
      }

      mysql_stmt_free_result(stmt);

      int res;

      res = mysql_stmt_close(stmt);

      if (res != 0) {
        printf("mysql_stmt_close error %d, %s\n", res, mysql_error(mysql));
      }
    }

    if (mysql_query(mysql, "COMMIT") != 0) {
      printf("COMMIT failed %s\n", mysql_error(mysql));
    }
  }

  mysql_close(mysql);

  return 0;
}
