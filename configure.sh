#!/bin/bash

while true; do
	echo -n "Home directory (current $(pwd), must have trailing slash): "
	read home_dir;

        last_char="${home_dir: -1}";
        if [ "$last_char" != "/" ]; then
                echo "ERROR: Home directory must end with a slash"
        else            
                if [ -d "$home_dir" ]; then
                        break;
                else 
                        echo "ERROR: Directory $home_dir does not exist"
                fi;
        fi;
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

makefile_name='makefile';
cp make/linux.template ./$makefile_name;

sed -i -e "s^__TEMPLATE_DIR_HOME__^DIR_HOME=$home_dir^g" ./makefile_name;
sed -i -e "s/__TEMPLATE_OPTIMIZATION__/$optimizations/g" ./makefile_name;
sed -i -e "s/__TEMPLATE_DEBUG__/$debug/g" ./makefile_name;

while true; do
	echo -n "Begin compilation (y/n)?: "
	read begin_compilation
	case begin_compilation in
		[y]* ) make clean; make all; break;;
		[n]* ) break;;
	esac;
done;

echo "Done";
