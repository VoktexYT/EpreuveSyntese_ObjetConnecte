/*
*	Projet : Épreuve synthèse Volet A
*	Fichier : main.cpp
*	Auteur : Ubert Guertin
*	Date : 2025/04/27
*	Description : La première partie de l'épreuve synthèse.
*/

// Fichiers projets
#include "main.h"


// Instances globales
// SHT31 capteurTemperatureHumidite;
ANSI ansi(&Serial);
rgb_lcd ecran;
DS1307 horlotge;
SoftwareSerial portSerieTransmetteur(PIN_UART_D9, PIN_UART_D8); // RX, TX
Systeme systemE2467525;
BME280 bmp280;

// Variables globales
char etatChangementUniteTemperature = F_C_DEFAUT_TEMPERATURE;


// Fonction de démarage
void setup() {
	Serial.begin(VITESSE_UART);
	// capteurTemperatureHumidite.begin();
	bmp280.init();
	portSerieTransmetteur.begin(19200);

	ansi.clearScreen();

	initialiserBroches();
	initialiserEcranLcd();

	#ifdef DEBUG
		Serial << "Démarage du programme dans 2 secondes..." << endl;
		delay(2000);
		ansi.clearScreen();
	#endif

	afficherStationControle();
} // setup()



// Boucle du programme
void loop() {
	static int ancienneValeurPotentiometre = 0;
	int valeurPotentiometre = map(
		analogRead(PIN_POTENTIOMETRE), 
		POTENTIOMETRE_MIN, 
		POTENTIOMETRE_MAX, 
		POTENTIOMETRE_MAP_MIN,
		POTENTIOMETRE_MAP_MAX
	);

	bool estValeurPotentiometreChangee = ancienneValeurPotentiometre != valeurPotentiometre;

	ansi.gotoXY(10, 40);
	Serial << (estValeurPotentiometreChangee ? "TRUE " : "FALSE") << " : " << ancienneValeurPotentiometre << " != " << valeurPotentiometre;

	static unsigned long int tempsAvant = millis();
	static unsigned long int tempsAvant2 = millis();

	static bool etatLed1 = ON_OFF_DEFAUT_LED_1;
	static bool etatLed2 = ON_OFF_DEFAUT_LED_2;
	static bool etatRelai = ON_OFF_DEFAUT_RELAI;
	static bool etatAlarme = ON_OFF_DEFAUT_ALARME;
	static bool etatDetecteurMouvement = ON_OFF_DEFAUT_DETECTEUR_MOUVEMENT;
	static bool etatLCDRetroEclarage = ON_OFF_DEFAUT_RETRO_ECLAIRAGE;

	static unsigned int etats = 0;
	static unsigned long int debutTemps = millis();

	static unsigned long int debutTempsDetecteurMouvement = 0;

	verifierEtatDetecteurMouvement(
		etatAlarme,
		etatDetecteurMouvement,
		etatLCDRetroEclarage,
		valeurPotentiometre * SECONDE,
		debutTempsDetecteurMouvement,
		estValeurPotentiometreChangee
	);

	// Calculer le temps écoulé avant d'envoyer les données
	if (millis() - tempsAvant >= valeurPotentiometre * SECONDE) {
		tempsAvant = millis();
	}

	if (millis() - tempsAvant2 >= SECONDE) {
		tempsAvant2 = millis();
		changerValeurEtatBinaire(
			etats, 
			!etatLed1, 
			!etatLed2,
			!etatRelai, 
			etatChangementUniteTemperature == SYMBOLE_CELCIUS, 
			etatDetecteurMouvement, 
			etatLCDRetroEclarage,
			false,
			etatAlarme
		);
		
		changerValeurSysteme(etats, valeurPotentiometre);
		
		portSerieTransmetteur.write((char*)&systemE2467525, sizeof(systemE2467525));
		portSerieTransmetteur.flush();	
	}

	afficherTempsEcoule((millis() - debutTemps) / SECONDE);
	afficherDateActuelle();
	afficherTemperatureHumidite();
	afficherValeurPotentiometre(valeurPotentiometre);
	afficherEtatAlarme(etatAlarme);
	afficherEtatBinaire(etats);

	ancienneValeurPotentiometre = valeurPotentiometre;
	// Changement d'états des composants avec les touches du clavier
	if (!Serial.available()) return;

	char commande = Serial.read();
	afficherOption(commande);
	executerCommande(commande,
		etatLed1, etatLed2, etatRelai, etatChangementUniteTemperature,
		etatDetecteurMouvement, etatLCDRetroEclarage);
} // loop()


