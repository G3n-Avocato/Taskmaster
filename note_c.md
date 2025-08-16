Note Laura :

Fait :
* exitcode version non scalar 
* trouver leak ligne 64 last_key not free sur une sortie
* resserer parsing pour chaque commande 
* sur-parsing pour changer type de certaine config
* ajouter env fonction

En cours :

* tester avec des configs erreurs voir ce qui passe et ne passe pas
* tester les sorties d'erreur (voir ce qu'il reste a free)

A faire : 

* revoir fonction parsing exitcode scalar 

* init config struct avec val par default

* fonction qui verifie pour chaque struct que cmd est present 

* reprendre reste du code en C