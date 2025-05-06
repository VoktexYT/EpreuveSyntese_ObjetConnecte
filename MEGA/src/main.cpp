/*
*	Projet : Épreuve synthèse Volet A
*	Fichier : main.cpp
*	Auteur : Ubert Guertin
*	Date : 2025/04/27
*	Description : La première partie de l'épreuve synthèse.
*/

#include "main.h"
#define recepteur Serial2


// Instances globales
Systeme e2467525;
ANSI ansi(&Serial);

// Fonction de démarage
void setup() {
  Serial.begin(UART_VITESSE);
  Serial.println("Démarrage du récepteur...");
  recepteur.begin(UART_VITESSE);

  ansi.clearScreen();

  afficherStationControleMega();

  
} // setup()

// Boucle du programme
void loop()
{
	static unsigned long int debutTemps = millis();
  static unsigned long int donneesValidesEnvoyees = 0;

  delay(100);

  // Validation coté MEGA
  // Vérifier si des données sont disponibles sur le port du transmetteur
  if (recepteur.available() == sizeof(e2467525)) {  // NOTE: unSysteme est une variable de type 'Systeme'
  
    // Si le bon nombre d'octets reçues, lire les données
    recepteur.readBytes((char*)&e2467525, sizeof(e2467525)); // (char*) indique de traiter 'unSysteme' comme une suite d'octets.
  
    // Afficher/traiter les données reçues
    afficherInformationSysteme(e2467525);
    donneesValidesEnvoyees++;
  }
  
  else {
    // Sinon, effacer le buffer de réception    
    while (recepteur.available()) {
      recepteur.read(); // Lire et ignorer les données du buffer
    }
  }

	afficherTempsEcoule((millis() - debutTemps) / SECONDE);
  afficherDonneeValideRecu(donneesValidesEnvoyees);
} // loop()

// Afficher le temps => j jour(s) h:m:s
void afficherTempsEcoule(unsigned long elapsedTime) {
	// Calcul des valeurs
	int jour = elapsedTime / 86400;
	int heure = (elapsedTime % 86400) / 3600;
	int minute = (elapsedTime % 3600) / 60;
	int seconde = elapsedTime % 60;

	// Préparer les valeurs et le curseur
	ansi.gotoXY(POSITION_X_TEMPS_ECOULE, POSITION_Y_TEMPS_ECOULE);

	char textJour[BUFFER_TEMPS_ECOULE];
	char textTemps[BUFFER_TEMPS_ECOULE];

	snprintf(textJour, BUFFER_TEMPS_ECOULE, "%d jour%s", jour, (jour > 1 ? "s " : "  "));
	snprintf(textTemps, BUFFER_TEMPS_ECOULE, "%02d:%02d:%02d", heure, minute, seconde);
	
	// Afficher les valeurs de temps
	Serial << F(JAUNE) << textJour << textTemps << F(COULEUR_NORMAL);
} // afficherTempsEcoule()


// Affiche le nombre de données que le MEGA a reçu
void afficherDonneeValideRecu(int donneesEnvoyees) {
  ansi.gotoXY(POSITION_X_DONNEE_VALIDE_RECU, POSITION_Y_DONNEE_VALIDE_RECU);
  Serial << ARRIERE_PLAN_BLEU << "          " << COULEUR_NORMAL;
  ansi.gotoXY(POSITION_X_DONNEE_VALIDE_RECU, POSITION_Y_DONNEE_VALIDE_RECU);
  Serial << ARRIERE_PLAN_BLEU << donneesEnvoyees << COULEUR_NORMAL;
} // afficherDonneeValideRecu()

// Permet d'aligner un texte avec des symbole. Ex: exemple ----->
void afficherTexteAvecRemplissage(const char* texte, int longueur, int posX, int posY, char caractere) {
    ansi.gotoXY(posX, posY);

    Serial << texte << " ";

    for (int i=0; i<longueur - recupererTailleTexte(texte); i++)
    {
        Serial << caractere;
    }

    Serial << ">";
} // afficherTexteAvecRemplissage()

// Fonction qui permet de récupérer la grandeur d'une chaine de caractères (Y compris les accents)
int recupererTailleTexte(const char* texte) {
    int compte = 0;
    int i = 0;
    
    while (texte[i] != '\0')
    {
        unsigned char c = texte[i];
        
        // Si l'octet n'est pas un octet de continuation UTF-8 (10xxxxxx)
        if ((c & 0b11000000) != 0b10000000)
        {
            compte++;
        }
        
        i++;
    }
    
    return compte;
} // recupererTailleTexte()