// Permet d'afficher chaque bit dans un octet
void afficherBinaire(unsigned int valeur) 
{
  for (int i = 7; i >= 0; i--) 
  {  // 16 bits pour un unsigned int
    Serial.print((valeur >> i) & 1);  // Décale et applique un masque pour extraire le bit
  }
}

// Insertion des données dans la structure Systeme
void changerValeurSysteme(unsigned int& etats, int valeurPotentiometre) {
	horlotge.getTime();

	strcpy(systemE2467525.nomSysteme, NOM_SYSTEME);

	systemE2467525.codeMessage = Evenements::evenement_statut_appareils;
	
	systemE2467525.heure   = horlotge.hour;
	systemE2467525.minute  = horlotge.minute;
	systemE2467525.seconde = horlotge.second;

	systemE2467525.appareils.etatAppareils = etats;
	systemE2467525.appareils.temperature   = bmp280.getTemperature();
	systemE2467525.appareils.humidite      = bmp280.getHumidity();
	systemE2467525.appareils.dureeAlarme   = valeurPotentiometre;

	float pressure = bmp280.getPressure();
	systemE2467525.appareils.altitude      = bmp280.calcAltitude(pressure);
	systemE2467525.appareils.pressionAtomospherique = pressure;
} // initialiserSysteme()


// Place tous les bits pour former la variable 'etats'
void changerValeurEtatBinaire(unsigned int &etats, bool etatLed1, bool etatLed2, bool etatRelai, bool etatChangementUniteTemperature, bool etatDetecteurMouvement, bool etatRetroEclairage, bool etatBoutonPanique, bool mouvementDetecte) {
	etats = 0;
	etats |= etatLed1                                         ? APP_LED1           : 0;
	etats |= etatLed2                                         ? APP_LED2           : 0;
	etats |= etatRelai                                        ? APP_RELAI          : 0;
	etats |= etatDetecteurMouvement                           ? APP_MOUVEMENT_ARME : 0;
	etats |= etatBoutonPanique                                ? APP_BOUTON_PANIQUE : 0;
	etats |= etatRetroEclairage                               ? APP_RETRO_LCD      : 0;
	etats |= etatChangementUniteTemperature                   ? APP_UNITE_TEMP     : 0;
	etats |= mouvementDetecte                                 ? APP_MOUVEMENT      : 0;
} // changerValeurEtatBinaire()


void afficherValeurPotentiometre(int valeur) {
	ansi.gotoXY(POSITION_X_ETAT, POSITION_Y_VALEUR_POTENTIOMETRE);
	Serial << F("        ");
	ansi.gotoXY(POSITION_X_ETAT, POSITION_Y_VALEUR_POTENTIOMETRE);
	Serial << valeur << F(" sec");
} // afficherValeurPotentiometre()


void afficherEtatAlarme(bool etatAlarme) {
	afficherOnOff(POSITION_X_ETAT, POSITION_Y_ETAT_ALARME, etatAlarme);
} // afficherEtatAlarme()


// Exécution des fonctions liées aux commandes
void executerCommande(char commande, bool& etatLed1, bool& etatLed2, bool& etatRelai, char& etatChangementUniteTemperature, bool& etatDetecteurMouvement, bool& etatLCDRetroEclarage) {
	switch (commande) {
		case COMMANDE_LED_1:
			etatLed1 = !etatLed1;
			changerValeurEtat(POSITION_Y_LED1, PIN_LED_1);
			break;
		case COMMANDE_LED_2:
			etatLed2 = !etatLed2;
			changerValeurEtat(POSITION_Y_LED2, PIN_LED_2);
			break;
		case COMMANDE_RELAI:
			etatRelai = !etatRelai;
			changerValeurEtat(POSITION_Y_RELAI, PIN_RELAI);
			break;
		case COMMANDE_CHANGEMENT_UNITE_TEMPERATURE:
			changerValeurTemperature(etatChangementUniteTemperature);
			break;
		case COMMANDE_DETECTEUR_MOUVEMENT:
			etatDetecteurMouvement = !etatDetecteurMouvement;
			changerTexteEtatAlarme(etatDetecteurMouvement);
			break;
		case COMMANDE_RETRO_ECLAIRAGE:
			etatLCDRetroEclarage = !etatLCDRetroEclarage;
			afficherOnOff(POSITION_X_ETAT, POSITION_Y_RETRO_ECLARAGE, etatLCDRetroEclarage);
			break;
		default:
			break;
	}
}


