#!bin/bash/
base_regex="([a-zA-Z0-9_\-\.\+]+)@([a-zA-Z0-9_\-\.]+)\."
standart_suffix="([a-zA-Z]{2,5})"
echo "To find emails with a specific suffix press 1"
echo "To find all emais press 2"
echo "To end script press 0"
read variant

if [ $variant -eq 0 ]
then
    exit 0
fi

if [ -z "$1" ]; then
    echo "Enter input file name"
    read input_file
else
    input_file=$1
fi

if [ ! -f "$input_file" ]; then
    echo "File not found!"
    exit 1
fi

if [ -z "$1" ]; then
    echo "Enter output file name:"
    read output_file
else
    output_file=$2
fi

if [ ! -f "$output_file" ]; then
    echo "File not found!"
    exit 1
fi

if [ $variant -eq 1 ] 
then
    echo "Enter suffix"
    read suffix
    if [[ $suffix =~ [.,] ]]
    then
        echo "Error! Suffix cant contain {.,}"
        exit 1
    else
        grep -E -o "$base_regex$suffix" $input_file > $output_file
    fi
else
    grep -E -o "$base_regex$standart_suffix" $input_file > $output_file
fi
echo "Successfully written to file $output_file:"
cat $output_file