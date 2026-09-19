#pragma once

namespace tools {
namespace tween {
namespace interpolators {

//!Describes an interpolation function to be given to a tweener.
struct interpolating_function_interface {

	virtual double  update(double)=0; //given a time [0,1] returns the amount of value that changes.
};

}}} //End of namespaces
