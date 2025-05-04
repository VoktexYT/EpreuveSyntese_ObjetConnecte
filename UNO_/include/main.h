/*
*	Projet : Épreuve synthèse Volet A
*	Fichier : main.h
*	Auteur : Ubert Guertin
*	Date : 2025/04/27
*	Description : La première partie de l'épreuve synthèse.
*/

#ifndef main_h
#define main_h

// Option MACRO
// #define DEBUG

// Librairies
#include <Arduino.h>
#include "ansi.h"
#include <rgb_lcd.h>
#include <Streaming.h>
#include <SHT31.h>
#include <DS1307.h>
#include <SoftwareSerial.h>
#include "Seeed_BME280.h"
#include <Wire.h>

// Nom système
#define NOM_SYSTEME "e2467525"
#define MAX_CAR_NOM_SYSTEME 10
#define lienAvecRecepteur Serial2

// Port communication UNO
#define UART_RX             6
#define UART_TX             7

// Structure & Enum
enum Evenements 
{
	evenement_statut_appareils,         // 0
	evenement_temperature_depassee,     // 1
	evenement_detection_mouvement,      // 2
	evenement_bouton_panique,           // 3
	evenement_systeme_enligne,          // 4
	evenement_changement_etat,          // 5
	evenement_UNO_non_connecte,         // 6
	evenement_nb_codes                  // 7  
};

enum EtatAppareils 
{
	APP_LED1                 =     0b1 << 0, // 0000 0001
	APP_LED2                 =     0b1 << 1, // 0000 0010
	APP_RELAI                =     0b1 << 2, // 0000 0100
	APP_MOUVEMENT            =     0b1 << 3, // 0000 1000
	APP_MOUVEMENT_ARME       =     0b1 << 4, // 0001 0000
	APP_BOUTON_PANIQUE       =     0b1 << 5, // 0010 0000
	APP_RETRO_LCD            =     0b1 << 6, // 0100 0000
	APP_UNITE_TEMP           =     0b1 << 7  // 1000 0000
};


struct Appareils
{
	unsigned int  etatAppareils;  // Traitement sur les bits.  // 2 octets
	float         temperature;                                 // 4 octets
	float         humidite;                                    // 4 octets
	int           dureeAlarme;                                 // 2 octets 
	uint32_t	  pressionAtomospherique;                      // 4 octets
	float		  altitude;									   // 4 octets
};

struct Systeme 
{
	char        nomSysteme[MAX_CAR_NOM_SYSTEME];  // 10 octets
	Evenements  codeMessage;                      //  2 octets
	byte        heure;                            //  1 octet
	byte        minute;                           //  1 octet
	byte        seconde;                          //  1 octet
	Appareils   appareils;                        // 12 octets
};  


// Signature des fonctions
void initialiserEcranLcd();
void initialiserBroches();
void changerValeurSysteme(unsigned int& etats, int valeurPotentiometre);

void afficherBinaire(unsigned int valeur);
void afficherEtatBinaire(unsigned int etatBinaire);

void afficherStationControle();
void afficherOnOff(int posX, int posY, bool on);
void afficherTemperatureHumidite();
void afficherTempsEcoule(unsigned long elapsedTime);
void afficherOption(char commande);
void afficherLCDIntrusion(bool estIntrusion);
void afficherDateActuelle();
void afficherValeurPotentiometre(int valeur);
void afficherEtatAlarme(bool etatAlarme);

void changerValeurEtatBinaire(unsigned int &etats, bool etatLed1, bool etatLed2, bool etatRelai, bool etatChangementUniteTemperature, bool etatDetecteurMouvement, bool etatRetroEclairage, bool etatBoutonPanique, bool mouvementDetecte);
void changerValeurEtat(int positionY, int pin = -1);
void changerTexteEtatAlarme(bool etatAlarme);
void changerValeurTemperature(char& etatChangementUniteTemperature);
void changerEtatBuzzer(int nouvelEtat);
void changerCouleurLCD(bool etatDetecteurMouvement, bool retroEclairage);

void verifierEtatBouton(bool& etatDetecteurMouvement);
void verifierEtatDetecteurMouvement(bool etatAlarme, bool& etatDetecteurMouvement, bool etatLCDRetroEclarage, int longueurTempsAlarme, unsigned long int& tempsDebut, bool estValeurPotentiometreChangee);


