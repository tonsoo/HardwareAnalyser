#include "PlacaDeVideo.h"

namespace SQR {

	PlacaDeVideo::PlacaDeVideo(double vram, int cores, int threads, double clock_base, double clock_turbo, double cache, char* nome, char* fabricante)
		: Processador(cores, threads, clock_base, clock_turbo, cache, nome, fabricante) {
		this->vram = vram;
	}
}