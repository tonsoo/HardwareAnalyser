#include <iostream>
#include <random>
#include <windows.h>

#include "Processador.h"

namespace SQR {

	Processador::Processador(int cores, int threads, double clock_base, double clock_turbo, double cache, char* nome, char* fabricante) : ComponenteHardware(nome, fabricante), cores(cores),
		threads(threads), clock_base(clock_base), cache(cache), clock_turbo(clock_turbo){
		if (clock_base < 0) {
			clock_base = 0.1;
		}

		this->clock_base = clock_base;
	}

	void Processador::CalcularNota(int applyedOffset) {
		// Calculo de desempenho medio de um processador
		// antigo: (n° de instruções * (ciclos de clock / n° de instruções) * periodo de clock) / frequencia
		// novo: (n° de instruções * (ciclos de clock / n° instruções usadas)) / frequencia

		if (applyedOffset < 0) {
			applyedOffset = 0;
		}

		std::mt19937 mt(Processador::rd());

		int programInstructions = 800;

		double frequency = 1;
		// GHz -> Hz (10, 3)
		// MHz -> Hz (10, 6)
		// KHz -> Hz (10, 9)
		// Hz -> Hz (10, 12)
		
		frequency = pow(10, 6) / this->clock_base;

		if (frequency <= 0) {
			frequency = 1;
		}

		std::uniform_int_distribution<int> dist(-applyedOffset, applyedOffset);

		int clockCycles = programInstructions + dist(mt);
		int instructionsUsed = programInstructions + dist(mt);

		double CPI = (double)clockCycles / instructionsUsed;

		double score = programInstructions * CPI;

		this->nota = score / frequency;
	}
}
