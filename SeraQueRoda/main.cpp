#include "HttpHandler.h"

#include <iostream>
#include <sstream>
#include <shellapi.h>

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

std::ostream& operator <<(std::ostream& os, const SQR::PlacaDeVideo* proc) {

	os << "-=-=-=-=-=-=-=-=\n";
	os << "Informacoes da Placa de Video:\n";
	os << "  Nome: " << proc->Nome() << "\n";
	os << "  Fabricante: " << proc->Fabricante() << "\n";
	os << "  Clock Base: " << proc->ClockBase() << "MHz\n";
	os << "  Clock Turbo: " << proc->ClockTurbo() << "MHz\n";
	os << "  Num. Nucleos: " << proc->Cores() << "\n";
	os << "  Num. Threads: " << proc->Threads() << "\n";
	os << "  Cache: " << proc->Cache() << "Mb\n";
	os << "  VRAM: " << proc->VRAM() << "Mb\n";
	os << "\n";
	os << "Pontuacao: " << proc->Nota() << "\n";
	os << "-=-=-=-=-=-=-=-=\n";

	return os;
}

std::string to_url(const std::string& value) {

	return value;
}

int main() {

	Http::HttpHandler* handler = new Http::HttpHandler();

	/*handler->Fetch("http://localhost/processador/cadastrar?nome=i3%2010100f&fabricante=Intel&nucleos=4&threads=8&clock=3600&clock_turbo=4100&cache=8", "80");

	return 0;*/
	// Inicializa o analisador de hardware
	SQR::HardwareAnalyser* analyser = new SQR::HardwareAnalyser();

	/* PROCESSADOR */
	// Inicializa um processador com base no processador do usuario
	SQR::Processador* p = analyser->ProcessorInfo();

	handler->AddParameter("Nome", p->Nome());
	handler->AddParameter("Fabricante", p->Fabricante());
	handler->AddParameter("Nucleos", p->Cores());
	handler->AddParameter("Threads", p->Threads());
	handler->AddParameter("Clock", p->ClockBase());
	handler->AddParameter("ClockTurbo", p->ClockTurbo());
	handler->AddParameter("Cache", p->Cache());

	std::cout << "Server response: " << handler->Fetch("http://api.seraqueroda.com.br/processador/cadastrar", "80") << "\n";

	if (handler->ResponseCode != 200) {
		std::cerr << "Houve um erro ao realizar conex�o com o servidor!";
		return -1;
	}

	ShellExecute(NULL, L"open", L"https://www.seraqueroda.com.br", NULL, NULL, SW_SHOWNORMAL);

	ExitProcess(0);

	return 0;

	p->Cache();

	// Calcula a nota com uma margem de erro de 10
	//p->CalcularNota(10);

	// Calcula a nota sem margem de erro
	p->CalcularNota(0);

	// Mostra as informa��es do processador na tela
	std::cout << p;

	/* PLACA DE VIDEO */

	SQR::PlacaDeVideo* gpu = analyser->GPUInfo();

	//gpu->CalcularNota(0);

	std::cout << gpu;

	std::cin.get();

	return 0;
}
