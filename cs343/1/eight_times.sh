#!/bin/bash
input="q2commands"
while read -r line
do
    echo "$line"
    for i in {1..4}
    do
        echo $(/usr/bin/time -f "%Uu %Ss %E" $line) | tail -1
    done
    echo "-----------------------------------------------"
done < "$input"
