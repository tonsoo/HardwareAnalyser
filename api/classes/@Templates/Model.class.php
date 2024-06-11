<?php

class Model {

    public function __get(string $attr) {

        if(!isset($this->$attr)){
            return null;
        }

        if(substr($attr, 0, 1) == '_'){
            throw new Exception('Trying to access private property');
        }

        return $this->$attr;
    }

    public function __set(string $attr, $value){

        if(substr($attr, 0, 1) == '_'){
            throw new Exception('Trying to alter private property');
        }

        $this->$attr = $value;
    }
}