// Changer la couleur du LCD
void changerCouleurLCD(bool etatDetecteurMouvement, bool retroEclairage) {
	if (!retroEclairage) {
		ecran.setRGB(0, 0, 0);
		return;
	}

	if (etatDetecteurMouvement) {
		ecran.setRGB(255, 0, 0);
		return;
	}

	ecran.setRGB(255, 255, 255);
} // changerCouleurLCD()


// Changer l'affichage du LCD
void afficherLCDIntrusion(bool estIntrusion) {
	ecran.clear();

	if (estIntrusion) {
		ecran.print("** INTRUSION **");
		ecran.setCursor(0, 1);
		ecran.print("-> 00:00:00");
		return;
	}

	ecran.print(F("Alarme : ON"));
	ecran.setCursor(0, 1);
	ecran.print(F("-> 00:00:00"));
} // afficherLCDIntrusion()


// Faire sonner le buzzer si true, sinon arrêter
void changerEtatBuzzer(int nouvelEtat) {
	digitalWrite(PIN_BUZZER, nouvelEtat);
} // changerEtatBuzzer()


// Exécuter des fonctions d'alerte, si il y a du mouvement
void verifierEtatDetecteurMouvement(bool etatAlarme, bool& etatDetecteurMouvement, bool etatLCDRetroEclarage, int longueurTempsAlarme, unsigned long int& tempsDebut, bool estValeurPotentiometreChangee) {
	static bool premiereFoisActive = true;

	if (etatAlarme) {
		bool bouttonPaniqueOuDetecteurMouvementActive = digitalRead(PIN_DETECTEUR_MOUVEMENT) || digitalRead(PIN_BOUTON);

		if (bouttonPaniqueOuDetecteurMouvementActive) {
			tempsDebut = millis();
		}

		if (!premiereFoisActive) {
			etatDetecteurMouvement = (tempsDebut + longueurTempsAlarme > millis());
		}

		if (estValeurPotentiometreChangee) {
			etatDetecteurMouvement = false;
			tempsDebut = 0;
		}

		changerEtatBuzzer(etatDetecteurMouvement);
		afficherLCDIntrusion(etatDetecteurMouvement);
		changerCouleurLCD(etatDetecteurMouvement, etatLCDRetroEclarage);
		
		#ifdef DEBUG
			ansi.gotoXY(0, 23);
			Serial << "Etat du detecteur de mouvement: " << etatDetecteurMouvement;
		#endif

		if (premiereFoisActive) {
			premiereFoisActive = false;
		}
	}
} // verifierEtatDetecteurMouvement()


// Changement de l'unité de température F & C dans l'interface
void changerValeurTemperature(char& etatChangementUniteTemperature) {
	etatChangementUniteTemperature = 
		etatChangementUniteTemperature == SYMBOLE_CELCIUS ? SYMBOLE_FAHRENHEIT : SYMBOLE_CELCIUS;	
	ansi.gotoXY(POSITION_X_ETAT, POSITION_Y_UNITE_TEMPERATURE);
	Serial << etatChangementUniteTemperature;
} // changerValeurTemperature()


// Vérifie si le bouton est appuyé et exécute COMMANDE_DETECTEUR_MOUVEMENT
// void verifierEtatBouton(bool& etatAlarme) {
// 	static bool estDejaAppuye = false;
// 	bool etatActuel = digitalRead(PIN_BOUTON);

// 	if (etatActuel && !estDejaAppuye) {
// 		estDejaAppuye = true;
// 		etatAlarme = !etatAlarme;
// 		changerTexteEtatAlarme(etatAlarme);
// 	}

// 	else if (!etatActuel && estDejaAppuye) {
// 		estDejaAppuye = false;
// 	}
// } // verifierEtatBouton()


