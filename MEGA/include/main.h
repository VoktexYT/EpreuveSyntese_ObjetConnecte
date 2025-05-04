/*
*	Projet : Épreuve synthèse Volet A
*	Fichier : main.h
*	Auteur : Ubert Guertin
*	Date : 2025/04/27
*	Description : La première partie de l'épreuve synthèse.
*/

#ifndef MAIN2_H
#define MAIN2_H

// Importer les librairies
#include <Arduino.h>
#include <SoftwareSerial.h>
#include <Arduino.h>
#include "ansi.h"
#include "Streaming.h"

// Information MEGA
#define UART_VITESSE        19200
#define UART_RX             10 
#define UART_TX             11
#define MAX_CAR_NOM_SYSTEME 10

// Couleurs
#define ARRIERE_PLAN_JAUNE "\033[43m"
#define ARRIERE_PLAN_BLEU "\033[44m"
#define COULEUR_NORMAL "\033[0m"
#define MAUVE "\033[35m"
#define JAUNE "\033[93m"

// Les icons
#define _ON                           "    🟢"
#define _OFF                          "    🔴"
#define _OFF_VIDE                     "      "
#define FAHRENHEIT                    "Fahrenheit"
#define CELCIUS                       "  Celcius "
#define SOS                           "    🆘"
#define NON_SOS                       "    🆗"

// Position de chaque élement dans la station de contrôle
#define POSITION_X_ETAT_ELEMENT 56

#define POSITION_Y_ETAT_LED1 4
#define POSITION_Y_ETAT_LED2 5
#define POSITION_Y_ETAT_RELAI 6
#define POSITION_Y_TEMPERATURE_SYMBOLE 7
#define POSITION_Y_DETECTEUR_MOUVEMENT_ACTIVE 8
#define POSITION_Y_ETAT_RETRO_ECLAIRAGE 9
#define POSITION_Y_ETAT_BOUTON_PANIQUE 10
#define POSITION_Y_ETAT_MOUVEMENT_DETECTE 11
#define POSITION_Y_TEMPERATURE 12
#define POSITION_Y_HUMIDITE 13
#define POSITION_Y_DUREE_ALARME 14
#define POSITION_Y_HEURE_UNO 15
#define POSITION_Y_ETAT_SYSTEME 16

#define POSITION_X_TEMPS_ECOULE 28
#define POSITION_Y_TEMPS_ECOULE 18
#define BUFFER_TEMPS_ECOULE 20

#define POSITION_Y_DONNEE_VALIDE_RECU 22
#define POSITION_X_DONNEE_VALIDE_RECU 40

#define POSITION_X_PRESSION 47

// Temps
#define SECONDE 1000


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
void afficherTexteAvecRemplissage(const char* texte, int longueur, int posX, int posY, char caractere = '-');
int recupererTailleTexte(const char* texte);
void afficherStationControleMega();
void afficherTempsEcoule(unsigned long elapsedTime);
void afficherDonneeValideRecu(int donneesEnvoyees);
void afficherInformationSysteme(Systeme systeme);


#endif