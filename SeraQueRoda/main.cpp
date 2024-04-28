#include <iostream>

#include "HardwareAnalyser.h"
#include "Processador.h"

std::ostream& operator <<(std::ostream& os, const SQR::Processador* proc) {

	os << "-=-=-=-=-=-=-=-=\n";
	os << "Informacoes do processador:\n";
	os << "  Nome: " << proc->Nome() << "\n";
	os << "  Fabricante: " << proc->Fabricante() << "\n";
	os << "  Clock Base: " << proc->ClockBase() << "MHz\n";
	os << "  Clock Turbo: " << proc->ClockTurbo() << "MHz\n";
	os << "  Num. Nucleos: " << proc->Cores() << "\n";
	os << "  Num. Threads: " << proc->Threads() << "\n";
	os << "  Cache: " << proc->Cache() << "Mb\n";
	os << "\n";
	os << "Pontuacao: " << proc->Nota() << "\n";
	os << "-=-=-=-=-=-=-=-=\n";

	return os;
}

int main() {
	// Inicializa o analisador de hardware
	SQR::HardwareAnalyser* analyser = new SQR::HardwareAnalyser();

	// Inicializa um processador com base no processador do usuario
	SQR::Processador* p = analyser->ProcessorInfo();

	// Calcula a nota com uma margem de erro de 10
	//p->CalcularNota(10);

	// Calcula a nota sem margem de erro
	p->CalcularNota(0);

	// Mostra as informações do processador na tela
	std::cout << p;

	std::cin.get();

	/*int i = 10;

	int* iP = &i;

	int& iA = i;

	std::cout << "i: " << i << "\n";
	std::cout << "iP: " << iP << "\n";
	std::cout << "iA: " << iA << "\n";*/

	return 0;
}
