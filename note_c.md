Note Laura :


Fait :
* refaire coommande reload pour qu'elle ne prenne pas d'arguments
* ctrl -> probleme prompt -> mutex
* passer ctrl en anglais
* reload
* partie reload file, il faut comparer ajouter mais aussi suprrimer des process free 
** test a faire => tester new case in struct para (new name)
                    tester initialisation new process in superMap for new config para
                    tester delete case oin struct para (delete name)
                    tester delete process in superMap

* revoir stdout et stderr des processus -> en cas de max output il faut pouvoir gerer ca 
-> stdout -> creer old -> copier dans old -> efface stdout 
-> parent verif max fichier stdout


En cours :

* ajouter loger reload fct in logger.h

-> A test logrotate_loop


A faire : 

-> mutex ?? race condition dans tous les sens
* gestion erreur et logger in command

* petite verif a faire
-> parser variable has_cmd non utiliser 


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
