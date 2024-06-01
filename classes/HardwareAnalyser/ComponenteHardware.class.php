<?php

abstract class ComponenteHardware extends Model {

    private $Id;
    private $Nome;
    private $Fabricante;

    public function __construct(int $id, string $Nome, string $Fabricante) {

        $this->Id = $id;
        $this->Nome = $Nome;
        $this->Fabricante = $Fabricante;
    }

    public abstract function Nota() : float;
}