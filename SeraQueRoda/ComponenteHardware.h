#pragma once

#ifndef ComponenteHardware_h
#define ComponenteHardware_h

namespace SQR {

	class ComponenteHardware {
	public:
		// Componente que não existe no banco de dados
		ComponenteHardware(char* nome, char* fabricante);

		const char* Nome() const { return nome; }
		const char* Fabricante() const { return fabricante; }
		const double& Nota() const { return nota; }

		void CalcularNota();
		void Enviar();

	protected:
		double nota;
		char nome[50];
		char fabricante[150];
	};
}

#endif