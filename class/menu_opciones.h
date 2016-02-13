#ifndef MENU_OPCIONES_HERRAMIENTAS_PROYECTO
#define MENU_OPCIONES_HERRAMIENTAS_PROYECTO

#include <iostream>

#include <string>
#include <map>
#include <vector>
#include <exception>
#include <algorithm>
#include <memory>

#include "valor_limitado.h"

#ifdef WINCOMPIL
//Localización del parche mingw32... Esto debería estar en otro lado, supongo.
#include <herramientas/herramientas/herramientas.h>
#endif

/*
La representación interna de un menú de opciones de un único nivel en el que
cada opción puede tener N valores que van "rotando", por ejemplo:

menu:
	resolución de pantalla : [800x600], [1000x1000]
	volumen_audio : [Sin sonido, Medio, Alto]
	idioma: [Español, Inglés, Francés, Japonés]
	cantidad: [un número entre 1 y 99]

El menu sería el primer nivel. El segundo nivel "resolución de pantalla",
"volumen_audio" e "idioma" serían "opciones" y el último nivel serían 
"selecciones"

Cada opción y selección están identificados por una clave que no debe repetirse.
En caso de que se repita tendremos una excepción. Ojo, no debe repetirse en 
general, no sólo entre opciones y selecciones!!!.

A la hora de insertar tanto una opción como una clave se ordenarán por el tipo 
Tclave especificado en el template.

Es perfectamente posible tener selecciones dentro de una opción con el mismo 
valor.

Va acompañado de un método "traducir" que hace una pasada simple por todos los
"nombres" para reemplazarlos por otro, de forma que podamos hacer menús 
en múltiples idiomas.

Al final de este archivo hay un ejemplo de uso.

--== Interface pública ==-------------------------------------------------------

Por lo general todos los métodos lanzan excepción si la clave no existe.

//TODO: Recrear interface.
//TODO: Recrear ejemplo.
//TODO: Crear desde dnot.
*/

namespace Herramientas_proyecto
{

class Menu_opciones_exception:
	public std::runtime_error
{
	public:
	Menu_opciones_exception(const std::string& s):std::runtime_error(s) {}
};

template<typename Tclave>
class Menu_opciones
{
	public:

	struct struct_traduccion
	{
		Tclave busca;
		std::string reemplaza;

		void reemplazar(const Tclave& clave, std::string& cad) const
		{
			if(clave==busca) 
			{
				cad=reemplaza;
			}
		};
	};

	private:

	enum class tipos			{tstring, tint};

	struct Base_seleccion
	{
		std::string 				nombre;

		virtual	std::string		valor_visible() const=0;
		virtual std::string		valor_string() const {return "";}
		virtual int			valor_int() const {return 0;}
		virtual tipos			tipo() const=0;
		virtual void			rotar(int)=0;
		virtual void			traducir(const struct_traduccion& t)=0;
						Base_seleccion(const std::string& n):nombre(n){}
	};

	typedef std::unique_ptr<Base_seleccion>				uptr_base;

	//Opción del menú de tipo string, rotatoria.
	struct Opcion_menu_string:public Base_seleccion
	{
		//El tipo de una selección de menú de tipo string.
		struct Seleccion_menu_string
		{		
			std::string			valor, 
							nombre;
		};

		std::map<Tclave, Seleccion_menu_string>	selecciones;
		Tclave					clave_actual;

		virtual std::string			valor_string() const 
		{
			comprobar_opciones_existen("La opción no tiene selecciones para valor_string");
			return selecciones.at(clave_actual).valor;
		}
		virtual std::string			valor_visible() const 
		{
			comprobar_opciones_existen("La opción no tiene selecciones para valor_visible");
			return selecciones.at(clave_actual).nombre;
		}
		virtual tipos				tipo() const {return tipos::tstring;}

		virtual void 				rotar(int dir)
		{
			auto it=selecciones.find(clave_actual);

			//Descender...
			if(dir < 0)
			{
				if(it==selecciones.begin()) //Dar la vuelta.
				{
					clave_actual=selecciones.rbegin()->first;
				}
				else
				{
					clave_actual=std::prev(it)->first;
				}
			}
			//Ascender...
			else
			{
				auto sigue=std::next(it);
				if(sigue==std::end(selecciones))
				{
					clave_actual=selecciones.begin()->first;
				}
				else
				{
					clave_actual=sigue->first;
				}
			}
		}

		void 					asignar_por_valor(const std::string& valor_seleccion)
		{
			for(auto& seleccion : selecciones)
			{
				if(seleccion.second.valor==valor_seleccion) 
				{
					clave_actual=seleccion.first;
					return;
				}
			}

			throw Menu_opciones_exception("El valor no existe al asignar selección");
		}

		void					seleccionar_opcion(const Tclave& clave_seleccion)
		{
			if(!selecciones.count(clave_seleccion)) 
			{
				throw Menu_opciones_exception("La clave no existe al asignar selección");
			}
			else clave_actual=clave_seleccion;
		}

		size_t				size() const
		{
			return selecciones.size();
		}

