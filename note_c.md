Note Laura :


Fait :
* petite verif a faire
-> parser variable has_cmd non utiliser 
-> mutex ?? race condition dans tous les sens
* ajouter loger reload fct in logger.h
-> A test sighup gestion signal

En cours :

-> A test logrotate_loop


* gestion erreur et logger in command
-> fichier parsing logger gestion erreur a revoir sur tout le projet

A faire : 

* parsing ajouter une verif pour le meme nom (saucisse: saucisse: comportement aleatoire)(pas tres beau)

* ranger un peu 


TEST A FAIRE :
* manually killing supervised processes, 
* trying to launch processes that never start correctly, 
* launching processes that generate lots of output
* tester avec PLUS DE configs erreurs 
-> verifier sortie d'erreur de lancement dans le terminal pour start et restart 




etape : parsing config bon -> creation du fichier de log -> demarrage -> ?

exemple_1:

ok Processus démarré	            INFO spawned: 'nom_du_process' with pid XXXX
ok Processus entré en état RUNNING	INFO success: nom_du_process entered RUNNING state
ok Processus arrêté normalement	INFO exited: nom_du_process (exit status 0; expected)
ok Processus mort anormalement	    INFO exited: nom_du_process (exit status 1; not expected)
ok Processus redémarré	            INFO spawned: 'nom_du_process' with pid XXXX
ok Processus atteint la limite de redémarrage	INFO gave up: nom_du_process entered FATAL state, too many start retries too quickly
ok Processus arrêté manuellement	INFO stopped: nom_du_process

exemple_2:
startsupervisor program:

no   CRIT Supervisor running as root (no user in config file)
no   INFO Included extra file "/etc/supervisor/conf.d/myapp.conf" during parsing
no   INFO RPC interface 'supervisor' initialized
no   INFO RPC interface 'supervisor' initialized
good INFO supervisord started with pid 12345

startapp:
good INFO spawned: 'myapp' with pid 12346
good INFO success: myapp entered RUNNING state, process has stayed up for > than 1 seconds (startsecs)

endapps:
good waiting for myapp to die
good stopped: myapp (terminated by SIGTERM)

endapp exit:
good INFO exited: myapp (exit status 1; not expected)
good INFO gave up: myapp entered FATAL state, too many start retries too quickly


reload:
INFO received SIGUSR2 indicating restart request
no INFO rereading: /etc/supervisor/conf.d/myapp.conf
good INFO found config changes to myapp
good INFO updating process myapp
good INFO stopped: myapp
 
good INFO spawned: 'myapp' with pid 13001
good INFO success: myapp entered RUNNING state, process has stayed up for > than 1 seconds (startsecs)

restart supervisor
no   INFO daemonizing the supervisord process
good INFO supervisord started with pid 14000
good INFO spawned: 'myapp' with pid 14001
good INFO success: myapp entered RUNNING state, process has stayed up for > than 1 seconds (startsecs)
