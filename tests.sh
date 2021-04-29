#!/bin/bash

echo "Checking file: abc.txt."
expec="$(sha512sum abc.txt)"
myout="$(./sha512 abc.txt)  abc.txt"
echo "Expected: " $expec
echo "Output:   " $myout
if [[ $myout == $expec ]]; then
    echo "Test Passed"
else
    echo "Test Failed"
fi

echo "Checking file: empty.txt."
expec="$(sha512sum empty.txt)"
myout="$(./sha512 empty.txt)  empty.txt"
echo "Expected: " $expec
echo "Output:   " $myout
if [[ $myout == $expec ]]; then
    echo "Test Passed"
else
    echo "Test Failed"
fi