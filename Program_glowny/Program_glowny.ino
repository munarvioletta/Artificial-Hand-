/**
 * @file
 * @author  Violetta Munar Ernandes
 * @version 1.0
 */

#define DEBUG_MODE false

#include <Arduino.h>
#include "Wyswietlanie.cpp"

#include "Reka.cpp"
#include "DualMC33926MotorShield.h"

// alokowanie pamieci w zmiennej typu string dla odbieranych danych
//String inputString = "";         
//boolean stringComplete = false;  // definicja zmiennej stringComplete 

CzujnikNacisku* czuj_nac[5]; 	  //!< tablica wskaźników na obiekty typu Czujnik reprezentujących czujniki nacisku.  
Enkoder* enk[5][2];  			      //!< tablica wskaźników na obiekty typu Czujnik reprezentujących enkodery.  
CzujnikPradu* czuj_prad;  		  //!< tablica wskaźników na obiekty typu Czujnik reprezentujących czujnik prądu.  

Silnik* sil[5][2];  			      //!< tablica wskaźników na obiekty typu Silnik reprezentujących silniki DC.  

SilnikRegulowany* napedy[5][2];            //!< tablica wskaźników na obiekty typu Naped reprezentujących napędy regulowane.  
Serwo* serwo_kciuk;   			               //!< wskaźnik do obiektu typu Naped reprezentujących serwomechanizm.  

DualMC33926MotorShield* sterowniki[5]; 		 //!< deklaracja tablicy wskaźników na obiekty typu DualMC33926MotorShield

Palec* pal[5];   							             //!< tablica wskaźników na obiekty typu Palec reprezentujących palce ręki.  

Reka* reka;     						               //!< wskaźnik do obiektu typu Reka reprezentujących cały mechanizm zawierający wszystkie elementy.  

float czas_prob = 1.0/10.0;   			         //!< zmienna określająca czas próbkowania w sekundach

float natezenie_zatrzymania = 3.0; 		     //!< natezenie pradu przy ktorym reka przestanie sie zaciskac

int ustaw_wyprost = 46;
int ustaw_ruch_sekwencyjny = 47;
int ustaw_chwyt_cylindryczny = 48;
int ustaw_chwyt_szczypcowy = 49;
int ustaw_chwyt_sferyczny = 50;
int ustaw_chwyt_hakowy = 51;
int ustaw_chwyt_dloniowy = 52;
//int ustaw_chwyt_lateralny = A9;

String inputString = "";         // a string to hold incoming data
boolean stringComplete = false;  // whether the string is complete


/**
* Funkcja służąca do konfiguracji używanych zmiennych i obiektów.
* Kompletowany jest tutaj obiekt klasy Reka, składający się z wielu elementów składowych.
*/

