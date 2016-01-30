#include <fstream>
#include "../class/dnot_parser.cpp"
#include "../class/dnot_token.cpp"

int main(int argc, char ** argv)
{
	using namespace Herramientas_proyecto;

	auto tok=parsear_dnot("dnot_test.dnot");

	std::cout<<"===== DOCUMENTO ORIGINAL ====="<<std::endl;
	std::cout<<tok.serializar();

	tok["nodo"][0]["valor_string"].asignar("Cambio");
	tok["nodo"][1]["bool_cambiar"].asignar(false);

	auto& l=tok["nodo"][0]["lista"].acc_lista();
	l.erase(l.begin()+1);

	Dnot_token nuevo_tok_1("Tralala");

	tok["nodo"][1].acc_tokens().insert(Dnot_token::par_mapa("nuevo_string", nuevo_tok_1));

	Dnot_token::t_vector lista;
	lista.push_back(Dnot_token("Hola"));
	lista.push_back(Dnot_token(1));
	lista.push_back(Dnot_token(2.44f));
	lista.push_back(Dnot_token(true));
	Dnot_token nodo_lista;
	nodo_lista.asignar(lista);
	tok["nodo"][1].acc_tokens().insert(Dnot_token::par_mapa("nueva_lista", nodo_lista));
	
	std::cout<<std::endl<<std::endl;
	std::cout<<"===== DOCUMENTO MODIFICADO ====="<<std::endl;
	std::cout<<tok.serializar();

	std::ofstream salida("dnot_cambiado.dnot");
	salida<<tok.serializar();
	salida.close();

	auto tok2=parsear_dnot("dnot_cambiado.dnot");
	std::cout<<std::endl<<std::endl;
	std::cout<<"===== DOCUMENTO GUARDADO Y LEIDO ====="<<std::endl;
	std::cout<<tok2.serializar();
	std::cout<<std::endl<<std::endl;

	return 1;

}
