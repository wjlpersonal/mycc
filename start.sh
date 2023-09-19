#! /bin/zsh

for file in find ./test/*/valid/*.c
do
	echo $file
	cat $file
	./mycc $file
	echo
done
