#!/bin/sh

make

./${@} # EXEC WITH ALL COMMANDS LINE ARGUMENTS 

rm /dev/shm/cahier /dev/shm/resto 2> /dev/null