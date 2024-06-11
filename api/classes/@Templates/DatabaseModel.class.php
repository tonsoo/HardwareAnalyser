<?php

abstract class DatabaseModel extends Model {

    public abstract static function FromArray(array $data) : self;
}