		void				comprobar_opciones_existen(const std::string& msj) const
		{
			if(!selecciones.size()) throw Menu_opciones_exception(msj);
		}

		void				insertar_seleccion(const Tclave& clave, const std::string& valor, const std::string& nombre)
		{
			if(!selecciones.size()) clave_actual=clave;
			selecciones[clave]={valor, nombre};
		}

		void 				eliminar_seleccion(const Tclave& clave_seleccion)
		{
			if(!selecciones.count(clave_seleccion)) throw Menu_opciones_exception("La clave no existe en selecciones para eliminar");
			selecciones.erase(clave_seleccion);
		}

		virtual void			traducir(const struct_traduccion& t)
		{
			for(auto& seleccion : selecciones)
			{
				t.reemplazar(seleccion.first, seleccion.second.nombre);
			}
		}
						Opcion_menu_string(const std::string& n):Base_seleccion(n), clave_actual() {}
	};

	//Estructura para la selección de un valor de enteros.
	struct Opcion_menu_int:public Base_seleccion
	{
		Valor_limitado<int>		val;

		virtual std::string		valor_visible() const 
		{
			#ifdef WINCOMPIL
			using namespace parche_mingw;
			#else
			using namespace std;
			#endif

			return to_string(val.actual());
		}
		virtual int			valor_int() const {return val.actual();}
		virtual tipos			tipo() const {return tipos::tint;}
		virtual void			rotar(int d){val+=d;}
		virtual void			traducir(const struct_traduccion& t){}

		Opcion_menu_int(const std::string& n, int min, int max, int a):Base_seleccion(n), val(min, max, a) {} 
	};

	void	comprobar_opcion_existe(const Tclave& clave, const std::string& msj) const
	{
		if(!opciones.count(clave)) 
			throw Menu_opciones_exception(msj);
	}

	void	comprobar_clave_unica(const Tclave& clave) const
	{
		if(std::find(std::begin(claves), std::end(claves), clave)!=std::end(claves))
		{
			throw Menu_opciones_exception("Detectada clave duplicada");
		}

		claves.push_back(clave);
	}

	void	validar_tipo(tipos ot, tipos t, const std::string& msj) const
	{
		if(ot!=t) throw new Menu_opciones_exception(msj);
	}

	public:

	void		insertar_opcion_string(const Tclave& clave, const std::string& nombre)
	{
		comprobar_clave_unica(clave);
		opciones.insert(std::pair<Tclave, uptr_base>(clave, uptr_base(new Opcion_menu_string(nombre))));
	}

	void		insertar_opcion_int(const Tclave& clave, const std::string& nombre, int min, int max, int act)
	{
		comprobar_clave_unica(clave);
		opciones.insert(std::pair<Tclave, uptr_base>(clave, uptr_base(new Opcion_menu_int(nombre, min, max, act))));
	}

	void		eliminar_opcion(const Tclave& clave)
	{
		comprobar_opcion_existe(clave, "La clave no existe para ser eliminada");
		opciones.erase(clave);
	}

	void		insertar_seleccion_string(const Tclave& clave_opcion, const Tclave& clave_seleccion, const std::string& nombre, const std::string& valor)
	{
		comprobar_clave_unica(clave_seleccion);
		comprobar_opcion_existe(clave_opcion, "La clave no existe para insertar seleccion "+nombre+" en selecciones");
		auto& o=opciones.at(clave_opcion);
		validar_tipo(o->tipo(), tipos::tstring, "Insertar_seleccion_string : opción no es tipo string");
		static_cast<Opcion_menu_string *>(o.get())->insertar_seleccion(clave_seleccion, valor, nombre);
	}

	void		eliminar_seleccion_string(const Tclave& clave_opcion, const Tclave& clave_seleccion)
	{
		comprobar_opcion_existe(clave_opcion, "La clave no existe para eliminar seleccion en selecciones");
		auto& o=opciones.at(clave_opcion);
		validar_tipo(o->tipo(), tipos::tstring, "Eliminar_seleccion_string : opción no es de tipo string");
		static_cast<Opcion_menu_string *>(o.get())->eliminar_seleccion(clave_seleccion);
	}

	void		rotar_opcion(const Tclave& clave, int dir)
	{
		comprobar_opcion_existe(clave, "La clave no existe para ser rotada");
		opciones.at(clave)->rotar(dir);
	}

	size_t		size_opcion_string(const Tclave& clave) const
	{
		comprobar_opcion_existe(clave, "La clave no existe para size_opcion");
		auto& o=opciones.at(clave);
		validar_tipo(o->tipo(), tipos::tstring, "size_opcion_string : opción no es de tipo string");
		return static_cast<Opcion_menu_string *>(o.get())->size();
	}

	std::string	valor_string(const Tclave& clave) const
	{
		comprobar_opcion_existe(clave, "La clave no existe para obtener_valor");
		const auto& o=opciones.at(clave);
		validar_tipo(o->tipo(), tipos::tstring, "valor_opcion : opción no es de tipo string");
		return o->valor_string();
	}

