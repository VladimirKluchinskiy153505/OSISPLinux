#!/bin/bash

# Trap interrupt signal (Ctrl+C) and execute cleanup function
cleanup() {
    # Reset terminal to normal state
    stty echo    # Restore echo
    tput cnorm   # Restore cursor
    tput sgr0    # Reset terminal text attributes
    clear        # Clear screen
    exit 0       # Exit script
}
trap cleanup SIGINT

# Function to generate random numbers
generate_random() {
    # Initialize random number generator based on current time
    RANDOM=$$$(date +%s)
}

# Function to print current time at given coordinates
print_current_time1() {
    tput cup $1 $2                # Move cursor to specified coordinates
    current_time=$(date +"%T")    # Get current time
    echo -n $current_time         # Print time without newline
}
print_current_time() {
    tput cup $1 $2                         # Move cursor to specified coordinates
    echo -e "\033[1;31m$(date +"%T")"     # Print time in red color
    #tput sgr0                              # Reset text attributes
}
# Clear the screen before starting
clear

# Disable cursor
tput civis

# Disable input
stty -echo

# Main program loop
seconds=4                       # Number of seconds to display time
while true; do
    generate_random                # Generate random number
    rows=$(tput lines)          # Get number of rows in terminal
    cols=$(tput cols)           # Get number of columns in terminal
    row=$((RANDOM % (rows-3) + 1))  # Random row (to keep text within terminal boundaries)
    col=$((RANDOM % (cols-10) + 1))  # Random col (to keep text within terminal boundaries)

    clear                           # Clear the screen before printing new position
    for ((i = 0; i < seconds; i++)); do
        print_current_time $row $col   # Print current time at random coordinates
        sleep 1                         # Sleep for 1 second
    done
done