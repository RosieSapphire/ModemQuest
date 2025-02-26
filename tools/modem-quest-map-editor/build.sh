#!/bin/bash

PROG=mqme

clear
make format -j
case "$1" in
	"clean")
		make clean -j
		;;

	"rebuild")
		make clean -j
		bear -- make -j
		;;
	
	"run")
		bear -- make -j && ./$PROG $2
		;;

	"debug")
		bear -- make -j && gdb --args ./$PROG $2
		;;

	*)
		bear -- make -j
		;;
esac
