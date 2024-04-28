#pragma once

#include "Processador.h"

#ifndef PlacaDeVideo_h
#define PlacaDeVideo_h

namespace SQR {

	class PlacaDeVideo : public Processador {
	public:
		PlacaDeVideo(double vram, int cores, int threads, double clock_base, double clock_turbo, double cache, char* nome, char* fabricante);

	private:
		double vram;
	};
}

#endif // !PlacaDeVideo_h