void setup() {

   for (int i = 2; i<=45; i++)
{
   pinMode(i, OUTPUT);
   digitalWrite(i, LOW); //turn pullup resistor on
} //!< Funkcja służąca do konfiguracji używanych zmiennych i obiektów. Kompletowany jest tutaj obiekt klasy Reka, składający się z wielu elementów składowych.
 
//  
//  for (int i = 46; i<=52; i++)
//{
//   pinMode(i, INPUT);
//   digitalWrite(i, LOW); //turn pullup resistor on
//} //!< Funkcja służąca do konfiguracji używanych zmiennych i obiektów. Kompletowany jest tutaj obiekt klasy Reka, składający się z wielu elementów składowych.  
////   digitalWrite(ustaw_chwyt_lateralny, LOW);
  //fragment kodu  odpowiedzialny za wyswietlanie komunikatow na monitorze portu szeregowego
  
  Serial.begin(115200);

  Serial.println(" ");
  Serial.println("Wybierz numer sekwencji ruchu od 1 do 8");
  Serial.println(" 1 - wyprostowanie palcow");
  Serial.println(" 2 - ruch Sekwencyjny");
  Serial.println(" 3 - chwyt Cylindryczny");
  Serial.println(" 4 - chwyt szczypcowy");
  Serial.println(" 5 - chwyt sferyczny");
  Serial.println(" 6 - chwyt hakowy");
  Serial.println(" ");
//  Serial.println(" 7 - chwyt dloniowy");
//  Serial.println(" 8 - chwyt lateralny");

  // tworzenie obiektow i przypisywanie do nich pinow
  czuj_nac[0] = new CzujnikNacisku(A0);
  czuj_nac[1] = new CzujnikNacisku(A1);
  czuj_nac[2] = new CzujnikNacisku(A2);
  czuj_nac[3] = new CzujnikNacisku(A3);
  czuj_nac[4] = new CzujnikNacisku(A4);

  czuj_prad = new CzujnikPradu(A6);

  enk[0][0] = new Enkoder(21, 22, 2);
  attachInterrupt(digitalPinToInterrupt(21), updateEnkoder00, CHANGE);
  delay(100);
  attachInterrupt(digitalPinToInterrupt(22), updateEnkoder00, CHANGE);
  delay(100);

  enk[0][1] = new Enkoder(23, 24, 2);
  attachInterrupt(digitalPinToInterrupt(23), updateEnkoder01, CHANGE);
  delay(100);
  attachInterrupt(digitalPinToInterrupt(24), updateEnkoder01, CHANGE);
  delay(100);

  enk[1][0] = new Enkoder(26, 27, 2);
  attachInterrupt(digitalPinToInterrupt(26), updateEnkoder10, CHANGE);
  delay(100);
  attachInterrupt(digitalPinToInterrupt(27), updateEnkoder10, CHANGE);
  delay(100);

  enk[1][1] = new Enkoder(44, 45, 2);
  attachInterrupt(digitalPinToInterrupt(44), updateEnkoder11, CHANGE);
  delay(100);
  attachInterrupt(digitalPinToInterrupt(45), updateEnkoder11, CHANGE);
  delay(100);

  enk[2][0] = new Enkoder(28, 29, 2);
  attachInterrupt(digitalPinToInterrupt(28), updateEnkoder20, CHANGE);
  delay(100);
  attachInterrupt(digitalPinToInterrupt(29), updateEnkoder20, CHANGE);
  delay(100);

  enk[2][1] = new Enkoder(30, 31, 2); // (pin, pin, tryb zliczania impulsow -> maly enkoder - 4, duzy enkoder - 2)
  attachInterrupt(digitalPinToInterrupt(30), updateEnkoder21, CHANGE);
  delay(100);
  attachInterrupt(digitalPinToInterrupt(31), updateEnkoder21, CHANGE);
  delay(100);

  enk[3][0] = new Enkoder(32, 33, 2);
  attachInterrupt(digitalPinToInterrupt(32), updateEnkoder30, CHANGE);
  delay(100);
  attachInterrupt(digitalPinToInterrupt(33), updateEnkoder30, CHANGE);
  delay(100);

  enk[3][1] = new Enkoder(34, 35, 2);
  attachInterrupt(digitalPinToInterrupt(34), updateEnkoder31, CHANGE);
  delay(100);
  attachInterrupt(digitalPinToInterrupt(35), updateEnkoder31, CHANGE);
  delay(100);

  enk[4][0] = new Enkoder(36, 37, 2);
  attachInterrupt(digitalPinToInterrupt(36), updateEnkoder40, CHANGE);
  delay(100);
  attachInterrupt(digitalPinToInterrupt(37), updateEnkoder40, CHANGE);
  delay(100);

  enk[4][1] = new Enkoder(38, 39, 2);
  attachInterrupt(digitalPinToInterrupt(38), updateEnkoder41, CHANGE);
  delay(100);
  attachInterrupt(digitalPinToInterrupt(39), updateEnkoder41, CHANGE);
  delay(100);

  sterowniki[0] = new DualMC33926MotorShield(40, 12, 3, 13, 43);  // ( M1DIR - faza A, M1PWM - AENBL, M2DIR- faza B, M2PWM - BENBL, nD2 - MODE)
  sterowniki[1] = new DualMC33926MotorShield(14, 10, 25, 11, 50); // ( M1DIR - faza A, M1PWM - AENBL, M2DIR- faza B, M2PWM - BENBL, nD2 - MODE)
  sterowniki[2] = new DualMC33926MotorShield(16, 8, 15, 9, 50);   // ( M1DIR - faza A, M1PWM - AENBL, M2DIR- faza B, M2PWM - BENBL, nD2 - MODE)
  sterowniki[3] = new DualMC33926MotorShield(18, 6, 17, 7, 50);   // ( M1DIR - faza A, M1PWM - AENBL, M2DIR- faza B, M2PWM - BENBL, nD2 - MODE)
  sterowniki[4] = new DualMC33926MotorShield(20, 4, 19, 5, 50);   // ( M1DIR - faza A, M1PWM - AENBL, M2DIR- faza B, M2PWM - BENBL, nD2 - MODE)

  //inicjacja sterownika silnika DC
  sterowniki[0]->init();
  sterowniki[1]->init();
  sterowniki[2]->init();
  sterowniki[3]->init();
  sterowniki[4]->init();

 // tworzenie obiektów o typie Silnik
  sil[0][0] = new Silnik(sterowniki[0], 0, 10,2); // (sterownik, id, mnoznik_przod, mnoznik_tyl)
  sil[0][1] = new Silnik(sterowniki[0], 1, 8, 4);
  sil[1][0] = new Silnik(sterowniki[1], 0, 8, 4);
  sil[1][1] = new Silnik(sterowniki[1], 1, 8, 4);
  sil[2][0] = new Silnik(sterowniki[2], 0, 8, 4);
  sil[2][1] = new Silnik(sterowniki[2], 1, 8, 4);
  sil[3][0] = new Silnik(sterowniki[3], 0, 8, 3);
  sil[3][1] = new Silnik(sterowniki[3], 1, 8, 3);
  sil[4][0] = new Silnik(sterowniki[4], 0, 8, 4);
  sil[4][1] = new Silnik(sterowniki[4], 1, 10,4);

  napedy[0][0] = new SilnikRegulowany(new RegulatorProporcjonalny(2), enk[0][1], sil[0][0]);
  napedy[0][1] = new SilnikRegulowany(new RegulatorProporcjonalny(2), enk[0][1], sil[0][1]);
  napedy[1][0] = new SilnikRegulowany(new RegulatorProporcjonalny(2), enk[1][1], sil[1][0]);
  napedy[1][1] = new SilnikRegulowany(new RegulatorProporcjonalny(2), enk[1][1], sil[1][1]);
  napedy[2][0] = new SilnikRegulowany(new RegulatorProporcjonalny(2), enk[2][1], sil[2][0]);
  napedy[2][1] = new SilnikRegulowany(new RegulatorProporcjonalny(2), enk[2][1], sil[2][1]);
  napedy[3][0] = new SilnikRegulowany(new RegulatorProporcjonalny(2), enk[3][0], sil[3][0]);
  napedy[3][1] = new SilnikRegulowany(new RegulatorProporcjonalny(2), enk[3][0], sil[3][1]);
  napedy[4][0] = new SilnikRegulowany(new RegulatorProporcjonalny(2), enk[4][0], sil[4][1]);
  napedy[4][1] = new SilnikRegulowany(new RegulatorProporcjonalny(2), enk[4][1], sil[4][1]);

  serwo_kciuk = new Serwo(2);

  pal[0] = new PalecKciuk(0, napedy[0], serwo_kciuk, czuj_nac[0]);
  pal[1] = new PalecNorm(1, napedy[1], czuj_nac[1]);
  pal[2] = new PalecNorm(2, napedy[2], czuj_nac[2]);
  pal[3] = new PalecNorm(3, napedy[3], czuj_nac[3]);
  pal[4] = new PalecNorm(4, napedy[4], czuj_nac[4]);

  reka = new Reka(pal, czuj_prad, natezenie_zatrzymania, czas_prob);

serwo_kciuk-> ustawWartosc(20);

for (int i = 0; i<5; i++)
{
  for (int j=0; j<2; j++)
  {
    sil[i][j]->ustawPredkosc(0);
  }
}
  
 // reserve 200 bytes for the inputString:
  inputString.reserve(200);
  
}


