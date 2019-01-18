<?php


  $ecrit = file_get_contents('php://input');
  $decode=json_decode($ecrit);
  if (! $decode) {
    http_response_code(415);
  }
  elseif (! $decode->temperature || ! $decode->humidite) {
    http_response_code(400);
  }
  else {
    $fichier=fopen("data.json","w");
    fwrite($fichier,$ecrit);
    fclose($fichier);
  }

  require_once 'add.php';
  $date=(date('l d F  Y H:i:s'));
  try {
    //Connexion
    $bdd = new PDO("mysql:host=$host;dbname=$dbname", $username, $password);
    
    //Préparation de la requête
    $req = $bdd->prepare('INSERT INTO Donnees_MyDHT11 (date,temperature,humidite)'
                        .'             VALUES (:date,:temperature,:humidite)');
    

    //Requête SQL
    $req->execute(array('date' => ''.$date,
                        'temperature' => ''.$decode->temperature,
                        'humidite' => ''.$decode->humidite
                        ));

    $req = null;
    
    $bdd = null;
    } 
    catch (PDOException $e) {
      print "Erreur : " . $e->getMessage() . "<br/>";
      die();
    }

?>
