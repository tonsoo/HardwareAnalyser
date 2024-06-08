<?php

class Processador extends ComponenteHardware {

    private $Modelo;
    private $Nucleos;
    private $Threads;
    private $Clock;
    private $ClockTurbo;
    private $Cache;

    public function __construct(int $id, string $nome, string $modelo, string $fabricante, int $nucleos, int $threads, int $clock, int $clockTurbo, float $cache) {

        parent::__construct($id, $nome, $fabricante);
        $this->Modelo = $modelo;
        $this->Nucleos = $nucleos;
        $this->Threads = $threads;
        $this->Clock = $clock;
        $this->ClockTurbo = $clockTurbo;
        $this->Cache = $cache;
    }

    public static function FromArray(array $data) : self {

        $correct_format = true;
        $correct_format = $correct_format && ($data['nome'] ?? '');
        $correct_format = $correct_format && ($data['modelo'] ?? '');
        $correct_format = $correct_format && ($data['fabricante'] ?? '');
        $correct_format = $correct_format && ($data['nucleos'] ?? '');
        $correct_format = $correct_format && ($data['threads'] ?? '');
        $correct_format = $correct_format && ($data['clock'] ?? '');
        $correct_format = $correct_format && ($data['clock_turbo'] ?? '');
        $correct_format = $correct_format && ($data['cache'] ?? '');

        if(!$correct_format){
            throw new MissingInputException("There are missing input for a Processor");
        }

        return new self(0, $data['nome'], $data['modelo'], $data['fabricante'], $data['nucleos'], $data['threads'], $data['clock'], $data['clock_turbo'], $data['cache']);
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