/**
* Pętla główna programu. Znajduje się tu cała logika działania programu.
*/
void loop() {
  
//
//if (digitalRead(ustaw_wyprost) == true){
//   reka -> wyprost();
//   Serial.println("wyprostowanie palcow");   
//}
//
//if (digitalRead(ustaw_ruch_sekwencyjny) == true){
//   reka -> ruchSekwencyjny(false, 1);
//    Serial.println("ruch kazdego palca po kolei");
//}
//
//if (digitalRead(ustaw_chwyt_cylindryczny) == true){
//    reka-> chwytCylindryczny(false, 1);
//     Serial.println("chwyt cylindryczny");
//}
//
//if (digitalRead(ustaw_chwyt_szczypcowy) == true){
//   reka-> chwytSzczypcowy(false, 1);
//   Serial.println("chwyt szczypcowy");
//}
//
//if (digitalRead(ustaw_chwyt_sferyczny) == true){
//    reka-> chwytSferyczny(false, 1);
//    Serial.println("Wybrales sekwencje 5 - chwyt sferyczny");
//}
//
//if (digitalRead(ustaw_chwyt_hakowy) == true){
//   reka-> chwytHakowy(false, 1);
//    Serial.println("Wybrales sekwencje 6 - chwytHakowy");
//}
//
//if (digitalRead(ustaw_chwyt_dloniowy) == true){
//   reka-> chwytDloniowy(false, 1);
//    Serial.println("Wybrales sekwencje 7 -chwytDloniowy");
//}
//
//if (digitalRead(ustaw_chwyt_lateralny) == true){
//   reka-> chwytLateralny(false, 1);
//   Serial.println("Wybrales sekwencje 8 -chwytLateralny");
//}


   if (stringComplete) 
 {
     Serial.println(inputString);
    // sekwencja 1 - wyprost palcow   
    if (inputString.startsWith("1")) 
    {
      Serial.println("Wybrales sekwencje 1 - wyprostowanie palcow");      
     reka -> wyprost();
    }

    // sekwencja 2 - ruchSekwencyjny
    if (inputString.startsWith("2")) 
    {
      Serial.println("Wybrales sekwencje 5 - ruch kazdego palca po kolei");
      reka -> ruchSekwencyjny(10);
    } 
     // sekwencja 3 - chwytCylindryczny
    if (inputString.startsWith("3")) 
    {
       Serial.println("Wybrales sekwencje 2 - chwyt cylindryczny");
       reka-> chwytCylindryczny(10);
    }
    // sekwencja 4 - chwyt szczypcowy
    if (inputString.startsWith("4")) 
    {
      Serial.println("Wybrales sekwencje 3 - chwyt szczypcowy");
     reka-> chwytSzczypcowy(10);
    }
    // sekwencja 5 - chwyt sferyczny
    if (inputString.startsWith("5")) 
    {
      Serial.println("Wybrales sekwencje 4 - chwyt sferyczny");
     reka-> chwytSferyczny(10);
    }
  
    // sekwencja 6 - chwytHakowy       
    if (inputString.startsWith("6")) 
    {
     Serial.println("Wybrales sekwencje 6 - chwytHakowy");
     reka-> chwytHakowy(10);
     }      

    // sekwencja 7 - chwytDloniowy       
    if (inputString.startsWith("7")) 
    {
     Serial.println("Wybrales sekwencje 7 -chwytDloniowy");
      reka-> chwytDloniowy(10);
     }      

    // sekwencja 8 - chwytHakowy       
    if (inputString.startsWith("8")) 
    {
     Serial.println("Wybrales sekwencje 8 -chwytLateralny");
      reka-> chwytLateralny(10);
     }   
     
    Serial.println("Wyierz nr sekwensji do wykonania od 1 do 8");      
    Serial.println(" 1 - wyprostowanie palcow");
    Serial.println(" 2 - ruch Sekwencyjny");
    Serial.println(" 3 - chwyt Cylindryczny");
    Serial.println(" 4 - chwyt szczypcowy");
    Serial.println(" 5 - chwyt sferyczny");
    Serial.println(" 6 - chwyt hakowy");
    Serial.println(" ");
    inputString = "";
    stringComplete = false;
  }
  
} //!< Pętla główna programu. Znajduje się tu cała logika działania programu.

