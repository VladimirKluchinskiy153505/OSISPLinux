#!/bin/bash

# Trap interrupt signal (Ctrl+C) and execute cleanup function
cleanup() {
    # Reset terminal to normal state
    stty echo       # Restore echo
    tput cnorm      # Restore cursor
    tput sgr0       # Reset terminal text attributes
    clear           # Clear screen
    exit 0          # Exit script
}
trap cleanup SIGINT

RANDOM=1
Multiplier=12791
A=27259
Modulus=43717
# Function to generate random numbers
generate_random() {
    next=$(((RANDOM*Multiplier+A) % Modulus));
}
# Function to print current time at given coordinates with specified color
print_current_time() {
    tput cup $1 $2                                   # Move cursor to specified coordinates
    current_time=$(date +"%T")                       # Get current time
    figlet -f small -c $current_time            # Print time with specified font size using figlet
    tput sgr0                              # Reset text attributes
}

# Function to generate random color code
generate_color() {
    colors=("30" "31" "32" "33" "34" "35" "36" "37")  # Array of ANSI color codes
    rand_index=$((RANDOM % ${#colors[@]}))  # Get random index
    echo -e "\033[1;${colors[$rand_index]}m" # Return random color code
}

# Clear the screen before starting
clear

# Disable cursor
tput civis

# Disable input
stty -echo

# Main program loop
seconds_interval=3                        # Number of seconds to display time
current_color=$(generate_color)     # Generate initial random color

while true; do
    generate_random                 # Generate random number
    rows=$(tput lines)              # Get number of rows in terminal
    cols=$(tput cols)               # Get number of columns in terminal
    row=$((RANDOM % (rows-3) + 1))  # Random row (to keep text within terminal boundaries)
    col=$((RANDOM % (cols-10) + 1)) # Random col (to keep text within terminal boundaries)

    clear                            # Clear the screen before printing new position
    for ((i = 0; i < seconds_interval; i++)); do
        print_current_time $row $col $current_color   # Print current time at random coordinates with current color
        sleep 1                                      # Sleep for 1 second
    done

    current_color=$(generate_color)  # Generate new random color
done