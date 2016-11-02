FROM ubuntu:14.04

RUN apt-get update && apt-get -yq install software-properties-common

RUN apt-key adv --recv-keys --keyserver hkp://keyserver.ubuntu.com:80 0xcbcb082a1bb943db \
  && add-apt-repository 'deb ftp://ftp.ulak.net.tr/pub/MariaDB/repo/10.0/ubuntu trusty main' \
  && apt-get update \
  && DEBIAN_FRONTEND=noninteractive apt-get -yq install mariadb-galera-server \
  && sed -i "s/^bind-address.*$/bind-address=0.0.0.0/g" /etc/mysql/my.cnf

ADD galera.cnf /etc/mysql/conf.d/galera.cnf

RUN chmod 600 /etc/mysql/conf.d/galera.cnf

ADD start.sh /start.sh

RUN chmod +x /start.sh

CMD ["/start.sh"]

EXPOSE 3306 4444 4567 4568
