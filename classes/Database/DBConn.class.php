<?php

class DBConn extends UniqueModel {

    private $Conn;
    private $Host;
    private $Port;
    private $User;
    private $Password;
    private $Database;

    public function Connect(string $username, string $password, string $database, string $host = 'localhost', int $port = 3306) : self {

        $this->Host = $host;
        $this->Port = $port;
        $this->User = $username;
        $this->Password = $password;
        $this->Database = $database;

        $settings = [
            PDO::ATTR_ERRMODE => PDO::ERRMODE_EXCEPTION,
            PDO::MYSQL_ATTR_INIT_COMMAND => "SET NAMES utf8"
        ];

        try {
            $this->Conn = new PDO("mysql:dbname={$this->Database};host={$this->Host}", $this->User, $this->Password, $settings);
        } catch(PDOException $ex){
            throw new DBConnException("Failed to connect on {$this->Host} with user {$this->User}\n", $ex->getCode());
        }

        return $this;
    }

    public function Execute(string $query, array $binds = []) : array {

        foreach($binds as $bind => $value){
            if(strpos($query, $bind) !== false){
                continue;
            }

            unset($binds[$bind]);
        }

        echo $query;
        echo '<pre>';
        print_r($binds);
        echo '</pre>';

        try {
            $stmt = $this->Conn->prepare($query, [
                PDO::ATTR_CURSOR => PDO::CURSOR_FWDONLY
            ]);

            $stmt->execute($binds);

            return $stmt->fetchAll(PDO::FETCH_ASSOC);
        } catch (PDOException $ex){
            throw new DBExecuteException("Failed to execute {$query}\n", $ex->getCode());
        }
    }

    private function ConvertToBind(array $binds, string $prefix = '') : array {

        $new_binds = [];
        foreach($binds as $key => $value) {
            if(is_object($value) || is_array($value)){
                throw new DBInvalidFormat("Value type of {$key} is invalid");
            }

            $new_binds[":{$prefix}{$key}"] = $value;
        }

        return $new_binds;
    }

    public function Read(string $table, string $query = '', array $binds = []) : array {

        $new_binds = $this->ConvertToBind($binds);
        return $this->Execute("SELECT * FROM {$table} {$query}", $new_binds);
    }

    public function Create(string $table, array $inserts) : array {

        $insert_data = $this->ConvertToBind($inserts);

        $insert_names = array_keys($inserts);
        $insert_binds = implode(',', array_map(function($e){
            return ":{$e}";
        }, $insert_names));
        $insert_names = implode(',', $insert_names);

        return $this->Execute("INSERT INTO {$table} ({$insert_names}) VALUES ({$insert_binds})", $insert_data);
    }

    public function Update(string $table, string $condition, array $updates, array $binds = []) : array {

        $update_data = $this->ConvertToBind($updates, 'update');
        $update_query = implode(', ', array_map(function($e){
            return "{$e}=:update{$e}";
        }, array_keys($updates)));
        $update_query = $update_query ? "SET {$update_query}" : '';

        if(!$update_query || !$condition){
            throw new DBExecuteException('Missing information for update');
        }

        $new_binds = $this->ConvertToBind($binds);

        return $this->Execute("UPDATE {$table} {$update_query} {$condition}", array_merge($new_binds, $update_data));
    }

    public function Delete(string $table, string $condition, array $binds) : array {

        $new_binds = $this->ConvertToBind($binds, '');
        return $this->Execute("DELETE FROM {$table} {$condition}", $new_binds);
    }
}