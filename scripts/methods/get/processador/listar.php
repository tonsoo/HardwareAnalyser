<?php

class Processador {

    private $Id;
    private $Nome;
    private $Cores;

    public function __construct(int $id, string $nome, int $cores) {
        $this->Id = $id;
        $this->Nome = $nome;
        $this->Cores = $cores;
    }

    public function ToArray() {

        $return = [];

        $vars = get_object_vars($this);
        foreach($vars as $name => $value) {
            $return[$name] = $value;
        }

        return $return;
    }
}

set_response(HTTP_OK, 'OK', [
    'processors' => [
        (new Processador(1, 'i3 10100f', 4))->ToArray(),
        (new Processador(2, 'amd athlon 3000g', 2))->ToArray()
    ]
]);