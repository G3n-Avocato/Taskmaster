Note Laura :

Fait et A revoir :
* tester avec des configs erreurs voir ce qui passe et ne passe pas
* tester les sorties d'erreur (voir ce qu'il reste a free)

Fait :

* init struct process :
* fct argv envp
* open_file std 
* fct de fork dans process
* opt autostart ok

En cours :

* loop supervisor :

* fct waitpid in loop
* fct de running
* fct pour startretries



A faire : 

* fct pour autorestart

* fct para stop 
* fct killstop

* Makefile et cmake pour lib yaml 

Ensuite :

* ajouter partie controller
-> fct de comparaison pour reload le fichier et voir si changement pour relancer ou non 
-> boucle fct pour trouver les groupname ou des pos id precis

* ajouter partie logger