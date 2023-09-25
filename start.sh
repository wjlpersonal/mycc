#! /bin/zsh

for file in find ./test/*/valid/*.c
do
	echo $file
	cat $file
	./mycc $file
	echo
done

for file in find ./test/stage_6/valid/*/*.c
do
	echo $file
	cat $file
	./mycc $file
	echo
done
