#!/bin/bash

while true; do
	echo -n "Home directory (with ending slash): "
	read home_dir;

	if [ -d "$home_dir" ]; then
		break;
	fi;

	echo "Directory $home_dir does not exist"
done;

while true; do
	echo -n "With optimizations (y/n): "
	read with_optimizations;
	case $with_optimizations in
		[y]* ) optimizations="OPTIMIZATION=-O2"; break;;
		[n]* ) optimizations="#OPTIMIZATION=-O2"; break;;
	esac
done;

while true; do
	echo -n "With debug (y/n): "
	read with_debug;
	case $with_debug in
		[y] ) debug="DEBUG=-g"; break;;
		[n] ) debug="#DEBUG=-g"; break;;
	esac;
done;

cp make/linux.template ./makefile_linux;

sed -i -e "s^__TEMPLATE_DIR_HOME__^DIR_HOME=$home_dir^g" ./makefile_linux;
sed -i -e "s/__TEMPLATE_OPTIMIZATION__/$optimizations/g" ./makefile_linux;
sed -i -e "s/__TEMPLATE_DEBUG__/$debug/g" ./makefile_linux;

echo "Done";
