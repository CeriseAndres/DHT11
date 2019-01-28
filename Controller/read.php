<?php

require '../Model/MeasureManager.php';

$measure = new \DHT11\Model\MeasureManager();

$req = $measures->getMeasure();


require '../view/thermometre.php';
