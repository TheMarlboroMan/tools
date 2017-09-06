#include "dnot_token.h"

#include "../templates/compatibility_patches.h"

using namespace tools;

bool dnot_token::is_value() const 
{
	return type==types::tstring||
	type==types::tint ||
	type==types::tfloat||
	type==types::tbool ||
	type==types::tdouble;
}

void dnot_token::set(const dnot_token& tok)
{
	type=tok.type;
	tokens=tok.tokens;
	vector=tok.vector;
	string_value=tok.string_value;
	int_value=tok.int_value;
	float_value=tok.float_value;
	double_value=tok.double_value;
	bool_value=tok.bool_value;
}

void dnot_token::set(const std::string& c)
{
	string_value=c;
	type=types::tstring;
}

void dnot_token::set(const char * c)
{
	string_value=c;
	type=types::tstring;
}

void dnot_token::set(int c)
{
	int_value=c;
	type=types::tint;
}

void dnot_token::set(float c)
{
	float_value=c;
	type=types::tfloat;
}

void dnot_token::set(double c)
{
	double_value=c;
	type=types::tdouble;
}

void dnot_token::set(bool c)
{
	bool_value=c;
	type=types::tbool;
}

void dnot_token::set(const t_map& t)
{
	tokens=t;
	type=types::tmap;
}

void dnot_token::set(const t_vector& t)
{
	vector=t;
	type=types::tvector;
}

dnot_token::dnot_token()
	:type(types::tstring),
	string_value(""), int_value(0), float_value(0.f), double_value(0.0), bool_value(false)
{

}

dnot_token::dnot_token(const std::string& v)
	:type(types::tstring), 
	string_value(v), int_value(0), float_value(0.f), double_value(0.0), bool_value(false)
{

}

dnot_token::dnot_token(const char * v)
	:type(types::tstring), 
	string_value(v), int_value(0), float_value(0.f), double_value(0.0), bool_value(false)
{

}

dnot_token::dnot_token(const char v)
	:type(types::tstring), 
	string_value(), int_value(0), float_value(0.f), double_value(0.0), bool_value(false)
{
	string_value+=v;
}

dnot_token::dnot_token(int v)
	:type(types::tint),
	string_value(""), int_value(v), float_value(0.f), double_value(0.0), bool_value(false)
{

}

dnot_token::dnot_token(float v)
	:type(types::tfloat),
	string_value(""), int_value(0), float_value(v), double_value(0.0), bool_value(false)
{

}

dnot_token::dnot_token(double v)
	:type(types::tdouble),
	string_value(""), int_value(0), float_value(0.0f), double_value(v), bool_value(false)
{

}

dnot_token::dnot_token(bool v)
	:type(types::tbool),
	string_value(""), int_value(0), float_value(0.f), double_value(0.0), bool_value(v)
{

}

dnot_token::dnot_token(const t_map& v)
	:type(types::tmap),
	string_value(""), int_value(0), float_value(0.f), double_value(0.0), bool_value(false)
{
	set(v);
}

dnot_token::dnot_token(const t_vector& v)
	:type(types::tvector),
	string_value(""), int_value(0), float_value(0.f), double_value(0.0), bool_value(false)
{
	set(v);
}

const dnot_token::t_map& dnot_token::get_map() const 
{
	if(type!=types::tmap) throw std::runtime_error("Not a map type ["+translate_type(type)+" - "+to_string()+"]");
	return tokens;
}

dnot_token::t_map& dnot_token::get_map()
{
	if(type!=types::tmap) throw std::runtime_error("Not a map type ["+translate_type(type)+" - "+to_string()+"]");
	return tokens;
}

const dnot_token::t_vector& dnot_token::get_vector() const 
{
	if(type!=types::tvector) throw std::runtime_error("Not a vector type ["+translate_type(type)+" - "+to_string()+"]");
	return vector;
}

dnot_token::t_vector& dnot_token::get_vector()
{
	if(type!=types::tvector) throw std::runtime_error("Not a vector type ["+translate_type(type)+" - "+to_string()+"]");
	return vector;
}

const std::string& dnot_token::get_string() const 
{
	if(type!=types::tstring) throw std::runtime_error("Not a string type ["+translate_type(type)+" - "+to_string()+"]");
	return string_value;
}

int dnot_token::get_int() const 
{
	if(type!=types::tint) throw std::runtime_error("Not an int type ["+translate_type(type)+" - "+to_string()+"]");
	return int_value;
}

float dnot_token::get_float() const 
{
	if(type!=types::tfloat) throw std::runtime_error("Not a float type ["+translate_type(type)+" - "+to_string()+"]");
	return float_value;
}

double dnot_token::get_double() const 
{
	if(type!=types::tdouble) throw std::runtime_error("Not a double type ["+translate_type(type)+" - "+to_string()+"]");
	return double_value;
}

bool dnot_token::get_bool() const 
{
	if(type!=types::tbool) throw std::runtime_error("Not a bool type ["+translate_type(type)+" - "+to_string()+"]");
	return bool_value;
}

const dnot_token& dnot_token::operator[](const std::string& k) const
{
	if(type!=types::tmap) throw std::runtime_error("Not a map type ["+translate_type(type)+" - "+to_string()+"]");
	else if(!tokens.count(k)) throw std::runtime_error("Unable to find key '"+k+"' in map");
	else return tokens.at(k);
}

