# NOTE Taskmaster C++

::::::::: A REPRENDRE :::::::::::

enum ProcessStatus {
    STOPPED,        // Le processus a été arrêté manuellement ou n’a jamais été lancé.
    STARTING,       // Il est en cours de démarrage suite à une requête de lancement.
    RUNNING,        // Le processus fonctionne normalement.
    BACKOFF,        // Il a cherché à démarrer (STARTING) mais s’est arrêté trop rapidement (avant startsecs), donc supervisord va réessayer.
    STOPPING,       // Le processus est en cours d’arrêt — requête de stop en cours.
    EXITED,         // Le processus s’est terminé après avoir fonctionné normalement (ou anormalement), alors qu’il n’était pas supposé sortir spontanément.
    FATAL,          // Le processus ne peut pas démarrer du tout malgré plusieurs tentatives (basé sur startretries), et le superviseur abandonne.
    UNKNOWN,        // État indéterminé — généralement une erreur interne de Supervisord
};

Lors du démarrage :
    Si le processus ne tient pas au moins startsecs, il passe en BACKOFF.
    Il alterne alors entre STARTING et BACKOFF jusqu’à épuisement de startretries, puis va en FATAL.
Une fois RUNNING :
    S’il s’arrête spontanément :
        avec autorestart=false → reste à EXITED
        avec autorestart=true → redémarre toujours
        avec autorestart=unexpected → redémarre seulement si le code de sortie n’est pas dans exitcodes



parsing : 
-> erreur dans le parsing en cas de fichier non-existant a revoir
-parsing renforcer pour chaque option reste std::string ,  workdir , stdout , stderr , cmd , comment parser ces paras
-> erreur stdout stderr non defini creer erreur parsing -> a changer std pas forcement definie
-test info aleatoire  

-> erreur fork() child process : en cas d'erreur exec dans child impossible de free quoi que ce soit (test ex cmd: ls -la)


list -> quel parametre touches au processus et necessite une relance de la cmd reload
* Note laura : continuer a ajouter les parametres tout en adaptant supervisor



# Plan Global
1- Implementation de la gestion basique des processus
ok    lancer un processus enfant avec - FORK()
ok    surveiller son etat - STD::THREAD
ok    envoyer des signaux SIGKILL pour arreter proprement
ok    recuperer la sortie standard / erreur

2- Supervision + redemarrage automatique 
    implementer systeme de boucle pour verifier etat
    suivre politique de redemarrage
    mettre en place toute les options 

3- Gestion des logs
ok    capturer stdout / stderr des processus dans fichier
    mettre en place rotation des logs (size nbre fichier) ....?

4- Gestion Config 
ok    format (YAML) 
ok    list parametre proposer fichier 
a revoir    charger et PARSER FICHIER de config au demarrage 
    
5- Interface user / controller 
    en ligne de commande (start stop statut etc)
    Permettre le rechargement dynamique du fichier de config ....?

# Def + notes fct
supervisord -> Gerer plusieurs processus : permet de lancer plusieurs services depuis un meme point central 
            -> Redemarrage automatique : si un processus plante, supervisord relance automatiquement
            -> Logging centralisee : gere les logs stdout et stderr de chaque processus (debug + monitoring)
            -> Demarrage au boot : supervisord peut demarrer automatiquement les processus definis lorsqu'il est lancee 
            -> Controle a distance : tu peux demarrer arreter redemarrer ou verif l'etat des processus a tout moment 




pid_t pid = fork() 
-- neg fork echoue 
-- 0 on est dans processus fils 
-- + dans processus parents

exec() - execlp() - execvp()
remplace procesus enfant par une commande reelle 

waitpid()
dans parents attende qu un enfant meure et dit pourquoi 

signal()
intercepter signaux SIGCHLD ou SIGTERM

std::thread 

kill(pid, 0)

# PARSING FILE CONFIG NOTE

    programs:
      nginx:
ok      cmd: "/usr/local/bin/nginx -c /etc/nginx/test.conf" // requis                       // redemarrage
ok      numprocs: 1                                         // non requis default:1         // oui
ok      umask: 022                                          // non requis default:022       // oui
ok      workingdir: /tmp                                    // non requis default:/         // oui
ok      autostart: true                                     // non requis default:true
        autorestart: unexpected                             // non requis default:unexpected
        exitcodes:                                          // non requis default:0
          - 0
          - 2
ok      startretries: 3                                     // non requis default:3
ok      starttime: 5                                        // non requis default:1
        stopsignal: TERM                                    // non requis default:TERM
        stoptime: 10                                        // non requis default:10
