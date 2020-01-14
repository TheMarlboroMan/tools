#pragma once

#include <iostream>

namespace tools
{

//!Represents an scalar value with a limited range. All arithmetic operations 
//!done upon it will respect the bounds.

//!TODO: So far only addition and substraction are implemented.
template <typename T>
class ranged_value
{
	public:

	//!Defines the type of boundaries available...can be only upper, lower or both.
	enum class bounds{upper=1, lower=2, both=3};

	//!Creates the instance with the minimum, maximum and current values. The
	//!bounds parameter is used to specify if upper, lower of both bounds
	//!are desired.
			ranged_value(T vmin, T vmax, T vact, bounds l=bounds::both):
				min_v(vmin),
				max_v(vmax),
				current(vact),
				bound(l) {
		check();
	}

	//!Returns the lower bound.
	T		min() const {return min_v;}

	//!Sets the lower bound.
	T		min(const T v) {min_v=v;}

	//!Returns the upper bound.
	T		max() const {return max_v;}

	//!Sets the upper bound.
	T		max(const T v) {min_v=v;}

	//!Returns the current value.
	T		get() const {return current;}

	//!Sets the current value.
	T		set(const T v) {
		current=v; 
		check();
		return *this;
	}

	//!Adds to the current value.
	ranged_value& operator+=(const T v) {
		this->operator+(v);
		return *this;
	}

	//!Substracts from the current value.
	ranged_value& operator-=(const T v) {
		this->operator-(v);
		return *this;
	}

	//!Returns the current value.
	operator T() const {
		return current;
	}

	//!Adds to the current value.
	void		operator+(const T v) {
		current+=v;
		check();
	}

	//!Prefix increment operator.
	ranged_value&	operator++() {
		++current;
		check();
		return *this;
	}

	//!Prefix decrement operator.
	ranged_value&	operator--() {
		--current;
		check();
		return *this;
	}

	//!Postfix increment operator.
	ranged_value	operator++(int) {
		ranged_value<T> tmp(*this);
		operator++();
		return tmp;
	}

	//!Postfix decrement operator.
	ranged_value	operator--(int) {
		ranged_value<T> tmp(*this);
		operator--();
		return tmp;
	}

	//!Substracts from the value.
	void		operator-(const T v) {
		current-=v;
		check();
	}

	//!Assigns a new current value.
	ranged_value&	operator=(const T v) {
		current=v;
		check();
		return *this;
	}

	//!Larger than comparison (against current value).
	bool operator>(const ranged_value& o) const {
		return current > o.current;
	}

	//!Larger than comparison (against current value).
	bool operator>(const T v) const {
		return current > v;
	}

	//!Less than comparison (against current value).
	bool operator<(const ranged_value& o) const {
		return current < o.current;
	}

	//!Less than comparison (against current value).
	bool operator<(const T v) const {
		return current < v;
	}

	//!Larger or equal than comparison (against current value).
	bool operator>=(const ranged_value& o) const {
		return current >= o.current;
	}

	//!Larger or equal than comparison (against current value).
	bool operator>=(const T v) const {
		return current >= v;
	}

	//!Less or equal than comparison (against current value).
	bool operator<=(const ranged_value& o) const {
		return current <= o.current;
	}

	//!Less or equal than comparison (against current value).
	bool operator<=(const T v) const {
		return current <= v;
	}

	private:

	//!Checks that the value is within the stablished bounds.
	void		check()
	{
		if(current > max_v && (bound == bounds::upper || bound == bounds::both)) {
			current=max_v;
		}
		else if(current < min_v && (bound == bounds::lower || bound == bounds::both)) {
			current=min_v;
		}
	}

	T		min_v,		//!< Lower bound.
			max_v,		//!< Upper bound.
			current;	//!< Current value.
	bounds		bound;		//!< Type of bound.
};
}