// Changement du texte du detecteur de mouvement dans l'interface (Activé / Désactivé)
void changerTexteEtatAlarme(bool etatAlarme) {
	ansi.gotoXY(POSITION_X_ETAT, POSITION_Y_DETECTEUR_MOUVEMENT);
	Serial << F("          ");
	ansi.gotoXY(POSITION_X_ETAT, POSITION_Y_DETECTEUR_MOUVEMENT);
	Serial << (etatAlarme ? TEXT_ACTIVE : TEXT_DESACTIVE);
} // changerTexteDetecteurMouvement()


// Changement générique de l'état d'un bool. Affichage ON OFF par rapport au bool 
void changerValeurEtat(int positionY, int pin) {
	#ifdef DEBUG
		ansi.gotoXY(0, 22);
		Serial << "                                                 " << endl;
		ansi.gotoXY(0, 22);
		char etatText[BUFFER];
		snprintf(etatText, BUFFER, "Broche %d %s", pin, (digitalRead(pin) ? ON_STR : OFF_STR));
		Serial << "Changement d'état, " << etatText << endl;
	#endif

	afficherOnOff(POSITION_X_ETAT, positionY, digitalRead(pin));
	digitalWrite(pin, !digitalRead(pin));
} // changerValeurEtat()


// Afficher la touche du clavier dans l'interface
void afficherOption(char commande) {
	ansi.gotoXY(POSITION_X_OPTION, POSITION_Y_OPTION);
	Serial << F(BLEU) << commande << F(COULEUR_NORMAL);
} // afficherOption()


// Exécute la fonction pinMode() pour chaque broche
void initialiserBroches() {
	#ifdef DEBUG
		char broches[BUFFER];
		snprintf(broches, BUFFER, "(D%d, D%d, D%d)", PIN_LED_1, PIN_LED_2, PIN_RELAI);
		Serial << "Initialisation des broches : " << broches << " à OUPUT" << endl;
	#endif

	pinMode(PIN_LED_1, OUTPUT);
	pinMode(PIN_LED_2, OUTPUT);
	pinMode(PIN_RELAI, OUTPUT);
	pinMode(PIN_DETECTEUR_MOUVEMENT, INPUT);
	pinMode(PIN_BUZZER, OUTPUT);
	pinMode(PIN_BOUTON, INPUT);
	pinMode(PIN_POTENTIOMETRE, INPUT);
} // initialiserBroches()


// Initialisation des valeurs par défaut du LCD
void initialiserEcranLcd() {
	#ifdef DEBUG
		Serial << "Initialisation de l'écran LCD" << endl;
	#endif

	ecran.begin(16, 2);
	afficherLCDIntrusion(false);
} // initialiserEcranLcd()


// Afficher le temps => j jour(s) h:m:s
void afficherTempsEcoule(unsigned long elapsedTime) {
	// Calcul des valeurs
	int jour = elapsedTime / 86400;
	int heure = (elapsedTime % 86400) / 3600;
	int minute = (elapsedTime % 3600) / 60;
	int seconde = elapsedTime % 60;

	// Préparer les valeurs et le curseur
	ansi.gotoXY(POSITION_X_TEMPS_ECOULE, POSITION_Y_TEMPS_ECOULE);
	ecran.setCursor(3, 1);

	char textJour[BUFFER];
	char textTemps[BUFFER];

	snprintf(textJour, BUFFER, "%d jour%s", jour, (jour > 1 ? "s " : "  "));
	snprintf(textTemps, BUFFER, "%02d:%02d:%02d", heure, minute, seconde);
	
	// Afficher les valeurs de temps
	Serial << F(JAUNE) << textJour << textTemps << F(COULEUR_NORMAL);
	ecran.print(textTemps);
} // afficherTempsEcoule()


// Afficher la température et l'humidité actuelle dans l'interface
void afficherTemperatureHumidite() {
	ansi.gotoXY(POSITION_X_ETAT, POSITION_Y_TEMPERATURE);

	double temperature = bmp280.getTemperature();
	double convertionTemperature = etatChangementUniteTemperature==SYMBOLE_CELCIUS ? temperature : temperature * 1.8 + 32;

	Serial << F(VERT) << convertionTemperature << F(" ") << etatChangementUniteTemperature << F(COULEUR_NORMAL);

	ansi.gotoXY(POSITION_X_ETAT, POSITION_Y_HUMIDITE);
	Serial << F(VERT) << bmp280.getHumidity() << F(" %") << F(COULEUR_NORMAL);
} // afficherTemperatureHumidite()


