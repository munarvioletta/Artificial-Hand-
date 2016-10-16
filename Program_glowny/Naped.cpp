/**
 * @file
 * @author  Violetta Munar Ernandes 
 * @version 1.0
 */

#ifndef Naped_H
#define Naped_H

#define DEBUG_MODE false

#include "Arduino.h"

#include "Wyswietlanie.cpp"
#include "Silnik.cpp"
#include "Czujnik.cpp"
#include "Regulator.cpp"
#include <Servo.h>

/*! \brief Klasa Naped zawiera metody wirtualne które nadpisywane są przez klasy dziedziczące.
*
*
*/

class Naped {
  public:
    float wartosc_zadana = 0.0;
    Enkoder* czujnik; // obiekt typu Czujnik .  

    Naped () {

    }
    virtual int ustawWartosc(float wartosc) = 0;

    virtual int ustawWartosc(float wartosc, float wspolczynnik_mocy) = 0;

    virtual int ustawWspolczynnikMocy(float wspol_mocy) = 0;

    virtual float zwrocUchyb() = 0;

    virtual int uaktualnij() = 0;

    virtual int zatrzymaj() = 0;
}; 

/*! \brief Klasa Serwo dziedziczy po klasie Naped. Zawiera w sobie obiekt klasy Servo i metody obsługującej stwerowanie serwomechanizmem.  
*
*/
class Serwo : public Naped {
  public:
    int pin;
    Servo servo;

    Serwo(int pin) {
      this -> pin = pin;
      servo.attach(this -> pin);

    }
    
    float zwrocUchyb() {
      return 0.0;
    }

    int uaktualnij() {

      return 0;
    }

    int zatrzymaj() {
      Serial.println("TODO: Serwo.zatrzymaj()");
      digitalWrite(pin, LOW);
      return 0;
    }

    int ustawWartosc(float kat) {
      wartosc_zadana = kat;   
      wyswietl("Kat serwo", wartosc_zadana);
      servo.write((int) wartosc_zadana);

      return 0;
    } //!< metoda ustawiajaca zadaną wartość kąta na wejście serwomechanizmu.

    int ustawWartosc(float kat, float wspolmocy) {
      ustawWartosc(kat);
    }

     int ustawWspolczynnikMocy(float wspol_mocy) {
      return 0;
     }
 
};
/*! \brief Klasa dzidzicząca po klasie Naped.
 *         Pozwala na wybór i ustawienie parametrów dla obiektów typu Naped. 
 */
class SilnikRegulowany : public Naped {
  public:

    Silnik* silnik;
//    Czujnik*/ czujnik;
    Regulator* regulator;

    float wartosc_aktualna = 0.0;
    //float wartosc_zadana = 0.0;
    float wspolczynnik_mocy = 1.0;

    SilnikRegulowany(Regulator* reg, Enkoder* czu, Silnik* sil) {
      wartosc_zadana = 0.0;
      silnik = sil;
      czujnik = czu;
      regulator = reg;
    } //!< Konstruktor przypisujący wartości otrzymanych parametrów do zmiennych lokalnych

    int uaktualnij() {
      if (DEBUG_MODE) Serial.println("SilnikRegulowany.uaktualnij()");

      wartosc_aktualna = czujnik -> zwrocWartosc();
      regulator -> uaktualnij(wartosc_zadana - wartosc_aktualna);

      silnik -> ustawPredkosc(regulator -> zwrocSygnalSterujacy());
     // silnik -> ustawPredkosc(regulator -> zwrocSygnalSterujacy()*wspolczynnik_mocy);
      wyswietl("E:", czujnik -> zwrocWartosc()); 

      
      return 0;
    } //!< metoda odwołująca się do metod uaktualnij obiektu silnik, czujnik i regulator.

    int zatrzymaj() {
      Serial.println(" SilnikRegulowany.zatrzymaj()");

      silnik -> zatrzymaj();

      return 0;
    } //!< metoda zatrzymująca pracę silnika.

    int ustawWartosc(float a) {

      wartosc_zadana = a;

      return 0;
    }

    int ustawWartosc(float kat, float wspolmocy) {
      ustawWartosc(kat);
      ustawWspolczynnikMocy(wspolmocy);
      return 0;
    }

     int ustawWspolczynnikMocy(float wspol_mocy) {
      wspolczynnik_mocy = wspol_mocy;
      return 0;
     } //!< Metoda zwracająca współczynnik mocy

    float zwrocUchyb() {
      return wartosc_zadana - wartosc_aktualna;
    }//!< metoda liczy i zwraca wartość uchybu.

    /**
     * \return Uchyb
     */
}; 
 

#endif
