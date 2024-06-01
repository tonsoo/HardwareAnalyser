<?php

function autoload__search_class(string $class, string $dir) : bool {

    $files = scandir($dir, SCANDIR_SORT_ASCENDING);
    foreach($files as $file_name){
        if(in_array($file_name, ['.', '..'])){
            continue;
        }

        $file_path = $dir.DIRECTORY_SEPARATOR.$file_name;

        if(is_dir($file_path)){
            if(autoload__search_class($class, $file_path)){
                return true;
            }

            continue;
        }

        if(!is_file($file_path)){
            continue;
        }

        if(!preg_match("/^{$class}\.class\.php$/", $file_name)){
            continue;
        }

        require $file_path;
        return true;
    }

    return false;
}

spl_autoload_register(function($class) {

    if(!autoload__search_class($class, __DIR__)){
        return;
    }
});