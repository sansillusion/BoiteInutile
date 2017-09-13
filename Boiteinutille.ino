//  _ ___ _______     ___ ___ ___  ___ _   _ ___ _____ ___ 
// / |_  )__ /   \   / __|_ _| _ \/ __| | | |_ _|_   _/ __| 
// | |/ / |_ \ |) | | (__ | ||   / (__| |_| || |  | | \__ \ 
// |_/___|___/___/   \___|___|_|_\\___|\___/|___| |_| |___/ 
// 
// Boite inutille
// 
// Made by sansillusion sansillusion
// License: GPL 3.0
// Downloaded from: https://circuits.io/circuits/4651196-boite-inutille

/* Boite sans usage de Steve Olmstead
    ecrit sans essait (car je n'ai pas de servo et autres pieces pour essayer)
    ATTENTION !!!! pour connecter les moteurs DC utiliser un H bridge comme SN754410NE ou L293D !!!!
    version 0.5
    changelog :
    v0.2 : creation de fonctions pour minimiser les codes repetitifs et simplification de la programmation futur.
    v0.3 : optimisation de la gestion du pouvoir
      (transistor pour controler le power des servos et preparation pour le capteur de proximitee)
        et ajout de random() a plusieurs endroits pour randomizer les choix de duree, claquements, vitesse etc...
    v0.4 : ajout de calibration pour les servo moteurs (afint de ne pas les forcer plus loin que necessaire)
      la calibration est indepedante pour le couvert et le bras.
        Ajout de calibration de la vitesse maximum des moteurs DC pour le deplacement.
        Retirer l'option de degree dans la fonction cerveaux() pour etre certain de tomber au bon endroit
        utiliser vitesse ou sinon placer d'un coup avec nouvelle fonction placer()
    v0.5 : bugfix de la calibration de vitesse maximale des moteurs de roues et simplification de codes.

  sansillusion@gmail.com
*/

#include <Servo.h>
int calibC[] = {10, 100}; // calibration du minimum et maximum pour la porte 'de 0 a 180'
int calibB[] = {8, 160}; // calibration du minimum et maximum pour le bras 'de 0 a 180'
int calibM = 255; // calibration de la vitesse maximum pour les moteurs 1 a 255
int tempDeDeplacement = 1600; // temps que le servo le plus lent prend a faire un trajet complet en ms
const byte switchPin = 2; // reglage de la pin de la switch pour le interupt
int moteurun = 5; // configure le moteur un sur une pin pwm
int moteurunx = 3; // configure le moteur un sur une pin pwm
int moteurdeux = 11; // configure le moteur deux sur une pin pwm
int moteurdeuxx = 6; // configure le moteur deux sur une pin pwm
int senseur = 7; // choicis la pin pour le senseur 'proximite ou touch sur la switch'
int couvertPin = 9; // choisis la pin pour le servo du couvert
int brasPin = 10; // choisis la pin pour le servo du bras
int dpouvoir = 12; // regle la pin por le controle de power du detecteur de proximitee
int pouvoir = 13; // regle la pin por le controle de power des servo
Servo couvert; // Creation du servo pour le couvert
Servo bras; // celui pour le bras qui ferme la switch
int cpos; // position du couvert
int bpos; // position du bras
volatile int ouverture = 0; // le couvert est fermer

void setup() {
  //  Serial.begin(9600);
  couvert.attach(couvertPin); // attache le servo du couvert
  bras.attach(brasPin); // attache le servo du bras
  pinMode(senseur, INPUT) ; // met le senseur en mode input
  pinMode(switchPin, INPUT_PULLUP); // reglage du mode de la pin de la switch pour un interrupt
  attachInterrupt(digitalPinToInterrupt(switchPin), ouvert, RISING); // cree un interupt avec la switch qui execute la fonction 'ouvert'
  pinMode(pouvoir, OUTPUT); // power controle des servos en output
  digitalWrite(pouvoir, LOW); // aucun power au startup
  pinMode(dpouvoir, OUTPUT); // power controle en output
  digitalWrite(dpouvoir, LOW); // aucun power au startup
  pinMode(moteurun, OUTPUT); // moteur en output
  pinMode(moteurdeux, OUTPUT); // duh!
  pinMode(moteurunx, OUTPUT); // deuxiemme pin moteur en output
  pinMode(moteurdeuxx, OUTPUT); // duh!
  digitalWrite(moteurun, LOW); // on fait sur quils bouge pas
  digitalWrite(moteurunx, LOW);
  digitalWrite(moteurdeux, LOW);
  digitalWrite(moteurdeuxx, LOW);
  cpos = map(0, 0, 180, calibC[0], calibC[1]); // calibre selon les valeurs de calibration dans la configuration ci haut
  bpos = map(0, 0, 180, calibB[0], calibB[1]); // calibre selon les valeurs de calibration dans la configuration ci haut
  servopower(1); // allume les servos
  bras.write(bpos); // dit au couvert d'aller a la position 0 selon la calibration
  delay(tempDeDeplacement); // laisse le temps au bras de retrer si yer sorti
  couvert.write(cpos); // dit au couvert d'aller a la position 0 selon la calibration
  delay(tempDeDeplacement); // laisse le temps au couvert de fermer si yer ouvert
  servopower(0); // eteint les servos
}

