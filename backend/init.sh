#!/bin/bash

/usr/sbin/php-fpm8.3 -D && /usr/sbin/nginx -g 'daemon off;'