void updateEnkoder00() { updateEnkoder(0, 0); } //!< Funkcja nieprzyjmująca parametrow wywołuję funkcję updateEnkoder() o dwóch parametrach.
void updateEnkoder01() { updateEnkoder(0, 1); }
void updateEnkoder10() { updateEnkoder(1, 0); }
void updateEnkoder11() { updateEnkoder(1, 1); }
void updateEnkoder20() { updateEnkoder(2, 0); }
void updateEnkoder21() { updateEnkoder(2, 1); }
void updateEnkoder30() { updateEnkoder(3, 0); }
void updateEnkoder31() { updateEnkoder(3, 1); }
void updateEnkoder40() { updateEnkoder(4, 0); }
void updateEnkoder41() { updateEnkoder(4, 1); }


/**
* Funkcja uruchamiana przez funckje void updateEnkoderXY(), które są funkcjami obsługi zdarzenia zmiany stanu na danym wyjściu cyfrowym.
* @param palid Zmienna oznaczająca numer palca.
* @param enkid Zmienna oznaczająca numer enkodera na danym palcu.
*/
void updateEnkoder(int palid, int enkid) {
  enk[palid][enkid] -> uaktualnij(); 
} //!< funkcja realizująca odwolanie do metody uaktualnij 

/*
  SerialEvent occurs whenever a new data comes in the
 hardware serial RX.  This routine is run between each
 time loop() runs, so using delay inside loop can delay
 response.  Multiple bytes of data may be available.
 */
void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    // add it to the inputString:
    inputString += inChar;
    // if the incoming character is a newline, set a flag
    // so the main loop can do something about it:
    if (inChar == '\n') {
      stringComplete = true;
    }
  }
}
