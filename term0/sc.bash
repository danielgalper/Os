#!/bin/bash

fileCurStat=$(stat -f %B "$1")
const=3600*24
dif=($(date +%s) - fileCurStat)
(( age=$dif/$const ))
if [[ ($age -ge 2) && ($age -le 5) ]]
then
    exit 0
fi

exit 1
