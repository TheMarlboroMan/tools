#ifndef MENU_OPCIONES_HERRAMIENTAS_PROYECTO
#define MENU_OPCIONES_HERRAMIENTAS_PROYECTO

#include <string>
#include <map>
#include <vector>
#include <exception>
#include <algorithm>

/*
La representación interna de un menú de opciones de un único nivel en el que
cada opción puede tener N valores que van "rotando", por ejemplo:

menu:
	resolución de pantalla : [800x600], [1000x1000]
	volumen_audio : [Sin sonido, Medio, Alto]
	idioma: [Español, Inglés, Francés, Japonés]

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
*/

namespace Herramientas_proyecto
{

class Menu_opciones_exception:
	public std::runtime_error
{
	public:
	Menu_opciones_exception(const std::string& s):std::runtime_error(s) {}
};

template<typename Tvalor, typename Tclave>
class Menu_opciones
{
	private:

	struct Seleccion_menu
	{		
		Tvalor				valor;
		std::string			nombre;
	};

	struct Opcion_menu
	{
		std::string 			nombre;
		std::map<Tclave, Seleccion_menu>	selecciones;
		Tclave				opcion_actual;

		void				comprobar_opciones_existen(const std::string& msj) const
		{
			if(!selecciones.size()) throw Menu_opciones_exception(msj);
		}

		void 				rotar(int dir)
		{
			auto it=selecciones.find(opcion_actual);

			//Descender...
			if(dir < 0)
			{
				if(it==selecciones.begin()) //Dar la vuelta.
				{
					opcion_actual=selecciones.rbegin()->first;
				}
				else
				{
					opcion_actual=std::prev(it)->first;
				}
			}
			//Ascender...
			else
			{
				auto sigue=std::next(it);
				if(sigue==std::end(selecciones))
				{
					opcion_actual=selecciones.begin()->first;
				}
				else
				{
					opcion_actual=sigue->first;
				}
			}

		}

		void				insertar_seleccion(const Tclave& clave, const Tvalor& valor, const std::string& nombre)
		{
			if(!selecciones.size()) opcion_actual=clave;
			selecciones[clave]={valor, nombre};
		}


						Opcion_menu(const std::string& n):nombre(n), opcion_actual() {}
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

	public:

	struct struct_traduccion
	{
		Tclave busca;
		std::string reemplaza;
	};


	void		insertar_opcion(const Tclave& clave, const std::string& nombre)
	{
		comprobar_clave_unica(clave);
		opciones.insert(std::pair<Tclave, Opcion_menu>(clave, Opcion_menu(nombre)));
	}

	void		eliminar_opcion(const Tclave& clave)
	{
		comprobar_opcion_existe(clave, "La clave no existe para ser eliminada");
		opciones.erase(clave);
	}

	void		insertar_seleccion_en_opcion(const Tclave& clave_opcion, const Tclave& clave_seleccion, const std::string& nombre, const Tvalor& valor)
	{
		comprobar_clave_unica(clave_seleccion);
		comprobar_opcion_existe(clave_opcion, "La clave no existe para insertar seleccion "+nombre+" en selecciones");
		auto& o=opciones.at(clave_opcion);
		o.insertar_seleccion(clave_seleccion, valor, nombre);
	}

	void		eliminar_seleccion(const Tclave& clave_opcion, const Tclave& clave_seleccion)
	{
		comprobar_opcion_existe(clave_opcion, "La clave no existe para eliminar seleccion en selecciones");
		auto& o=opciones.at(clave_opcion);

		if(!o.selecciones.count(clave_seleccion)) throw Menu_opciones_exception("La clave no existe en selecciones para eliminar");
		o.selecciones.erase(clave_seleccion);
	}

	void		rotar_opcion(const Tclave& clave, int dir)
	{
		comprobar_opcion_existe(clave, "La clave no existe para ser rotada");
		opciones.at(clave).rotar(dir);
	}

	size_t		size_opcion(const Tclave& clave) const
	{
		comprobar_opcion_existe(clave, "La clave no existe para obtener_valor");
		return opciones.at(clave).selecciones.size();
	}

	Tvalor		valor_opcion(const Tclave& clave) const
	{
		comprobar_opcion_existe(clave, "La clave no existe para obtener_valor");
		const auto& o=opciones.at(clave);
		o.comprobar_opciones_existen("La opción no tiene selecciones para obtener valor");
		return o.selecciones.at(o.opcion_actual).valor;
	}

	std::string	nombre_opcion(const Tclave& clave) const
	{
		comprobar_opcion_existe(clave, "La clave no existe para obtener_valor");
		return opciones.at(clave).nombre;
	}

	std::string	nombre_seleccion(const Tclave& clave) const
	{
		comprobar_opcion_existe(clave, "La clave no existe para obtener_valor");
		const auto& o=opciones.at(clave);
		o.comprobar_opciones_existen("La opción no tiene selecciones para obtener valor");
		return o.selecciones.at(o.opcion_actual).nombre;
	}

	void		traducir(const std::vector<struct_traduccion>& v)
	{
		for(const auto& t : v) traducir(t); 
	}

	void		traducir(const struct_traduccion& t)
	{
		const auto busca=t.busca, reemplaza=t.reemplaza;

		auto reemplazar=[&busca, &reemplaza](const Tclave& clave, std::string& cad)
		{
			if(clave==busca) 
			{
				cad=reemplaza;
			}
		};

		for(auto& opcion : opciones)
		{
			reemplazar(opcion.first, opcion.second.nombre);
			for(auto& seleccion : opcion.second.selecciones)
			{
				reemplazar(seleccion.first, seleccion.second.nombre);
			}
		}
	}

	size_t		size() const
	{
		return opciones.size();
	}


	std::vector<Tclave>		obtener_claves() const 
	{
		//No podemos usar "claves" puesto que contiene también las claves de las selecciones...
		std::vector<Tclave> res;
		for(const auto& o : opciones) res.push_back(o.first);
		return res;
	}

	private:

	mutable std::vector<Tclave>		claves;
	std::map<Tclave, Opcion_menu>		opciones;
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
