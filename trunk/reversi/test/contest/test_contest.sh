#!/bin/bash

#Test each test file, the test is correct if the printed result is the same as the move at the end of the file name

#Configuration are from "Othello: A minute to learn... a lifetime to master Brian Rose" p15

for file in `ls -I "*.sh"`
do
		echo -n $file " "
		./../../bin/reversi -c $file
done