void afficherEtatBinaire(unsigned int etatBinaire) {
	ansi.gotoXY(POSITION_X_ETAT_BINAIRE, POSITION_Y_ETAT_BINAIRE);
	Serial << ARRIERE_PLAN_ROUGE;
	afficherBinaire(etatBinaire);
	Serial << COULEUR_NORMAL;
}


// Achicher le texte ON OFF
void afficherOnOff(int posX, int posY, bool on) {
	ansi.gotoXY(posX, posY);
	Serial << F("   ");
	ansi.gotoXY(posX, posY);
	Serial << (on ? F(ON_STR) : F(OFF_STR));
} // afficherOnOff


// Retourne la date d'aujourd'hui sous forme de texte "Nous sommes le..."
void afficherDateActuelle() {
	horlotge.getTime();
	char buffer[70];

	snprintf(buffer, sizeof(buffer), " Nous sommes %s, le %d %s %d et il est %02d:%02d:%02d         ",
		SEMAINE[horlotge.dayOfWeek],
		horlotge.dayOfMonth, 
		MOIS[horlotge.month], 
		2000 + horlotge.year,
		horlotge.hour, 
		horlotge.minute, 
		horlotge.second
	);

	ansi.gotoXY(POSITION_X_DATE, POSITION_Y_DATE);
	Serial << F(ARRIERE_PLAN_INTERFACE) << buffer << F(COULEUR_NORMAL);
} // recupererDateActuel()


// Permet de convertir les valeurs booléennes en caractère
char recupererCaractereEtat(bool etat) {
	return etat ? '1' : '0';
}


// Afficher l'interface du projet
void afficherStationControle() {
	Serial << F("┌──────────────────────────────────────────────────────────────────┐\n");
	Serial << F("│") << ARRIERE_PLAN_INTERFACE << F("                   Station de contrôle UNO 3.0                    ") << COULEUR_NORMAL << F("│\n");
	Serial << F("├──────────────────────────────────────────────────────────────────┤\n");
	Serial << F("│ 1 - LED1  sur ") << BLEU << F("D2") << F(COULEUR_NORMAL) << F("                                     OFF         │\n");
	Serial << F("│ 2 - LED2  sur ") << BLEU << F("D3") << F(COULEUR_NORMAL) << F("                                     OFF         │\n");
	Serial << F("│ 3 - RELAI sur ") << BLEU << F("D4") << F(COULEUR_NORMAL) << F("                                     OFF         │\n");
	Serial << F("│ 4 - Celcius/Fahrenheit sur ") << BLEU << F("I2C") << F(COULEUR_NORMAL) << F("                       C           │\n");
	Serial << F("│ 5 - Détecteur de mouvement sur ") << BLEU << F("D5") << F(COULEUR_NORMAL) << F("                    Activé      │\n");
	Serial << F("│ 6 - LCD Rétro-éclairage                              ON          │\n");
	Serial << F("├──────────────────────────────────────────────────────────────────┤\n");
	Serial << F("│  Détecteur de mouvement : Alarme sur D6                          │\n");
	Serial << F("│  Température            :                                        │\n");
	Serial << F("│  Humidité               :                                        │\n");
	Serial << F("│  Durée d'alarme/panique :                                        │\n");
	Serial << F("├──────────────────────────────────────────────────────────────────┤\n");
	Serial << F("│ Temps écoulé:                                          Option:   │\n");
	Serial << F("├──────────────────────────────────────────────────────────────────┤\n");
	Serial << F("│                                                                  │\n");
	Serial << F("├──────────────────────────────────────────────────────────────────┤\n");
	Serial << F("│") << F(ARRIERE_PLAN_ROUGE) << F(" Compteur msg vers MEGA->               Apareils (bits):          ") << F(COULEUR_NORMAL) <<  F("│\n");
	Serial << F("├──────────────────────────────────────────────────────────────────┤\n");
	Serial << F("│ Projet réalisé par Ubert Guertin                     TP02--> UNO │\n");
	Serial << F("└──────────────────────────────────────────────────────────────────┘\n");
} // afficherStationControle()
