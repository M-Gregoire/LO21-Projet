#include "controleur.h"
#include "../litterales/litterales.h"
#include "../operateurs/operateurs.h"
#include "../GUI/mainwindow.h"
#include <sstream>
#include <string>

void Controleur::executer()
{

        // On recupere le haut de la pile
        Operande* topOperande = pile.top();

        // Si c'est un opérateur
        Operateur* monOperateur = dynamic_cast<Operateur*>(topOperande);
        if(monOperateur)
        {
            // On l'enleve de la pile
            pile.pop();
            // On verifie son arite
            Unaire* operateurUnaire = dynamic_cast<Unaire*>(monOperateur);
            Binaire* operateurBinaire = dynamic_cast<Binaire*>(monOperateur);

            if(operateurUnaire)
            {
                if(pile.size()>=1)
                {
                }
                else
                {
                    // Si on arrive ici, c'est qu'il n'y a pas assez d'élément dans la pile
                    MainWindow::getInstance()->setMsg("Erreur : Pas assez d'élément !");
                }
            }
            if(operateurBinaire)
            {
                // Si c'est un opérateur binaire, il faut que la pile soit supérieur ou égale à 2
                if(pile.size()>=2)
                {
                // On recupere la premiere opérande
                Operande* v1 = pile.top();

                Litterale* l1 = dynamic_cast<Litterale*>(v1);
                // Si ce n'est pas une littérale, c'est un autre opérateur, on relance donc execution
                if(!l1)
                    this->executer();

                // On enleve la littérale de la pile
                pile.pop();

                // On recupere le deuxieme opérande
                Operande* v2 = pile.top();

                Litterale* l2 = dynamic_cast<Litterale*>(v2);
                // Si ce n'est pas une littérale, on relance executer
                if(!l2)
                    this->executer();

                // On enleve la littérale de la pile
                pile.pop();

                //On cree une serie de bool pour un test ulterieur
                bool isDiv=false;
                // On cree un pointeur vers le resultat
                Litterale* res = 0;
                // On fait le calcul correspondant à l'opérateur (qui renvoie un pointeur vers le résultat)
                if(operateurBinaire->getValue() == "+")
                    res=(*l1+*l2);
                if(operateurBinaire->getValue() == "-")
                    res=(*l1-*l2);
                if(operateurBinaire->getValue() == "*")
                    res=(*l1**l2);
                if(operateurBinaire->getValue() == "/")
                    res=(*l1 / *l2);
                if(operateurBinaire->getValue() == "DIV")
                {
                    isDiv=true;
                    Entier* e1 = dynamic_cast<Entier*>(l1);
                    Entier* e2 = dynamic_cast<Entier*>(l2);

                    if (e1&&e2)
                    {
                        res=DIV(e1,e2);
                        this->pile.push(res);
                        MainWindow::getInstance()->setMsg("Calcul effectué !");
                    }
                    else
                    {
                        MainWindow::getInstance()->setMsg("Erreur : DIV ne peut s'appliquer que sur deux entiers !");
                        //On re-empile
                        this->pile.push(l2);
                        this->pile.push(l1);
                    }
                }

                if (!isDiv)
                {
                // On met le resultat en haut de pile
                this->pile.push(res);

                MainWindow::getInstance()->setMsg("Calcul effectué !");
                }

                }
                else
                {
                    // Si on arrive ici, c'est qu'il n'y a pas assez d'élément dans la pile
                    MainWindow::getInstance()->setMsg("Erreur : Pas assez d'élément !");
                }
            }


        }
        // Si ce n'est pas un opérateur
        else
        {
            // Alors c'est une littérale
            Litterale* l1 = dynamic_cast<Litterale*>(topOperande);
            // On l'ajoute à la pile
            this->pile.push(l1);

        }

}

// Renvoi la pile sous la forme d'un string
std::string Controleur::pileString()
{
    // On copie la pile dans une pile tampon
    std::stack<Operande*> tampon = pile;

    // Operande buffer
    Operande* op;

    // Stringstream qui contient la pile
    std::stringstream result;

    // Tant que le tampon n'est pas vide
    while(!tampon.empty())
    {
        // On depile, et on ajoute au string de retour
        op=tampon.top();
        tampon.pop();
        op->afficher(result);
        result<<std::endl;
    }
    // Si la pile est vide, on le dit !
    if(result.rdbuf()->in_avail()==0) // Test si result contient des caracteres
        return "Pile vide";
    // Sinon, on renvoie le resultat
    return result.str();
}

// Permet de separer une commande en différentes opérandes : "3 3 +" empilera 3 puis empilera 3 avant d'empiler + et d'executer la pile
void Controleur::commande(std::string cmd)
{
    std::istringstream iss(cmd);
    std::string sub;
    iss >> sub;

     while(iss)
     {
         std::string t = sub.c_str();
         // A ce stade, sub contient la partie du string entre des espaces
         // Si ce string est un opérateur, on l'ajoute à l'aide de la FactoryOperateur
         if(estOperateur(t))
         {
             this->empiler(FactoryOperateur::getInstance(),sub);
             // SI c'est un opérateur, on execute !
             this->executer();
         }
         // Sinon, on considere que c'est une litterale
         else
         {
             this->empiler(FactoryLitterale::getInstance(),sub);

         }

         iss >> sub;
     }

}

// Initialisation de l'attribut statique
Controleur::Handler Controleur::handler = Controleur::Handler();

// Singleton
Controleur& Controleur::getInstance()
{
    if(handler.instance==nullptr) {
    handler.instance=new Controleur;
}
return *handler.instance;
}


void Controleur::libererInstance()
{
  delete handler.instance;
  handler.instance=nullptr;

}