	int	valor_int(const Tclave& clave) const
	{
		comprobar_opcion_existe(clave, "La clave no existe para obtener_valor");
		const auto& o=opciones.at(clave);
		validar_tipo(o->tipo(), tipos::tint, "valor_opcion : opción no es de tipo int");
		return o->valor_int();
	}

	std::string	nombre_opcion(const Tclave& clave) const
	{
		comprobar_opcion_existe(clave, "La clave no existe para nombre_opcion");
		return opciones.at(clave)->nombre;
	}

	std::string	nombre_seleccion(const Tclave& clave) const
	{
		comprobar_opcion_existe(clave, "La clave no existe para nombre_seleccion");
		const auto& o=opciones.at(clave);
		return o->valor_visible();
	}


	void		traducir(const std::vector<struct_traduccion>& v)
	{
		for(const auto& t : v) 
		{
			for(auto& opcion : opciones)
			{
				t.reemplazar(opcion.first, opcion.second->nombre);
				opcion.second->traducir(t);
			}
		}
	}

	size_t		size() const
	{
		return opciones.size();
	}
	
	//Sólo para tipo string.
	void		asignar_por_clave_string(const Tclave& clave_opcion, const Tclave& clave_seleccion)
	{
		comprobar_opcion_existe(clave_opcion, "La clave no existe para seleccionar opción");
		auto& o=opciones.at(clave_opcion);
		validar_tipo(o->tipo(), tipos::tstring, "seleccionar_opcion_string : opción no es de tipo string");
		static_cast<Opcion_menu_string *>(o.get())->seleccionar_opcion(clave_seleccion);
	}


	void		asignar_por_valor(const Tclave& clave_opcion, const std::string& valor_seleccion)
	{
		comprobar_opcion_existe(clave_opcion, "La clave no existe para seleccionar opción");
		auto& o=opciones.at(clave_opcion);
		validar_tipo(o->tipo(), tipos::tstring, "asignar_por_valor : opción no es de tipo string");
		static_cast<Opcion_menu_string *>(o.get())->asignar_por_valor(valor_seleccion);
	}

	void		asignar_por_valor(const Tclave& clave_opcion, int valor_seleccion)
	{
		comprobar_opcion_existe(clave_opcion, "La clave no existe para seleccionar opción");
		auto& o=opciones.at(clave_opcion);
		validar_tipo(o->tipo(), tipos::tint, "asignar_por_valor : opción no es de tipo string");
		static_cast<Opcion_menu_int *>(o.get())->val=valor_seleccion;
	}

	std::vector<Tclave>		obtener_claves() const 
	{
		//No podemos usar "claves" puesto que contiene también las claves de las selecciones...
		std::vector<Tclave> res;
		for(const auto& o : opciones) res.push_back(o.first);
		return res;
	}

	private:

	mutable std::vector<Tclave>					claves;
	std::map<Tclave, std::unique_ptr<Base_seleccion>>		opciones;
};

}

/*

Ejemplo de uso

int main(int argc, char ** argv)
{
	using namespace Herramientas_proyecto;

	Menu_opciones<std::string, int> menu;

	menu.insertar_opcion(1, "--OPCION_1");
	menu.insertar_seleccion_en_opcion(1, 100, "--VALOR 1.1", "PRIMER VALOR 1");
	menu.insertar_seleccion_en_opcion(1, 101, "--VALOR 1.2", "2 VALOR 1");
	menu.insertar_seleccion_en_opcion(1, 102, "--VALOR 1.3", "3 VALOR 1");

	menu.insertar_opcion(2, "--OPCION_2");
	menu.insertar_seleccion_en_opcion(2, 103, "--VALOR 2.1", "1 VALOR 2");
	menu.insertar_seleccion_en_opcion(2, 104, "--VALOR 2.2", "2 VALOR 2");

	menu.insertar_opcion(3, "--OPCION_3");
	menu.insertar_seleccion_en_opcion(3, 105, "--VALOR 3.1", "1 VALOR 3");
	menu.insertar_seleccion_en_opcion(3, 106, "--VALOR 3.2", "2 VALOR 3");

	std::vector<Menu_opciones<std::string, int>::struct_traduccion > trad={ 
		{1, "TAMAÑO PANTALLA"}, {2, "VOLUMEN SONIDO"}, {3, "IDIOMA"}, 
		{100, "300x200"}, {101, "600x400"}, {102, "1200x800"},
		{103, "Medio"}, {104, "Alto"}, 
		{105, "Español"}, {106, "Inglés"} 
	};
	menu.traducir(trad);

	const auto& v=menu.obtener_claves();

	int i=0;

	while(i < 4)
	{
		for(auto c : v)
		{
			std::cout<<"C"<<c<<" : "<<menu.nombre_opcion(c)<<" : "<<menu.nombre_seleccion(c)<<" : '"<<menu.valor_opcion(c)<<"'"<<std::endl;
			menu.rotar_opcion(c, 1);
		}

		std::cout<<" ------ "<<std::endl;
		++i;
	}

	return 1;
}
*/

#endif
