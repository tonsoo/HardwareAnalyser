#include "ComponenteHardware.h"
#include <string.h>

namespace SQR {

	ComponenteHardware::ComponenteHardware(char* nome, char* fabricante) {

		this->nota = 0;

		strcpy_s(this->nome, 50, nome);
		strcpy_s(this->fabricante, 150, fabricante);
	}

	void ComponenteHardware::Enviar() { }

	void ComponenteHardware::CalcularNota() { }
}