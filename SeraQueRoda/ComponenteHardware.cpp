#include "ComponenteHardware.h"
#include <string.h>

namespace SQR {

	ComponenteHardware::ComponenteHardware(char* nome, char* fabricante) {

		strcpy_s(this->nome, 50, (char*)nome);
		strcpy_s(this->fabricante, 150, (char*)fabricante);
	}

	void ComponenteHardware::Enviar() { }

	void ComponenteHardware::CalcularNota() { }
}