#!/bin/bash

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
		bear -- make -j && ./mqme $2
		;;

	"debug")
		bear -- make -j && gdb --args ./mqme $2
		;;

	*)
		bear -- make -j
		;;
esac
