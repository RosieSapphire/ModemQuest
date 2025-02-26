#!/bin/bash

ROM=modemquest.z64

clear
make format
case "$1" in
	"clean")
		make clean -j
		;;

	"rebuild")
		make clean -j
		bear -- make -j
		;;
	
	"run")
		bear -- make -j && UNFLoader -r $ROM
		;;

	"debug")
		bear -- make -j && UNFLoader -r $ROM -d
		;;

	*)
		bear -- make -j
		;;
esac