dnot_token& dnot_token::operator[](const std::string& k)
{
	if(type!=types::tmap) throw std::runtime_error("Not a map type ["+translate_type(type)+" - "+to_string()+"]");
	else if(!tokens.count(k)) throw std::runtime_error("Unable to find key '"+k+"' in map");
	else return tokens[k];
}

const dnot_token& dnot_token::operator[](const char * k) const
{
	return this->operator[](std::string(k));
}

dnot_token& dnot_token::operator[](const char * k)
{
	return this->operator[](std::string(k));
}

const dnot_token& dnot_token::operator[](size_t l) const
{
	if(type!=types::tvector) throw std::runtime_error("Not a vector type ["+translate_type(type)+" - "+to_string()+"]");
	else if(l >= vector.size()) throw std::runtime_error("Invalid vector index");
	else return vector.at(l);
}

dnot_token& dnot_token::operator[](size_t l)
{
	if(type!=types::tvector) throw std::runtime_error("Not a vector type ["+translate_type(type)+" - "+to_string()+"]");
	else if(l >= vector.size()) throw std::runtime_error("Invalid vector index");
	else return vector[l];
}

const dnot_token& dnot_token::operator[](int l) const
{
	return this->operator[]((size_t)l);
}

dnot_token& dnot_token::operator[](int l)
{
	return this->operator[]((size_t)l);
}

std::ostream& tools::operator<<(std::ostream& os, const tools::dnot_token& t)
{
	using namespace tools;

	switch(t.type)
	{
		case dnot_token::types::tmap: os<<"{tmap}"; break;
		case dnot_token::types::tvector: os<<"[vector]"; break;
		case dnot_token::types::tstring: os<<t.string_value; break;
		case dnot_token::types::tint: os<<t.int_value; break;
		case dnot_token::types::tfloat: os<<t.float_value; break;
		case dnot_token::types::tdouble: os<<t.double_value; break;
		case dnot_token::types::tbool: t.bool_value ? os<<"true" : os<<"false"; break;
	}

	return os;
}

bool dnot_token::key_exists(const std::string& k) const
{
	if(type!=types::tmap) throw std::runtime_error("Not a map type ["+translate_type(type)+" - "+to_string()+"]");
	else return tokens.count(k);
}

std::string dnot_token::serialize(const dnot_token_serialize_options& options, int recursive) const
{
	std::string result;

	size_t tot=0, cur=0;

	auto nl=[&result, &options](int rec)
	{
		result+="\n";
		for(int i=0; i<rec; ++i) 
		{
			result+=options.tab_str;
		}
	};


	auto abre=[&result, &nl, &options](types type, int rec)
	{
		if(rec < 1) return;
		result+=type==types::tvector ? "[" : "{";
		if(options.tab_depth) nl(rec);
	};

	auto cierra=[&result, &nl, &options](types type, int rec)
	{
		if(rec < 1) return;
		if(options.tab_depth) nl(rec-1);
		result+=type==types::tvector ? "]" : "}";
	};

	switch(type)
	{
		case types::tvector: 
		case types::tmap:

			cur=0;
			tot=type==types::tvector ? vector.size()-1 : tokens.size()-1;

			abre(type, recursive);
			if(type==types::tvector)
			{
				for(const auto& e : vector) 
				{
					result+=e.serialize(options, recursive+1);
					if(cur++!=tot) result+=", ";
				}
			}
			else if(type==types::tmap)
			{
				for(const auto& e : tokens) 
				{
					result+=e.first+":"+e.second.serialize(options, recursive+1);
					if(cur++!=tot) result+=", ";
				}
			}
			cierra(type, recursive);

		break;
		case types::tstring: 
			result+="\""+string_value+"\"";
		break;
		case types::tint: 
			result+=compat::to_string(int_value);
		break;
		case types::tfloat:
			result+=compat::to_string(float_value)+'f';
		break;
		case types::tdouble:
			result+=compat::to_string(double_value);
		break;
		case types::tbool:
			result+=bool_value ? "true" : "false";
		break;
	}

	return result;
}

std::string dnot_token::translate_type(dnot_token::types t)const
{
	switch(t)
	{
		case types::tmap: return "tmap"; break;
		case types::tstring: return "string"; break;
		case types::tint: return "int"; break;
		case types::tfloat: return "float"; break;
		case types::tdouble: return "double"; break;
		case types::tbool: return "bool"; break;
		case types::tvector:	return "vector"; break;
	}

	//Shut up compiler...
	return std::string();
}

std::string dnot_token::to_string()const
{
	switch(type)
	{
		case types::tmap: return "tmap ("+compat::to_string(tokens.size())+")"; break;
		case types::tstring: return string_value; break;
		case types::tint: return compat::to_string(int_value); break;
		case types::tfloat: return compat::to_string(float_value); break;
		case types::tdouble: return compat::to_string(double_value); break;
		case types::tbool: return bool_value ? "true" : "false"; break;
		case types::tvector:	return "vector ("+compat::to_string(vector.size())+")"; break;
	}

	//Shut up compiler...
	return std::string();
}
