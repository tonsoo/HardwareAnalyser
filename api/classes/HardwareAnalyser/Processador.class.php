<?php

class Processador extends ComponenteHardware {

    private $Nucleos;
    private $Threads;
    private $Clock;
    private $ClockTurbo;
    private $Cache;

    public function __construct(int $id, string $nome, string $fabricante, int $nucleos, int $threads, int $clock, int $clockTurbo, float $cache) {

        parent::__construct($id, $nome, $fabricante);
        $this->Nucleos = $nucleos;
        $this->Threads = $threads;
        $this->Clock = $clock;
        $this->ClockTurbo = $clockTurbo;
        $this->Cache = $cache;
    }

    public static function FromArray(array $data) : self {

        $id = $data['Id'] ?? 0;
        $nome = $data['Nome'] ?? null;
        $fabricante = $data['Fabricante'] ?? null;
        $nucleos = $data['Nucleos'] ?? null;
        $threads = $data['Threads'] ?? null;
        $clock = $data['Clock'] ?? null;
        $clockTurbo = $data['ClockTurbo'] ?? null;
        $cache = $data['Cache'] ?? null;

        if($nome === null || $fabricante === null || $nucleos === null || $threads === null || $clock === null || $clockTurbo === null || $cache === null) {
            throw new MissingInputException("There are missing input for a Processor");
        }

        return new self($id, $nome, $fabricante, $nucleos, $threads, $clock, $clockTurbo, $cache);
    }

    public function Nota() : float {

        $applyedOffset = 0;
        if($applyedOffset < 0){
            $applyedOffset *= -1;
        }

		$programInstructions = 800;

		// GHz -> Hz (10, 3)
		// MHz -> Hz (10, 6)
		// KHz -> Hz (10, 9)
		// Hz -> Hz (10, 12)
		
		$frequency = pow(10, 6) / $this->Clock;

		if ($frequency <= 0) {
			$frequency = 1;
		}

		$clockCycles = $programInstructions + rand(-$applyedOffset, -$applyedOffset);
		$instructionsUsed = $programInstructions + rand(-$applyedOffset, -$applyedOffset);

		$CPI = $clockCycles / $instructionsUsed;

		$score = $programInstructions * $CPI;

        return $score / $frequency;
    }

    public function ToArray() : array {

        return get_object_vars($this);
    }
}