void loop() {
  ouverture = digitalRead(switchPin); // check l'etat de la switch au cas ou on aurrais manquer l'interrupt 'eg: la switch est flipper pendant la retraction du bras ou la fermeture du couvert'
  while (ouverture == 0) { //tant qu'on as pas eu d'interrupt de la switch
    int azar = random(1, 9); //choisis au hazard entre 1 et 8 'le nombre de case, default au cas ou y choisirais 8 mais il la jamais fait a datte'
    switch (azar) {
      case 1:
        conteur(8000); // on attend pour un interrupt de la switch pour 8 secondes
        break;

      case 2:
        conteur(random(1000, 30000)); // on attend pour un interrupt de la switch entre 1 et 30 secondes
        break;

      case 3:
        if (detecteur(random(5, 20)) == true) { // on lance le detecteur pour 5 a 20 secondes et si y detect on fait quoi
          if (ouverture == 1) { // faut casser le switch case si on as un interrupt de la switch
            break;
          }
          //mouvement detecte ou toucher faire bouger boite ici
          int haha = random(100, 500); // on choisis un delais d'attente en ms
          int fun = random(10, 120); // on choisis aux hazard un vitesse de deplacement entre 10 et 120 'valide entre 1 et 255'
          deuxroue(fun, 0);
          conteur(haha);
          arrete();
          conteur(random(400, 2000));
          deuxroue(fun, 1);
          conteur(haha);
          arrete();
        }
        break;

      case 4:
        if (detecteur(random(5, 20)) == true) { // on lance le detecteur pour 5 a 20 secondes et si y detect on fait quoi
          if (ouverture == 1) { // faut casser le switch case si on as un interrupt de la switch
            break;
          }
          //mouvement detectee ou toucher faire bouger boite ici
          int haha = random(100, 600); // on choisis un delais d'attente en ms
          int fun = random(20, 150); // on choisis aux hazard un vitesse deplacement entre 20 et 150 'valide entre 1 et 255'
          deuxroue(fun, 1);
          conteur(haha);
          arrete();
          conteur(1000);
          deuxroue(fun, 0);
          conteur(haha);
          arrete();
        }
        break;

      case 5:
        if (detecteur(10) == true) { // on lance le detecteur pour combien de secondes et si y detect on fait quoi
          if (ouverture == 1) { // faut casser le 'switch case' si on as un interrupt de la switch
            break;
          }
          //detecter mouvement ou toucher faire bouger boite ici
          int haha = random(500, 1500); // on choisis un delais d'attente en ms
          int fun = random(50, 255); // on choisis aux hazard un vitesse de rotation entre 50 et 255 'valide entre 1 et 255'
          tourne(fun, 1); // on fait tourner dans un sens
          conteur(haha); // on attend
          arrete();
          conteur(haha);
          tourne(fun, 0);
          conteur(haha);
          arrete();
        }
        break;

      case 6:
        conteur(random(1000, 20000));
        break;

      case 7:
        conteur(random(1000, 30000));
        break;
      
      case 8:
      	conteur(random(2000, 60000));
      	break;

      default:
        if (detecteur(10) == true) { // on lance le detecteur pour combien de secondes et si y detect on fat quoi
          if (ouverture == 1) { // faut casser le switch case si on as un interrupt de la switch
            break;
          }
          //detecter mouvement ou toucher faire bouger boite ici
          int haha = random(100, 600); // on choisis un delais d'attente en ms
          int fun = random(20, 100); // on choisis aux hazard un vitesse de rotation entre 20 et 100 'valide entre 1 et 255'
          deuxroue(fun, 1);
          conteur(haha);
          arrete();
          conteur(1000);
          deuxroue(fun, 0);
          conteur(haha);
          arrete();
        }
        break;
    }
  }
  // ici sont les commandes pour quand la switch est ouverte
  int azar = random(1, 7); // choisis aux hazard entre les choix si bas 'case 1 a x'
  switch (azar)
  {
    case 1:
      cerveaux(1, 180, random(5, 35));
      cerveaux(0, 180, random(2, 22));
      cerveaux(0, 0, random(3, 30));
      cerveaux(1, 0, random(3, 45));
      break;

    case 2:
      cerveaux(1, 180, random(2, 25));
      cerveaux(0, 180, random(2, 25));
      delay(random(100, 3000));
      cerveaux(0, 0, random(2, 25));
      cerveaux(1, 0, random(2, 25));
      break;

    case 3:
      cerveaux(1, 180, 15); // couvert, step de 1 degree, vas a 180 degree, delais de 15ms entre chaque step
      cerveaux(0, 180, 15); // bras, step de 1 degree, vas a 180 degree, delais de 15ms entre chaque step
      cerveaux(0, 0, 15); // bras, step de 1 degree, vas a 0 degree, delais de 15ms entre chaque step
      //claquement de la porte
      claque(0, 180, 200, random(2, 8));
      delay(random(500, 1500)); // attend
      claque(0, 20, 100, random(3, 10));
      break;

    case 4:
      cerveaux(1, random(30, 90), random(15, 40));
      delay(random(500, 2500));
      cerveaux(1, 0, random(5, 30));
      delay(random(500, 1000));
      ouverture = digitalRead(switchPin); // check l'etat de la switch
      if (ouverture == 0) {
        break;
      }
      cerveaux(1, 180, random(10, 35));
      cerveaux(0, 180, random(15, 40));
      cerveaux(0, 0, random(5, 30));
      claque(0, 180, 200, random(2, 10));
      delay(1000);
      claque(90, 120, 10, random(20, 100));
      delay(1000);
      claque(0, 30, random(80, 160), random(3, 10));
      break;

    case 5:
      cerveaux(1, random(30, 90), random(15, 40));
      delay(random(500, 2500));
      cerveaux(1, 0, random(5, 30));
      delay(random(500, 1000));
      ouverture = digitalRead(switchPin); // check l'etat de la switch
      if (ouverture == 0) {
        break;
      }
      cerveaux(1, 180, random(10, 35));
      cerveaux(0, 180, random(1, 40));
      cerveaux(0, 0, random(5, 30));
      cerveaux(1, 0, random(1, 40));
      break;

    case 6:
      cerveaux(1, random(30, 90), random(15, 40));
      delay(random(500, 2500));
      cerveaux(1, 0, random(5, 30));
      delay(random(500, 1000));
      break;

    default:
      cerveaux(1, 180, 5); // couvert, vas a 180 degree, delais de 5ms entre chaque step
      cerveaux(0, 180, 5); // bras, vas a 180 degree, delais de 5ms entre chaque step
      cerveaux(0, 0, 5); // bras, vas a 180 degree, delais de 5ms entre chaque step
      //claquement de la porte
      claque(0, 180, 200, 5); //claque de 180 a 0 degree avec 200ms de delais entre chaques fois et fait le 5 fois
      break;
  }
}

