#! /bin/sh
g++ Kawaii.cpp -o Kawaii -w -Os -s `pkg-config gtk+-3.0 --cflags` `pkg-config gtk+-3.0 --libs`
if [ $? == 0 ]
then
	./Kawaii
fi

