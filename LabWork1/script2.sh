#!/bin/bash/
mynum=2010
expr 30 + 10
expr 30 \* 4
if [ $mynum -gt 200 ] #-ne -gt
then
    echo "The cond is true"
else
    echo "The var is not equat to 200"
fi

#check presents of file
if [ -f ~/myfile ]
then
    echo "The file exists"
else
    echo "the file doesnt exist"
fi