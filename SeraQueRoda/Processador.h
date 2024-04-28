#pragma once

#include <iostream>
#include <random>
#include "ComponenteHardware.h"

#ifndef Processador_h
#define Processador_h

namespace SQR {

	class Processador : public ComponenteHardware {
	public:
		std::random_device rd;

		const int& Cores() const { return cores; }
		const int& Threads() const { return threads; }
		const double& ClockBase() const { return clock_base; }
		const double& ClockTurbo() const { return clock_turbo; }
		const double& Cache() const { return cache; }

		Processador(int cores, int threads, double clockSpeed, double clock_turbo, double cache, char* nome, char* fabricante);

		void CalcularNota(int applyedOffset);
		void Enviar();

	private:
		int cores;
		int threads;
		double clock_base; // Clock Base em MHz
		double clock_turbo; // Clock Turbo em MHz
		double cache; // Memoria cache em Mb
	};
}

#endif