// // Afficher l'interface du projet
void afficherStationControleMega()
{
	Serial << F("┌──────────────────────────────────────────────────────────────────┐\n");
	Serial << F("│") << ARRIERE_PLAN_JAUNE << F("                   Station de contrôle MEGA 3.0                   ") << COULEUR_NORMAL << F("│\n");
	Serial << F("├──────────────────────────────────────────────────────────────────┤\n");
	Serial << F("│                                                                  │\n");
  Serial << F("│                                                                  │\n");
  Serial << F("│                                                                  │\n");
  Serial << F("│                                                                  │\n");
  Serial << F("│                                                                  │\n");
  Serial << F("│                                                                  │\n");
  Serial << F("│                                                                  │\n");
  Serial << F("│                                                                  │\n");
  Serial << F("│                                                                  │\n");
  Serial << F("│                                                                  │\n");
  Serial << F("│                                                                  │\n");
  Serial << F("│                                                                  │\n");
  Serial << F("│                                                                  │\n");
	Serial << F("├──────────────────────────────────────────────────────────────────┤\n");
  Serial << F("│ Système démarré depuis :                                         │\n");
	Serial << F("├──────────────────────────────────────────────────────────────────┤\n");
  Serial << F("│ Projet réalisé par     : Ubert Guertin             TP02 --> MEGA │\n");
	Serial << F("├──────────────────────────────────────────────────────────────────┤\n");
  Serial << F("│") << ARRIERE_PLAN_BLEU << F(" Données valide reçus du UNO        :  3075                       ") << COULEUR_NORMAL << F("│\n");
	Serial << F("├──────────────────────────────────────────────────────────────────┤\n");
  Serial << F("│") << ARRIERE_PLAN_JAUNE<< F(" Requêtes soumises vers l'API Web   :  <PAS ENCORE FAIT>          ") << COULEUR_NORMAL << F("│\n");
	Serial << F("└──────────────────────────────────────────────────────────────────┘\n");

  afficherTexteAvecRemplissage("LED1", 50, 3, 4);
  afficherTexteAvecRemplissage("LED2", 50, 3, 5);
  afficherTexteAvecRemplissage("RELAI", 50, 3, 6);
  afficherTexteAvecRemplissage("Unité de température", 50, 3, 7);
  afficherTexteAvecRemplissage("Détecteur de mouvement activé", 50, 3, 8);
  afficherTexteAvecRemplissage("Rétro-éclairage du LCD", 50, 3, 9);
  afficherTexteAvecRemplissage("Bouton panique", 50, 3, 10);
  afficherTexteAvecRemplissage("Mouvement détecté", 50, 3, 11);
  afficherTexteAvecRemplissage("Température", 50, 3, 12);
  afficherTexteAvecRemplissage("Humidité", 50, 3, 13);
  afficherTexteAvecRemplissage("Durée d'alarme/panique", 50, 3, 14);
  afficherTexteAvecRemplissage("Heure du UNO", 50, 3, 15);
  afficherTexteAvecRemplissage("État du système", 50, 3, 16);

} // afficherStationControle()

void afficherInformationSysteme(Systeme systeme) {
  const char * graphDigits[] = {"", "1️⃣", "2️⃣", "3️⃣", "4️⃣", "5️⃣", "6️⃣","7️⃣", "8️⃣","9️⃣", "🔟"};
  unsigned int etatsSysteme = systeme.appareils.etatAppareils;

  ansi.gotoXY(POSITION_X_ETAT_ELEMENT, POSITION_Y_ETAT_LED1);
  Serial.print(etatsSysteme & 1 ? _OFF : _ON);

  ansi.gotoXY(POSITION_X_ETAT_ELEMENT, POSITION_Y_ETAT_LED2);
  Serial.print(etatsSysteme & (1 << 1) ? _OFF : _ON);

  ansi.gotoXY(POSITION_X_ETAT_ELEMENT, POSITION_Y_ETAT_RELAI);
  Serial.print(etatsSysteme & (1 << 2) ? _OFF : _ON);

  ansi.gotoXY(POSITION_X_ETAT_ELEMENT, POSITION_Y_TEMPERATURE_SYMBOLE);
  Serial.print(etatsSysteme & (1 << 7) ? CELCIUS : FAHRENHEIT);

  ansi.gotoXY(POSITION_X_ETAT_ELEMENT, POSITION_Y_DETECTEUR_MOUVEMENT_ACTIVE);
  Serial.print(etatsSysteme & (1 << 4) ? _ON : _OFF);

  ansi.gotoXY(POSITION_X_ETAT_ELEMENT, POSITION_Y_ETAT_RETRO_ECLAIRAGE);
  Serial.print(etatsSysteme & (1 << 6) ? _ON : _OFF);

  ansi.gotoXY(POSITION_X_ETAT_ELEMENT, POSITION_Y_ETAT_BOUTON_PANIQUE);
  Serial.print(etatsSysteme & (1 << 5) ? _ON : _OFF_VIDE);

  ansi.gotoXY(POSITION_X_ETAT_ELEMENT, POSITION_Y_ETAT_MOUVEMENT_DETECTE);
  Serial.print(etatsSysteme & (1 << 3) ? _ON : _OFF_VIDE);

  ansi.gotoXY(POSITION_X_ETAT_ELEMENT, POSITION_Y_TEMPERATURE);
  Serial.print("   ");
  Serial.print(systeme.appareils.temperature);

  ansi.gotoXY(POSITION_X_ETAT_ELEMENT, POSITION_Y_HUMIDITE);
  Serial.print("   ");
  Serial.print(systeme.appareils.humidite);

  ansi.gotoXY(POSITION_X_ETAT_ELEMENT, POSITION_Y_DUREE_ALARME);
  Serial.print("    ");
  Serial.print(graphDigits[systeme.appareils.dureeAlarme]);

  ansi.gotoXY(POSITION_X_ETAT_ELEMENT, POSITION_Y_HEURE_UNO);
  char buffer[9];
  snprintf(buffer, 9, "%02d:%02d:%02d", systeme.heure, systeme.minute, systeme.seconde);
  Serial.print("  ");
  Serial.print(buffer);

  ansi.gotoXY(POSITION_X_ETAT_ELEMENT, POSITION_Y_ETAT_SYSTEME);
  Serial.print(systeme.codeMessage == Evenements::evenement_statut_appareils ? NON_SOS : SOS);

  ansi.gotoXY(POSITION_X_PRESSION, POSITION_Y_TEMPS_ECOULE);
  Serial << "P: " << MAUVE << systeme.appareils.pressionAtomospherique/1000 << "kPa " << COULEUR_NORMAL << "A: " << MAUVE << systeme.appareils.altitude << "m" << COULEUR_NORMAL;
} // afficherInformationSysteme()