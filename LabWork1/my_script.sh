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

# Function to print clock at the current position
print_clock() {
    while true; do
        tput sc                           # Save cursor position
        tput cup $row $col                # Move cursor to the previous position
        current_time=$(date +"%T")        # Get current time
        printf "%-8s" "$current_time"     # Print time without moving cursor
        tput rc                           # Restore saved cursor position
        sleep 1                           # Update time every second
    done
}

# Function to print clock at random location on screen
print_clock &
clock_pid=$!  # Store the process ID of the clock update process

# Main program loop
while true; do
    generate_random  # Generate random number
    rows=$(tput lines)          # Get number of rows in terminal
    cols=$(tput cols)           # Get number of columns in terminal
    row=$((RANDOM % (rows-3) + 1))  # Random row (to keep text within terminal boundaries)
    col=$((RANDOM % (cols-10) + 1))  # Random col (to keep text within terminal boundaries)
    clear                        # Clear screen
    tput cup $row $col           # Move cursor to random position
    figlet -c $(date +"%T")      # Print time using figlet
    sleep 5                     # Regenerate image every 10 seconds
done

# Wait for the clock update process to finish before exiting
wait $clock_pid