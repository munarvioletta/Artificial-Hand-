/**
 * @file
 * @author  Violetta Munar Ernandes
 * @version 1.0
 */

#ifndef Regulator_H
#define Regulator_H

#define DEBUG_MODE false

#include "Arduino.h"

#include "Wyswietlanie.cpp"

/*! \brief Bazowa klasa Regulator zawierająca metody wirtualne które są nadpisywane przez metody klas dziedziczących.
 *          
 */
class Regulator {
  public:
    float syg_sterujacy;

    Regulator () {
      syg_sterujacy = 0.0;
    }

    virtual int uaktualnij(float uchyb) = 0;

    virtual int zeruj() = 0;

    virtual float zwrocSygnalSterujacy() {

      return syg_sterujacy;
    }

};
/*! \brief Klasa RegulatorPID.
 *         Klasa ta zawiera algorytm liczący wartość sygnału sterującego prędkością silników. 
 */
class RegulatorPID : public Regulator {
  public:
    float p, i, d;
    float kp, ki, kd;
    float dt_sec;

    float uchyb_stary;
    float uchyb_aktualny;

    RegulatorPID (float kp, float ki, float kd, float dt_s) {
      p = 0;
      i = 0;
      d = 0;

      this -> kp = 0;
      this -> ki = 0;
      this -> kd = 0;

      this -> dt_sec = dt_s;

      uchyb_stary = 0.0;

      Serial.println("utworzono RegulatorPID");
    } //!< Konstruktor zawierający definicje otrzymanych paramentow. 
    
              
    int uaktualnij(float uchyb) {
      if (DEBUG_MODE) Serial.println("RegulatorPID.uaktualnij()");

      // liczenie p, i, d
      p  = kp * uchyb;
      i += ki/2*dt_sec * uchyb;
      d  = (kd/dt_sec) * (uchyb - uchyb_stary);

      // liczenie nowego syg_sterujacego
      syg_sterujacy = p + i + d;

      wyswietl("uchyb RegulatorPID =", uchyb);
      if (DEBUG_MODE) wyswietl("sterowanie RegulatorPID =", syg_sterujacy);

      return 0;
    } //!< Metoda licząca wartość sygnału sterującego przyjmując jako parametr wartość aktualnego uchybu. 

    int zeruj() {
      Serial.println(" RegulatorPID.zeruj()");

      p = 0;
      i = 0;
      d = 0;

      return 0;
    }  //!< Metoda zerująca wartości członów regulatora. 
};

/*! \brief Klasa RegulatorProporcjonalny.
 *         Klasa ta zawiera algorytm regulatora proporcjonalnego liczący wartość sygnału sterującego prędkością silników. 
 */
class RegulatorProporcjonalny : public Regulator {
  public:
    float k;

    RegulatorProporcjonalny (float k) {
      this -> k = k;

    } //!< Konstruktor przypisujący wartość otrzymanego parametru do zmiennej lokalnej

    int uaktualnij(float uchyb) {
      if (DEBUG_MODE) Serial.println("RegulatorProporcjonalny.uaktualnij()");

      syg_sterujacy = uchyb*k;    // liczenie nowego syg_sterujacego

      wyswietl(" U:", uchyb);
      if (DEBUG_MODE) wyswietl("sterowanie RegulatorProporcjonalny =", syg_sterujacy);

      return 0;
    } //!< Metoda liczy wartość sygnału sterującego na podstawie otrzymanego parametru uchybu

    int zeruj() {
      k = 0;
      return true;
    } //!< Metoda zerująca wartość wzmocnienia regulatora w celu przyjęcia kolejnej. 

};

/*! \brief Klasa RegulatorTrojstawny.
 *         Klasa ta zawiera algorytm regulatora trójstawnego liczący wartość sygnału sterującego prędkością silników. 
 */
class RegulatorTrojstawny : public Regulator {
  public:
    float k; 
    float przedzial_tolerancji;
    /**
     * @param k wartość maksymalna wmocnienienia
     * @param przedzial_tolerancji przedzial tolerancji dla którego liczona ma być wartość uchybu i sygnał sterujący.
     */
    RegulatorTrojstawny (float k, float tolerancja) {
      this -> k = k;
      this -> przedzial_tolerancji = tolerancja;
    } //!<  Metoda przypisująca wartości otrzymnaych parametrów do zmiennych lokalnych.

   /**
     * @param uchyb parametr na podtawie której metoda ta wylicza wartość sygnału sterującego.
     */
    int uaktualnij(float uchyb) {
      if (DEBUG_MODE) Serial.println("RegulatorTrojstawny.uaktualnij()");

      // liczenie nowego syg_sterujacego
      if (uchyb >= -przedzial_tolerancji/2 && uchyb <= przedzial_tolerancji/2) { // jesli uchyb miesci sie w przedziale tolerancji
        syg_sterujacy = 0.0;
      } else if (uchyb < 0) {
        syg_sterujacy = -k; // jesli uchyb mniejszy od zera
      } else {
        syg_sterujacy =  k; // jesli uchyb wiekszy od zera
      }

     // wyswietl("Reg Trojstawny uchyb  = ", uchyb);
     // if (DEBUG_MODE) wyswietl("sterowanie RegulatorTrojstawny =", syg_sterujacy);

      return 0;
    } //!<  Metoda uaktualniająca zmienne wewnętrzne obiektu tej klasy.

    int zeruj() {
      k = 0; 
      przedzial_tolerancji = 0;
      return true;
    }//!< Metoda zerująca wartości regulatora w celu przyjęcia nowych. 
};

#endif
