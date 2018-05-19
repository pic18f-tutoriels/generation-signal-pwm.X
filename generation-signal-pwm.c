/* 
 * Démontrer le concept de signal PWM avec un exemple de modulation 
 * d'un signal sinusoïdal. Le rapport cyclique du signal PWM contient la 
 * valeur instantanée du signal modulé qui est, en l'occurrence, 
 * Deux versions sont proposées:
 * - VERSION1: produit le PWM programmatiquement.
 * - VERSION2: produit le PWM avec le générateur du micro-contrôleur.
 * @author jean-michel-gonet
 */
#include <xc.h>

/**
 * Bits de configuration:
 */
#pragma config FOSC = INTIO67  // Osc. interne, A6 et A7 comme IO.
#pragma config IESO = OFF      // Pas d'osc. au d�marrage.
#pragma config FCMEN = OFF     // Pas de monitorage de l'oscillateur.

// Nécessaires pour ICSP / ICD:
#pragma config MCLRE = EXTMCLR // RE3 est actif comme master reset.
#pragma config WDTEN = OFF     // Watchdog inactif.
#pragma config LVP = OFF       // Single Supply Enable bits off.


char sinus[] =
{
    16, 21, 25, 28, 30, 31, 30,
    28, 25, 21, 16, 11,  7,  4,
     2,  1,  2,  4,  7, 11, 16
};

//#define VERSION1
#define VERSION2

#ifdef VERSION2

/**
 * Gère les interruptions de haute priorité.
 */
void interrupt interruptionsHP()
{
    static char n = 0;

    // Met à jour le rapport cyclique:
    CCPR5L = sinus[n];

    // Suivante étape dans la séquence
    // du rapport cyclique.
    n++;
    if (n > 20)
    {
        n = 0;
    }
}

/**
 * Point d'entrée.
 * Configure le port A comme entrée, et le B comme sortie.
 * Puis copie la valeur lue du port A dans le port B.
 */
void main()
{
    ANSELA = 0x00;      // Désactive les convertisseurs A/D.
    ANSELB = 0x00;      // Désactive les convertisseurs A/D.
    ANSELC = 0x00;      // Désactive les convertisseurs A/D.

    TRISA = 0x00;       // Tous les bits du port A comme sorties.

    T2CONbits.T2CKPS = 0;   // Pas de diviseur de fréquence.
    T2CONbits.TMR2ON = 1;   // Allume le temporisateur 2.
    PR2 = 32;               // Période de 32 pour le temporisateur 2.

    CCP5CONbits.CCP5M = 0x0F;   // Allume le générateur PWM.
    CCPTMRS1bits.C5TSEL = 0;    // Le PWM CCP5 utilise le temp. 2.
    CCPR5L = 16;                // Rapport cyclique à 50% pour commencer.
    TRISAbits.TRISA4 = 0;       // Port RA4 en sortie.

    RCONbits.IPEN = 1;          // Active les interruptions.
    INTCONbits.GIEH = 1;        // Active les int. de haute priorité.
    PIE1bits.TMR2IE = 1;        // Active les int. pour temp. 2
    IPR1bits.TMR2IP = 1;        // Int. du temp2 sont de haute prio.

    // Dodo
    while(1);
}
#endif

#ifdef VERSION1
/**
 * Point d'entrée.
 * Configure le port A comme entrée, et le B comme sortie.
 * Puis copie la valeur lue du port A dans le port B.
 */
void main()
{
    char tmr = 0;
    char pr = 32;
    char ccpr = 0;

    char n;

    ANSELA = 0x00;      // Désactive les convertisseurs A/D.
    ANSELB = 0x00;      // Désactive les convertisseurs A/D.
    ANSELC = 0x00;      // Désactive les convertisseurs A/D.

    TRISA = 0x00;       // Tous les bits du port A comme sorties.

    while(1)
    {
        for(n=0; n<20; n++)
        {
            ccpr = sinus[n];
            for(tmr=0; tmr<pr; tmr++)
            {
                if (tmr<ccpr)
                {
                    PORTA = 0;
                }
                else
                {
                    PORTA = 255;
                }
            }
        }
    }
}
#endif