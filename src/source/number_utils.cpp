#include "number_utils.h"

using namespace tools;

size_t tools::count_digits(int pint) {

	if(pint==0) return 1;

	int copy=pint;
	int result=0;

	if(copy < 0) {
		copy=-copy;
	}
		
	while(copy > 0) {
		result++;
		copy/=10;
	}

	return result;
}
