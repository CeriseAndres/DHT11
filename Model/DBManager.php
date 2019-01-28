<?php
namespace DHT11\Model;


  class DBManager extends Config {
      public function dbConnect() {

        try {
          $bdd = new \PDO("mysql:host=$this->host;dbname=$this->dbname", $this->username, $this->password);
              return $bdd;
        }
        catch (\PDOException $e) {
          print "Erreur : " . $e->getMessage() . "<br/>";
          die();
        }
      }
  }
?>
