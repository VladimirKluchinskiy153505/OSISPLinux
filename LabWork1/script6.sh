#!/bin/bash
check_exit_status(){
    if [ $? -ne 0]
    then
        echo "An error occured"
    fi
}
echo "You have entered arguments $0, $1, $2, $3, $4, $5, $6"