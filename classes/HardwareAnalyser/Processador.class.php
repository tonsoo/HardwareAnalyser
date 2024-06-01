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

    public function Nota() : float {

        return 0;
    }

    public function ToArray() : array {
        
        return get_object_vars($this);
    }
}