ok      stdout: /tmp/nginx.stdout                           // non requis default:NULL      // oui
ok      stderr: /tmp/nginx.stderr                           // non requis default:NULL      // oui
ok      env:                                                // non requis                   // oui
          STARTED_BY: taskmaster
          ANSWER: 42
      vogsphere:
        cmd: "/usr/local/bin/vogsphere-worker --no-prefork"
        numprocs: 8
        umask: 077
        workingdir: /tmp
        autostart: true
        autorestart: unexpected
        exitcodes: 0
        startretries: 3
        starttime: 5
        stopsignal: USR1
        stoptime: 10
        stdout: /tmp/vgsworker.stdout
        stderr: /tmp/vgsworker.stderr
  test_error:
    cmd: "ls            OK// erreur de syntax
    cmd ls              OK// erreur de syntax 
    numprocessus: 2     OK// parametre inconnue
    umask: test         OK// parametre du mauvais type
    umask: -1 => 511    OK// int to octal max 
    exitcodes: 0        // parametre du mauvais type list
    stoptime: -3        OK// valeur invalide
                        OK// parametre manquant obligatoire
                        // not program 

* numprocs -> nombres d'instances demarrer par supervisor
* umask -> masque attribuer a l'interieur du child pour les fichiers sur lesquels il travail ou va travailler
* autostart -> true or false -> demarre au lancement de supervisor

* autorestart -> false redemarre pas, unexpected redemarre seulement si exitcode n'est pas celui attendue, true redemarre peu importe sortie d'erreur
autorestart = redémarrage automatique — Il agit après que le processus a tourné correctement au moins startsecs secondes.

* exitcode -> attendu par autorestart pour unexpected 

* startretries -> nbre d'essaie de supervisor pour lancer le processus
startretries ≠ redémarrage automatique — Il gère uniquement la robustesse du démarrage initial.
* starttime -> nbre de sec ou le processus doit rester run after start pour conciderer qu'il est good

* stoptime -> supervisor envoie d’abord SIGTERM pour demander un arrêt "propre" du processus.
Il attend ensuite stoptime secondes pour laisser au programme le temps de se fermer correctement.
Si le programme ne s’est pas arrêté à temps, Supervisor envoie un SIGKILL pour le forcer à se terminer.
* stopsignal -> définit quel signal est envoyé au processus lorsqu’on lui demande de s’arrêter (stop)

arret par default de processus par supervisord =>
signal d'arret default sigterm, 
attend stopsecs, si processus tjs vivant
envoie sigkill tuer brutalement 
c'est donc sigterm qui est remplacer par stopsignal mais en cas de non reponse, c sigkill qui termine la danse


Etape demarrage supervisord :
1. État initial : STARTING
    supervisord exécute le processus (fork/exec).
    Il le considère dans l’état STARTING.
    À partir de là, il attend que le processus reste vivant pendant un certain temps, défini par le paramètre startsecs (par défaut 1 seconde).
2. Transition vers RUNNING
    Si le processus est toujours en vie après startsecs, alors :
        Il est considéré comme sain.
        supervisord change son état en RUNNING.
3. Si le processus meurt avant startsecs
    Alors, il n’a jamais atteint RUNNING.
    supervisord marque cela comme un échec de démarrage.
only startime RUNNING dis a supervisord si un processus a reussie meme si il retourne un exit 0

# BASE C++ revision Laura
:: Classes :: 

