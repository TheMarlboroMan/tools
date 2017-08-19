#include "dnot_config_file.h"
#include <map>
#include "../source/string_utils.h"

using namespace tools;

dnot_config_file::dnot_config_file(const std::string& ppath)
try
	:token(tools::dnot_parse(ppath)),
	path(ppath)
{

}
catch(std::runtime_error& e)
{
	throw std::runtime_error(std::string("dnot_config_file: error starting configuration ")+ppath+" : "+e.what());
}

void dnot_config_file::load(const std::string& p)
{
	try
	{
		token=tools::dnot_parse(p);
		path=p;
	}
	catch(std::runtime_error& e)
	{
		throw std::runtime_error(std::string("dnot_config_file: error loading configuration ")+p+" : "+e.what());
	} 
}

void dnot_config_file::reload()
{
	try
	{
		token=tools::dnot_parse(path);
	}
	catch(std::runtime_error& e)
	{
		throw std::runtime_error(std::string("dnot_config_file: error reloading configuration ")+path+" : "+e.what());
	} 
}

void dnot_config_file::save()
{
	dnot_token_serialize_options os;
	os.tab_depth=true;
	std::ofstream f(path);
	f<<token.serialize(os);
}

/**
* Localiza un token nombrado siguiendo una cadena sencilla: config:video:tam_pantalla
* localizaría el token en el objeto "config" de la raiz, seguido del objeto 
* "video" contenido en "config" y "tam_pantalla" contenido en "video".
*/

const tools::dnot_token& dnot_config_file::token_from_path(const std::string& c) const
{
	const dnot_token * p=&token;
	auto v=explode(c, ':');
	for(const auto& clave : v) 
	{
		try
		{
			p=&p->get_map().at(clave);
		}
		catch(std::exception& e)
		{
			throw std::runtime_error("unable to locate key "+clave+" in file "+c);
		}
	}
	return *p;
}

tools::dnot_token& dnot_config_file::token_from_path(const std::string& c)
{
	dnot_token * p=&token;
	auto v=explode(c, ':');
	for(const auto& clave : v) p=&p->get_map()[clave];
	return *p;
}
