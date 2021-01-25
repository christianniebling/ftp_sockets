#!/usr/bin/bash
n=1
last=11
while [ $n -lt $last ]
do
 ./TigerC <<< "tconnect 129.21.154.29 user pass
tget down$n.txt
tput upload$n.txt
exit" 
 n=$(($n+1))
done
