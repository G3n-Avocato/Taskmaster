# NOTE Taskmaster
C++

1- Implementation de la gestion basique des processus
    lancer un processus enfant avec - FORK()
    surveiller son etat - STD::THREAD
    envoyer des signaux SIGKILL pour arreter proprement
    recuperer la sortie standard / erreur

2- Supervision + redemarrage automatique 
    implementer systeme de boucle pour verifier etat
    suivre politique de redemarrage

3- Gestion des logs
    capturer stdout / stderr des processus dans fichier
    mettre en place rotation des logs (size nbre fichier)

4- Gestion Config 
    format (YAML) 
    list parametre proposer fichier 
    charger et PARSER FICHIER de config au demarrage 
    permettre le rechargement dynamique

5- Interface user / controller 
    en ligne de commande (start stop statut etc)

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

BASE de C++
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