public, accessible depuis l'exterieur 
 private, membre accessibles uniquement a l'interieur de la classe
  this->, pointeur vers l'objet courant
   
    *, les pointeurs (stock une adresse et peut acceder a la valeur pointee)
    &, les references (Alias vers une variable existante) ou sert a obtenir une adresse
    ex :
        int x = 42; int *ptr = &x; // ptr stock l'adresse de x
        std::cout << ptr << std::endl; // affiche l'addr de x

        int x = 10; int &ref = x; // ref est un autre nom pour x
        ref = 20; std::cout << x << std::endl; // affiche 20 car ref modif x

        int x = 5; int *ptr = &x; // ptr contient l'addr de x
        *ptr = 10; // deference ptr pour modifier x
        std::cout << x; // affiche 10

        #####################################
        void modify(int n) {
            n = 100;
        }

        int main() {
            int x = 10;
            modify(x);     // x reste 10
        }
        #####################################
        void modify(int& n) {
            n = 100;
        }

        int main() {
            int x = 10;
            modify(x);    // x devient 100
        }
        #####################################
        void modify(int* p) {
            *p = 100;
        }

        int main() {
            int x = 10;
            modify(&x);   // x devient 100
        }
        #####################################

    Constructeur de copie, classname(const classname &src)
        cree un objet a partir d'un autre : Myclass a(10); Myclass b = a;
        const, protege l'objet original
        &, evite une copie inutile

    Operateur d'affectation, ClassName& operator=(const ClassName& src)
        appelee quand tu assigne un objet existant a un autre : Myclass a(10); Myclass b; b = a;
        retourner une ref sur this pour permettre les affectation en chaine

        #####################################
        // Constructeur normal
        MyString(const char* text) {
            data = new char[strlen(text) + 1];
            strcpy(data, text);
        }

        // Constructeur de copie
        MyString(const MyString& src) {
            data = new char[strlen(src.data) + 1];
            strcpy(data, src.data);
            std::cout << "Copie construite\n";
        }

        // Opérateur d'affectation
        MyString& operator=(const MyString& src) {
            if (this != &src) { // Toujours vérifier l'auto-affectation
                delete[] data; // Libère la mémoire actuelle
                data = new char[strlen(src.data) + 1];
                strcpy(data, src.data);
            }
            std::cout << "Affectation\n";
            return *this;
        }

        int main() {
        MyString a("Salut");
        MyString b = a;   // Constructeur de copie
        MyString c("Test");
        c = a;            // Opérateur d'affectation
        }
        ######################################


    Exception, throw, try, catch, 
        
        throw lance une exception
        #######################################
        double divide(double a, double b) {
            if (b == 0)
                throw std::runtime_error("Erreur : division par zéro !");
            return a / b;
        }

        int main() {
            try {
                double result = divide(10, 0);  // ← Provoque une exception
                std::cout << "Résultat : " << result << std::endl;
            }
            catch (const std::exception& e) {
                std::cerr << "Exception attrapée : " << e.what() << std::endl;
            }
        }
        #######################################
        Creer sa propre exception:
        #######################################
        class MonException : public std::exception {
        public:
            const char* what() const noexcept override {
                return "Ceci est une exception personnalisée !";
            }
        };

        int main() {
            try {
                throw MonException();
            }
            catch (const std::exception& e) {
                std::cerr << e.what(); // Affiche le message personnalisé
            }
        }
        #######################################

    new, allocation dynamique
    int* p = new int(5);
    int *tab = new int[3];

    delete p;
    delete[] tab;

    virtual,
    polymorphisme dynamique, 
        #######################################
        class Animal {
            public:
                virtual void makeSound() {
                    std::cout << "L'animal fait un bruit\n";
                }
            };

        class Dog : public Animal {
            public:
                void makeSound() override {
                    std::cout << "Le chien aboie : Woof!\n";
                }
            };

        Animal* a = new Dog();
        a->makeSound();  // Affiche : "Le chien aboie" (grâce au virtual)
        delete a;
        ######################################

    Template :
    permet d'ecrire du code generique, c a dire des fonctions ou des classes qui peuvent fonctionner avec differents types de donnees, sans dupliquer le code.

    template<typename T> ecris un modele de code qui fonctionne avc n'importe quel type T

        fonction Template
        ######################################
        template<typename T>
        T addition(T a, T b) {
            return a + b;
        }

        int x = addition<int>(2, 3);        // retourne 5
        double y = addition<double>(1.5, 2.3); // retourne 3.8
        std::string s = addition<std::string>("ab", "cd"); // retourne "abcd"

        ######################################
        Template avec class
        ######################################
        template<typename T>
        class Box {
        private:
            T value;

        public:
            Box(T v) : value(v) {}
            T get() const { return value; }
        };

        Box<int> intBox(42);
        Box<std::string> strBox("Hello");

        std::cout << intBox.get();  // 42
        std::cout << strBox.get();  // Hello

        #####################################

    static_cast<unsigned int>() , conversion de type explicit et sure

    Conteneurs sequentiels :
        std::vector / std::deque / std::list / std::forward_list / std::array 

    Conteneurs associatifs : 
        std::map / std::multimap

    Conteneur adaptatifs : 
        std::stack / std::queue

    empty() - size() - insert() - erase() - clear() - begin() / end() - find()
    

    Surcharge de l'opérateur (ex : << -> operateur d'insertion utilisee avc le flux std::cout),
        definir comment il se comporte pour des types personnalisees


ancienne verison :

enum ProcessStatus {
    START,          // Le process est en cours de démarrage (juste forké)
    STARTING,       // Process en train de démarrer (peut-être juste après fork)
    DOWN,           // Process terminé normalement (exit code 0)
    UNEXPECT_DOWN,  // Process terminé de façon inattendue (exit code != 0 ou signal)
    SHUTING_DOWN    // Process en cours d'arrêt contrôlé (ex: signal demandé)
};
