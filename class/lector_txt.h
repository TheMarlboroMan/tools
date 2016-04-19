#ifndef LECTOR_TXT_HERRAMIENTAS_PROYECTO
#define LECTOR_TXT_HERRAMIENTAS_PROYECTO

#include <string>
#include <fstream>
#include <vector>


namespace Herramientas_proyecto
{

class Lector_txt
{
	private:

	unsigned int numero_linea;
	char comentario;
	std::string linea_actual;
	std::ifstream archivo;

	public:

	Lector_txt();
	Lector_txt(const std::string& ruta, const char comentario);
	~Lector_txt();

	std::string leer_linea();
	void abrir_archivo(const std::string&);

	/*explicit*/ operator bool() const {return archivo;}
	bool es_eof() const {return archivo.eof();}
	unsigned int obtener_numero_linea() const {return numero_linea;}
	char obtener_comentario() const {return comentario;}
	const std::string& obtener_linea_actual() {return linea_actual;}
	void establecer_comentario(const char v) {comentario=v;}
	void rebobinar(){archivo.seekg(0, archivo.beg);}
};

std::vector<std::string> obtener_entradas_lector_txt_desde_ruta(const std::string& ruta);

}

#endif
