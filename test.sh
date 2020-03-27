#!/bin/bash

files="input_file1 input_file2 input3 input4 input5"

make all

for file in $files
do
    echo "run assem on $file"
    ./assem $file > Test/$file.out
    diff Test/$file.out Test/$file.expected  
done

echo "I am done! "
