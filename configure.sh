#!/bin/bash

makefile_name='makefile';
__retval="";

#params: prompt value_y value_n 
#return code:0 on y and 1 on no.
#reference return: uses __retval
function y_n_choice() {

	local read_value;
	local res;
	while true;
	do
		echo -n "$1 (y/n): ";
		read read_value;

		y_n_to_value $read_value $2 $3;
		if [ "-1" != $__retval ]; then
			break
		fi;
	done;
}

#params: value to be evaluated, value to be returned if the "y", value to
#be returned if "n". 
#return code: none
#reference return: uses __retval
function y_n_to_value() {

	case $1 in
		"y") __retval=$2;;
		"n") __retval=$3;;
		*) __retval=-1; 
	esac;
}

function help() {

	echo "./configure [optimizations] [debug] [cpp14] [debug-code] [assertions] [compile y/n]"
	echo "ex. ./configure y n n n n y"
}

optimizations_y="OPTIMIZATION=-O2"
optimizations_n="#OPTIMIZATION=-O2"
debug_y="DEBUG=-g"
debug_n="#DEBUG=-g"
cpp14_y="CPPREV=-std=c++14" 
cpp14_n="CPPREV=-std=c++11"
debug_code_y="WITH_DEBUG_CODE=-DWITH_DEBUG_CODE"
debug_code_n="#WITH_DEBUG_CODE=-DWITH_DEBUG_CODE"
disable_assert_y="#DISABLE_ASSERT=-DNDEBUG=1"
disable_assert_n="DISABLE_ASSERT=-DNDEBUG=1";

if [ "$#" -ne 0 ]; then

	if [ $1 == "-h" ] || [ "$#" -ne 6 ]; then 
		help;
		exit 1;
	fi;

	y_n_to_value $1 $optimizations_y $optimizations_n
	optimizations=$__retval
	
	y_n_to_value $2 $debug_y $debug_n
	debug=$__retval
	
	y_n_to_value $3 $cpp14_y $cpp14_n
	cpprev=$__retval

	y_n_to_value $4 $debug_code_y $debug_code_n
	debug_code=$__retval

	y_n_to_value $5 $disable_assert_y $disable_assert_n
	disable_assert=$__retval

	y_n_to_value $6 "1" "0";
	compile=$__retval;

	if [ "-1" == $optimizations ] || [ "-1" == $debug ] || [ "-1" == $cpprev ] || [ "-1" == $debug_code ] || [ "-1" == $disable_assert ] || [ "-1" == $compile ] ; then
		echo "invalid parameters"
		exit 2;
	fi;

else

	echo "Use -h for help on building without human interaction";

	y_n_choice "With optimizations" $optimizations_y $optimizations_n
	optimizations=$__retval;

	y_n_choice "With debug" $debug_y $debug_n
	debug=$__retval;

	y_n_choice "With C++14" $cpp14_y $cpp14_n
	cpprev=$__retval

	y_n_choice "With debug code" $debug_code_y $debug_code_n
	debug_code=$__retval;

	y_n_choice "With assertions" $disable_assert_y $disable_assert_n
	disable_assert=$__retval;

	y_n_choice "Compile?" "1" "0"
	compile=$__retval;
fi;

cp make/linux.template ./$makefile_name;

sed -i -e "s/__TEMPLATE_OPTIMIZATION__/$optimizations/g" ./$makefile_name;
sed -i -e "s/__TEMPLATE_DEBUG__/$debug/g" ./$makefile_name;
sed -i -e "s/__TEMPLATE_WITH_DEBUG_CODE__/$debug_code/g" ./$makefile_name;
sed -i -e "s/__TEMPLATE_DISABLE_ASSERT__/$disable_assert/g" ./$makefile_name;
sed -i -e "s/__TEMPLATE_CPPREV__/$cpprev/g" ./$makefile_name;

if [ 1 == $compile ]; then 
	make clean; 
	make all; 
	echo "Done";
fi;

exit 0;

