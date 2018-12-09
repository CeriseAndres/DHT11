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




 ?>
