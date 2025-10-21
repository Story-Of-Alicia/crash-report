FROM ubuntu
RUN apt-get -y update && apt-get -y install nginx php-fpm

COPY default /etc/nginx/sites-available/default
COPY www.conf /etc/php/8.3/fpm/pool.d

RUN rm -r /var/www/html

EXPOSE 80/tcp

ADD init.sh /
RUN chmod +x /init.sh

RUN mkdir -p /var/www/reports
RUN chmod 744 /var/www

ADD index.php /var/www
RUN chmod +x /var/www/index.php

RUN chown -R www-data:www-data /var/www

CMD ["./init.sh"]
