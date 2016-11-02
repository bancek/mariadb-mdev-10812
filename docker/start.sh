#!/bin/bash

mysqld --defaults-file=/etc/mysql/my.cnf >/dev/null 2>&1 &

while [[ ! -S /var/run/mysqld/mysqld.sock ]]; do
  sleep 1
done

mysqladmin -u root password "root"

echo 'update user set Host = "%" where User = "root" limit 1;' | mysql -u root --password="root" mysql

killall -w -s SIGTERM mysqld

rsyslogd

sleep 3

tail -f /var/log/syslog &

if [ "x$GALERA_CLUSTER" == "x" ]; then\
  echo "Bootstraping new Galera cluster"
  /usr/bin/mysqld_safe --wsrep_new_cluster
else
  echo "Joining Galera cluster $GALERA_CLUSTER"
  /usr/bin/mysqld_safe --wsrep_cluster_address=gcomm://$GALERA_CLUSTER
fi
