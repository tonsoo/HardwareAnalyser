<?php

class Jogo extends DatabaseModel {

    private $Id;
    private $Nome;
    private $Lancamento;
    private $Descricao;
    private $LinkDownload;

    public function __construct($id, $nome, $lancamento, $descricao, $link_download) {

        $this->Id = $id;
        $this->Nome = $nome;
        $this->Lancamento = $lancamento;
        $this->Descricao = $descricao;
        $this->LinkDownload = $link_download;
    }

    public static function FromArray(array $data) : self {

        $id = $data['Id'] ?? 0;
        $nome = $data['Nome'] ?? null;
        $lancamento = $data['Lancamento'] ?? null;
        $descricao = $data['Descricao'] ?? null;
        $link_download = $data['LinkDownload'] ?? null;

        if($nome === null || $lancamento === null || $descricao === null || $link_download === null) {
            throw new MissingInputException('There are missing inputs for Jogo');
        }

        return new self($id, $nome, $lancamento, $descricao, $link_download);
    }

    public function ToArray() : array {

        return get_object_vars($this);
    }
}