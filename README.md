# mariadb-mdev-10812

Test case for MariaDB issue MDEV-10812 https://jira.mariadb.org/browse/MDEV-10812

# Run

You will need four terminals to run this test.

## Build Docker image

```
cd docker
docker build -t maria-galera-test .
```

## Run Galera cluster

```
docker run --rm -i -t -p 13306:3306 --name=maria-galera-test-1 maria-galera-test
docker run --rm -i -t -p 23306:3306 --link=maria-galera-test-1 --name=maria-galera-test-2 --env=GALERA_CLUSTER=maria-galera-test-1 maria-galera-test
```

## Prepare test database

```
mysql -h 127.0.0.1 -P 13306 -uroot -proot
```

```
CREATE DATABASE `crashtest`;

CREATE TABLE `crashtest`.`test` (
  `id` BIGINT NOT NULL PRIMARY KEY,
  `name` VARCHAR(255) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET utf8 COLLATE utf8_unicode_ci;

INSERT INTO `crashtest`.`test` VALUES (1, 'value');
```

## Build test client

```
gcc -o mysqltest -I/usr/include/mysql mysqltest.c -lmysqlclient
```

## Run two test clients

```
./mysqltest 127.0.0.1 13306 root root
./mysqltest 127.0.0.1 23306 root root
```

## Result

One of the clients will start writing error:

```
Statement execute failed: MySQL server has gone away
COMMIT failed MySQL server has gone away
START TRANSACTION failed
```
