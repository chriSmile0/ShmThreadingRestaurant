#!/bin/sh
rm /dev/shm/cahier /dev/shm/resto 2> /dev/null
kill -s kill `ps -u | grep -e ./convive | awk '{print $2}'` 2> /dev/null
kill -s kill `ps -u | grep -e ./restaurant | awk '{print $2}'` 2> /dev/null