/**
 * @file
 * @author  Violetta Munar Ernandes 
 * @version 1.0
 */

#ifndef Silnik_H
#define Silnik_H

#define DEBUG_MODE false

#include "Arduino.h"

#include "Wyswietlanie.cpp"
#include "DualMC33926MotorShield.h"

/*! \brief Klasa Silnik zawierająca metody sterujące pracą silników.
 *          
 */

class Silnik {
  public:

    static const char TRYB_ZATRZYMANIA_NORMALNY = 0; // mozna wznowic od razu; ustawianie predkosci na 0
    static const char TRYB_ZATRZYMANIA_AWARYJNY = 1; // wznawianie po uprzednim resecie do ustawien poczatkowych

    float mnoznik_regulacji_przod;
    float mnoznik_regulacji_tyl;
    float aktualna_predkosc;

    int pin_silnika0;
    int pin_silnika1;
    int pin_silnika2;

    DualMC33926MotorShield* sterownik;
    int id;

    Silnik(DualMC33926MotorShield* ster, int id, float mnoznik_reg_przod, float mnoznik_reg_tyl) {
      this -> sterownik = ster;
      this -> id = id;

      this -> mnoznik_regulacji_przod = mnoznik_reg_przod;
      this -> mnoznik_regulacji_tyl = mnoznik_reg_tyl;
      this -> aktualna_predkosc = 0.0;

     // wyswietl("utworzono Silnik", (unsigned int) this);
    } //!< Konstruktor zawierający trzy parametry - rodzaj setrownika, id silnika i mnożnik regulacji. 
      /**< Mnożnik regulacji stosuje się w celu zwiększenia wartości sygnału sterującego. Domyślnie przyjmuję wartość 1.  */
 
    virtual int zatrzymaj() {
     // wyswietl(" Silnik.zatrzymaj()", (unsigned int) this);

      ustawPredkosc(0.0);

      return 0;
    } //!<  wirtualna metoda bazowa zatrzumująca pracę silnika

    virtual int ustawPredkosc(float predkosc) {
     if (predkosc > 0){
      this -> aktualna_predkosc = predkosc*mnoznik_regulacji_przod;
     }
     else {
      this -> aktualna_predkosc = predkosc*mnoznik_regulacji_tyl;
     }
      // ustawianie silnika fizycznego na predkosc
      if (this -> id == 0) {             
        this -> sterownik -> setM1Speed((int) aktualna_predkosc);
        wyswietl(" P M1:", aktualna_predkosc);  
      } else if (this -> id == 1) {         
        this -> sterownik -> setM2Speed((int) aktualna_predkosc);
        wyswietl(" P M2:", aktualna_predkosc);  
      }

      return 0;
    } //!<  wirtualna metoda bazowa ustawiająca zadaną prędkość dla silnika w zależności od nr ID silnika.
};

#endif
