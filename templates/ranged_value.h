#ifndef TOOLS_RANGED_VALUE_H
#define TOOLS_RANGED_VALUE_H

#include <iostream>

namespace tools
{

template <typename T>
class ranged_value
{
	public:

	enum class bounds{upper=1, lower=2, both=3};

			ranged_value(T vmin, T vmax, T vact, bounds l=bounds::both):
				min_v(vmin),
				max_v(vmax),
				current(vact),
				bound(l)
	{
		check();
	}

	T		min() const {return min_v;}
	T		min(const T v) {min_v=v;}

	T		max() const {return max_v;}
	T		max(const T v) {min_v=v;}

	T		get() const {return current;}
	T		set(const T v) 
	{
		current=v; 
		check();
		return *this;
	}
	
	ranged_value& operator+=(const T v)
	{
		this->operator+(v);
		return *this;
	}

	ranged_value& operator-=(const T v)
	{
		this->operator-(v);
		return *this;
	}

	operator T() const
	{
		return current;
	}

	void		operator+(const T v) 
	{
		current+=v;
		check();
	}

	//Prefix
	ranged_value&	operator++() 
	{
		++current;
		check();
		return *this;
	}

	//Prefix
	ranged_value&	operator--() 
	{
		--current;
		check();
		return *this;
	}

	// postfix
	ranged_value	operator++(int)
	{
		ranged_value<T> tmp(*this);
		operator++();
		return tmp;
	}

	// postfix
	ranged_value	operator--(int)
	{
		ranged_value<T> tmp(*this);
		operator--();
		return tmp;
	}

	void		operator-(const T v) 
	{
		current-=v;
		check();
	}


	ranged_value&	operator=(const T v)
	{
		current=v;
		check();
		return *this;
	}

	bool operator>(const ranged_value& o) const
	{
	    	return current > o.current;
	}

	bool operator>(const T v) const
	{
	    	return current > v;
	}

	bool operator<(const ranged_value& o) const
	{
	    	return current < o.current;
	}

	bool operator<(const T v) const
	{
	    	return current < v;
	}

	bool operator>=(const ranged_value& o) const
	{
	    	return current >= o.current;
	}

	bool operator>=(const T v) const
	{
	    	return current >= v;
	}

	bool operator<=(const ranged_value& o) const
	{
	    	return current <= o.current;
	}

	bool operator<=(const T v) const
	{
	    	return current <= v;
	}
 
	private:

	//Se pueden usar currentores máximos menores que el mínimo y viceversa para
	//no limitar por algún lado.
	void		check()
	{
		if(current > max_v && (bound == bounds::upper || bound == bounds::both)) 
		{
			current=max_v;
		}
		else if(current < min_v && (bound == bounds::lower || bound == bounds::both)) 
		{
			current=min_v;
		}
	}

	T		min_v,
			max_v,
			current;
	bounds		bound;
};
}

#endif