/* les fonctions utilisee dans ce sketch sont ici bas */

void ouvert() {
  ouverture = 1;
}

void servopower(int etat) { // 1 egal allumer 0 egal eteint
  if ( etat == 0 ) {
    if (calibC[0] == cpos) {
      digitalWrite(pouvoir, etat); // set le power a off
    }
  } else {
    digitalWrite(pouvoir, etat); // set le power
  }
}

void detectpower(int etat) { // 1 egal allumer 0 egal eteint
  digitalWrite(dpouvoir, etat); // set le power
}

void conteur(int letemp) {
  int loopeur = 0;
  while (loopeur <= letemp) {
    delay(10);
    loopeur = loopeur + 10;
    if (ouverture == 1) { // faut casser le while loop si on as un interrupt de la switch
      break;
    }
  }
}

void arrete() { //on arrete les deux moteurs DC (roues)
  digitalWrite(moteurun, LOW);
  digitalWrite(moteurdeux, LOW);
  digitalWrite(moteurunx, LOW);
  digitalWrite(moteurdeuxx, LOW);
}

void deuxroue(int vitesse, int parou) { // fait bouger la boite dans une direction selon 'parou' a une vitesse selon 'vitesse'
  vitesse = map(vitesse, 0, 255, 0, calibM); // calibre selon les valeurs de calibration dans la configuration ci haut
  if (parou == 1) { // on choisis la direction selon 'parou'
    digitalWrite(moteurun, LOW); // on assure la bonne polarite des moteurs pour la direction
    digitalWrite(moteurdeux, LOW);
    analogWrite(moteurunx, vitesse); // on ajuste la vitesse des moteurs a 'vitesse' 1 a 255
    analogWrite(moteurdeuxx, vitesse);
  } else {
    digitalWrite(moteurunx, LOW);
    digitalWrite(moteurdeuxx, LOW);
    analogWrite(moteurun, vitesse);
    analogWrite(moteurdeux, vitesse);
  }
}