void executerCommande(
    char commande, 
    bool& etatLed1, bool& etatLed2, bool& etatRelai, 
    char& etatChangementUniteTemperature, bool& etatDetecteurMouvement, bool& etatLCDRetroEclarage);

char recupererCaractereEtat(bool etat);


// Vitesse de communication UART
#define VITESSE_UART 19200

// Valeur 1 seconde
#define SECONDE 1000

// Valeurs constantes en lien avec une date 
const char* MOIS[] = {"", 
	"Janvier", "Fevrier", "Mars", "Avril", 
	"Mai", "Juin", "Juillet", "Aout",
	"Septembre", "Octobre", "Novembre", "Decembre"
};

const char* SEMAINE[] = {"", 
	"Lundi", "Mardi", "Mercredi",
	"Jeudi", "Vendredi", "Samedi", 
	"Dimanche"
};

// Couleurs
#define ROUGE "\033[91m"
#define VERT "\033[92m"
#define JAUNE "\033[93m"
#define BLEU "\033[94m"
#define COULEUR_NORMAL "\033[0m"

#define ARRIERE_PLAN_INTERFACE "\033[44;30m"
#define ARRIERE_PLAN_ROUGE "\033[41m"

// Potentiomètre
#define POTENTIOMETRE_MIN 0
#define POTENTIOMETRE_MAX 1023
#define POTENTIOMETRE_MAP_MIN 1
#define POTENTIOMETRE_MAP_MAX 10


// Valeur ON OFF
#define ON_STR VERT "ON" COULEUR_NORMAL
#define OFF_STR ROUGE "OFF" COULEUR_NORMAL

// Broches des composants
#define PIN_LED_1 2
#define PIN_LED_2 3
#define PIN_RELAI 4
#define PIN_DETECTEUR_MOUVEMENT 5
#define PIN_BUZZER 6
#define PIN_BOUTON 7
#define PIN_UART_D8 8
#define PIN_UART_D9 9
#define PIN_POTENTIOMETRE A0

// Touches du clavier à détecter
#define COMMANDE_LED_1 '1'
#define COMMANDE_LED_2 '2'
#define COMMANDE_RELAI '3'
#define COMMANDE_CHANGEMENT_UNITE_TEMPERATURE '4'
#define COMMANDE_DETECTEUR_MOUVEMENT '5'
#define COMMANDE_RETRO_ECLAIRAGE '6'

// Valeur des paramètres
#define EXCLURE_AFFICHAGE -1
#define EXCLURE_DIGITAL_WRITE -2

// État par défaut des composants
#define ON_OFF_DEFAUT_LED_1 false
#define ON_OFF_DEFAUT_LED_2 false
#define ON_OFF_DEFAUT_RELAI false
#define ON_OFF_DEFAUT_RETRO_ECLAIRAGE false
#define ON_OFF_DEFAUT_ALARME true
#define ON_OFF_DEFAUT_DETECTEUR_MOUVEMENT false
#define ON_OFF_DEFAUT_RETRO_ECLAIRAGE true
#define F_C_DEFAUT_TEMPERATURE 'C'

#define SYMBOLE_CELCIUS 'C'
#define SYMBOLE_FAHRENHEIT 'F'
#define TEXT_ACTIVE "Activé"
#define TEXT_DESACTIVE "Désactivé"

// Position curseur dans menu
#define POSITION_Y_LED1 4
#define POSITION_Y_LED2 5
#define POSITION_Y_RELAI 6
#define POSITION_Y_UNITE_TEMPERATURE 7
#define POSITION_Y_DETECTEUR_MOUVEMENT 8
#define POSITION_Y_RETRO_ECLARAGE 9

#define POSITION_Y_ETAT_ALARME 11
#define POSITION_Y_TEMPERATURE 12
#define POSITION_Y_HUMIDITE 13
#define POSITION_Y_VALEUR_POTENTIOMETRE 14


#define POSITION_Y_OPTION 16
#define POSITION_Y_ETAT_BINAIRE 20
#define POSITION_Y_TEMPS_ECOULE 16

#define POSITION_X_ETAT 56
#define POSITION_X_OPTION 66
#define POSITION_X_ETAT_BINAIRE 59
#define POSITION_X_TEMPS_ECOULE 19

#define POSITION_X_DATE 2
#define POSITION_Y_DATE 18


// Buffer des chaines textJour & textTemps
#define BUFFER 20

#endif