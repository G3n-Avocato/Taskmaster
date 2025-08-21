Note Laura :

Fait et A revoir :
* tester avec des configs erreurs voir ce qui passe et ne passe pas
* tester les sorties d'erreur (voir ce qu'il reste a free)

Fait :
* signal ctrl+c 

DATE- HEURE:MIN,MILLIS NIVEAU

En cours :

* logger supervisord

-> changer toute les sorties d'erreur en log pour logger (adapter les fct)
-> voir si toute ces sorties necessites des return du program, non program peut tourner il suffit de reload les fichier

INFO ->ajouter les loggers pour les infos de base start - end - config reload - ctrl display
- etats des programmes





A faire : 


* ajouter partie controller
-> fct de comparaison pour reload le fichier et voir si changement pour relancer ou non 
-> boucle fct pour trouver les groupname ou des pos id precis
* partie reload file, il faut comparer ajouter mais aussi suprrimer des process free
* signal ctrl+/ ctrl+quit


* revoir stdout et stderr des processus -> en cas de max output il faut pouvoir gerer ca 
* tester bad process with thread check erreur 




manually killing supervised processes, 
trying to launch processes that never start correctly, 
launching processes that generate lots of output