void tourne(int vitesse, int parou) { // fait tourne la boite sur elle meme dans une direction selon 'parou' a une vitesse selon 'vitesse'
  vitesse = map(vitesse, 0, 255, 0, calibM); // calibre selon les valeurs de calibration dans la configuration ci haut
  if (parou == 1) { // on choisis la direction selon 'parou'
    digitalWrite(moteurun, LOW); // on assure la bonne polarite des moteurs pour la direction
    digitalWrite(moteurdeuxx, LOW);
    analogWrite(moteurunx, vitesse); // on ajuste la vitesse des moteurs a 'vitesse' 1 a 255
    analogWrite(moteurdeux, vitesse);
  } else {
    digitalWrite(moteurunx, LOW);
    digitalWrite(moteurdeux, LOW);
    analogWrite(moteurun, vitesse);
    analogWrite(moteurdeuxx, vitesse);
  }
}

int detecteur(int temps) {
  int sense = 0;
  temps = temps * 1000; // convertit teps en millisecondes
  int loopeur = 0;
  detectpower(1);
  delay(50);
  while ( loopeur <= temps) // attend la fin du temps
  {
    sense = digitalRead(senseur) ; // lit la pin du senseur pour son statut
    if (sense == HIGH) {
      detectpower(0);
      return 1;
    }
    if (ouverture == 1) { // faut casser le while loop si on as un interrupt de la switch
      detectpower(0);
      return 0;
    }
    delay(20);
    loopeur = loopeur + 20;
  }
  detectpower(0);
  return 0;
}

void cerveaux(int quel, int ouca, int delais) {
  servopower(1); // allume les servos
  delay(50);
  if (quel == 1) { // 1 = couvert 0 = bras
    ouca = map(ouca, 0, 180, calibC[0], calibC[1]); // calibre selon les valeurs de calibration dans la configuration ci haut
    if (cpos < ouca) { //verifie dans quel sense on doit aller selon la potition courrante
      for (cpos = cpos; cpos < ouca; cpos++) { // on ouvre le couvert par step de 1 degee
        couvert.write(cpos);              // dit au couvert d'aller a la position 'cpos'
        delay(delais);                       // attend 'delais' entre chaque step
      }
    } else {
      for (cpos = cpos; cpos > ouca; cpos--) { // on ferme le couvert par step de 1 degee
        couvert.write(cpos);              // dit au couvert d'aller a la position 'cpos'
        delay(delais);                       // attend 'delais' entre chaque step
      }
    }
  } else {
    ouca = map(ouca, 0, 180, calibB[0], calibB[1]); // calibre selon les valeurs de calibration dans la configuration ci haut
    if (bpos < ouca) {
      for (bpos = bpos; bpos < ouca; bpos++) { // on sort le bras par step de 1 degee
        bras.write(bpos);              // dit au bras d'aller a la position 'bpos'
        delay(delais);                       // attend 'delais' entre chaque step
      }
    } else {
      for (bpos = bpos; bpos > ouca; bpos--) { // on retracte le bras par step de 1 degee
        bras.write(bpos);              // dit au bras d'aller a la position 'bpos'
        delay(delais);                       // attend 'delais' entre chaque step
      }
    }
  }
  delay(tempDeDeplacement); // fait sure que le servo est rendu ou il est supposer
  servopower(0); // eteint les servos
}

void claque(int minim, int maxim, int vitte, int fois) { // fonction pour faire vibrer/claquer le couvert
  minim = map(minim, 0, 180, calibC[0], calibC[1]); // calibre selon les valeurs de calibration dans la configuration ci haut
  maxim = map(maxim, 0, 180, calibC[0], calibC[1]); // calibre selon les valeurs de calibration dans la configuration ci haut
  servopower(1); // allume les servos
  for (int compte = 0; compte < fois; compte++) {
    couvert.write(maxim);              // dit au couvert d'aller a la position maxim
    delay(vitte);                       // attend 'vitte'ms
    couvert.write(minim);              // dit au couvert d'aller a la position minim
    delay(vitte);                       // attend 'vitte'ms
    cpos = minim;
  }
  delay(tempDeDeplacement); // fait sure que le servo est rendu ou il est supposer
  servopower(0); // eteint les servos
}
