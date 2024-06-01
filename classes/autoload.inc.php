<?php

function autoload__search_class(string $class, string $dir) : bool {

    $files = scandir($dir, SCANDIR_SORT_ASCENDING);
    foreach($files as $file){
        if(is_dir($file)){
            return autoload__search_class($class, $file);
        }

        if(!is_file($file)){
            continue;
        }

        $file_name = basename($file);
        if(!preg_match('/\.class\.php$/', $file_name)){
            continue;
        }

        require $file;
        return true;
    }

    return false;
}

spl_autoload_register(function($class) {

    if(!autoload__search_class($class, __DIR__)){
        return;
    }
});