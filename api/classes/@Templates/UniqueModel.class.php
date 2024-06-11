<?php

class UniqueModel extends Model {

    public static $_Instance = null;

    public static function Instance(...$params) {

        $class_name = get_called_class();

        if(self::$_Instance === null){
            self::$_Instance[$class_name] = new $class_name(...$params);
        }

        return self::$_Instance[$class_name];
    }
}