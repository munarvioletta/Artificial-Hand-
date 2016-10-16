/**
 * @file
 * @author  Violetta Munar Ernandes 
 * @version 1.0
 */

#ifndef Palec_H
#define Palec_H

#define DEBUG_MODE false

#include "Arduino.h"

#include "Wyswietlanie.cpp"

#include "Czujnik.cpp"
#include "Naped.cpp"

/*! \brief Klasa Palec zawiera klasy dziedziczące PalecNorm i PalecKciuk.
 *         Klasa zaiwera metody wirtualne które nadpisywane są przez klasy dziedziczące.    
 */
class Palec {
  public:
    char nr_palca;

    float dokladnosc_ruchu_deg = 20;
    float maksymalny_nacisk_N = 100;

    CzujnikNacisku* czujnik_nacisku;
    SilnikRegulowany* napedy[2];
    Enkoder* enk[2];

    // konstruktor palec
    Palec() {
      nr_palca = 0;
    }

    virtual int uaktualnij() {
      wyswietl("Nac[N]: ", zwrocNacisk());

      for (int i=0; i<2; i++) {
        Serial.print(" N"); Serial.print(i+1);Serial.print(": ");
        napedy[i] -> uaktualnij();

        float wspolczynnik_mocy = zwrocWspolczynnikMocy();

        napedy[i] -> ustawWspolczynnikMocy(wspolczynnik_mocy);
      }  
      return 0;
    } //!< Metoda uaktualniająca zmienne wewnętrzne obiektu tej klasy.


    virtual int zatrzymaj() = 0;

    virtual int zegnijKat(float c, float a, float b, float maxnacisk) {
      if (DEBUG_MODE) Serial.println("Palec: redirecting from zegnijKat(3zm) -> zegnijKat(2zm)");
      zegnijKat(a, b, maxnacisk);

      return 0;
    } //!<  Metoda wirtualna nadpisywana przez metodą klasy dziedziczącej.

    virtual int zegnijKat(float a, float b, float maxnacisk) {
      if (DEBUG_MODE) Serial.println("Palec: redirecting from zegnijKat(2zm) -> zegnijKat(3zm)");
      zegnijKat(0.0, a, b, maxnacisk);

      return 0;
    }

    virtual int wyprost() = 0; //!< mwtoda prostująca palec

    float zwrocNacisk() {
      
      float nacisk = czujnik_nacisku->zwrocWartosc();  
   
      return nacisk;
    } //!<  Metoda zwracająca wartość nacisku na czujnik nacisku w [N]

    /*! \return wspolczynnik_mocy */  
    float zwrocWspolczynnikMocy() {
      float nacisk = this -> zwrocNacisk();
      float wspolczynnik_mocy = (maksymalny_nacisk_N - nacisk)/maksymalny_nacisk_N;

      if (wspolczynnik_mocy > 0)
        return wspolczynnik_mocy;
      else
        return wspolczynnik_mocy*0.1;
    } //!<  Metoda zwracająca wartość współczynnika mocy  dla wartości prędkości silnika. Mspółczynnik mocy obliczany jest na podstawie siły nacisku na pole czujnika nacisku.  
      
  
    bool czyRuchZakonczony() {
      for (int i=0; i<2; i++) {
        float uchyb = napedy[i] -> zwrocUchyb();
        if ((uchyb < -dokladnosc_ruchu_deg) || (uchyb > dokladnosc_ruchu_deg)) {
          return false;
        }
      }
      return true;
    } //!< Metoda sprawdzająca czy ruch palca (silnikók w palcu)został zakończony.

    int zerujWskazaniaEnkoderow() {
      
      napedy[0] -> czujnik -> ustawKat(10);
      napedy[1] -> czujnik -> ustawKat(10);

      return 0;
    }
};

/*! \brief Klasa PalecNorm
 *         Klasa zaiwera metody które realizują pracę poszczególnych palów.    
 *         Metody te odnoszą sie do palców które w swojej budowie zawierają dwa silniki.
 */

class PalecNorm : public Palec {
  public:

  /**
    * \param nr_palca nr ID palca.
    * \param napedy[] wskaźnik na obiekt typu Naped przypisany do danego obiektu typu PalecNorm
    * \param czuj_nac wskaźnik na obiekt typu Czujnik przypisany do danego obiektu typu PalecNorm
    */
    PalecNorm(char nr_palca, SilnikRegulowany* napedy[], CzujnikNacisku* czuj_nac) {
      this -> nr_palca = nr_palca;
      this -> napedy[0] = napedy[0];
      this -> napedy[1] = napedy[1];
      this -> czujnik_nacisku = czuj_nac;
    }  //!< Konstruktor PalecNorm realizujący przypisanie otrzymanych parametrów do zmiennych lokalnych.

  
    int uaktualnij() {
//      if (DEBUG_MODE) wyswietl("\n\nPalecNorm.uaktualnij()", nr_palca);

      wyswietl("Nac[N]: ", zwrocNacisk());

      for (int i=0; i<2; i++) {
        Serial.print(" N"); Serial.print(i+1);Serial.print(": ");
        napedy[i] -> uaktualnij();

        float wspolczynnik_mocy = zwrocWspolczynnikMocy();

        napedy[i] -> ustawWspolczynnikMocy(wspolczynnik_mocy);
      } //!< Metoda uaktualniająca zmienne wewnętrzne obiektu tej klasy.

      return 0;
    } 

    int zatrzymaj() {
      if (DEBUG_MODE) wyswietl("\n\nPalecNorm.zatrzymaj()", nr_palca);

      for (int i=0; i<2; i++) {
        napedy[i] -> zatrzymaj();
      }

      return 0;
    } //!<  Metoda zatrzymująca ruch palca odowłując się do metody zatrzymującej napędy znajdujęce się w danym palcu.

    int zegnijKat(float a, float b, float maxnacisk) {
      if (DEBUG_MODE) wyswietl("\n\nPalecNorm.zegnijKat(3zm)", nr_palca);

      maksymalny_nacisk_N = maxnacisk;
      float wspolczynnik_mocy = zwrocWspolczynnikMocy();//!<  Metoda uaktualniająca zmienne wewnętrzne obiektu tej klasy.

      napedy[0] -> ustawWartosc(a, wspolczynnik_mocy);
      napedy[1] -> ustawWartosc(b, wspolczynnik_mocy);

      return 0;
    } //!<  Metoda ustawiająca wartość kątów pod jakim ma zgiąć się palec (napędy znajdujące sie w palcu - naped[0] i naped[1] ).

    int wyprost() {
      if (DEBUG_MODE) wyswietl("\n\nPalecNorm.wyprost()", nr_palca);

      zegnijKat(0, 0, 10);

      return 0;
    } //!< Metoda ustawiająca wszystkie napędy w danym palcu na osiągnięcie położenia początkowego. 
};

/*! \brief Klasa PalecKciuk
 *         Klasa zaiwera metody które realizują pracę kciuka.    
 *         Metody te odnoszą sie do palca który w swojej budowie zawiera dwa silniki i serwomechanizm.
 */

class PalecKciuk : public Palec {
  public:
    Serwo* serwo;

    PalecKciuk(char nr_palca, SilnikRegulowany* napedy[], Serwo* serwo, CzujnikNacisku* czuj_nac) {
      this -> nr_palca = nr_palca;
      this -> napedy[0] = napedy[0];
      this -> napedy[1] = napedy[1];
      this -> serwo = serwo;
      this -> czujnik_nacisku = czuj_nac;
    }//!< Konstruktor PalecKciuk realizujący przypisanie otrzymanych parametrów do zmiennych lokalnych.
      /**
    * \param nr_palca nr ID palca.
    * \param napedy[] wskaźnik na obiekt typu Naped przypisany do danego obiektu typu PalecKciuk
    * \param serwo wskaźnik na obiekt typu Naped przypisany do danego obiektu PalecKciuk
    * \param czuj_nac wskaźnik na obiekt typu Czujnik przypisany do danego obiektu typu PalecKciuk
    */

    int uaktualnij() {
      if (DEBUG_MODE) wyswietl("\n PalecKciuk.uaktualnij()", nr_palca);

      wyswietl("Nac[N]: ", zwrocNacisk());

      for (int i=0; i<2; i++) {
        Serial.print(" N"); Serial.print(i+1);Serial.print(": ");
        napedy[i] -> uaktualnij();
        
        float wspolczynnik_mocy = zwrocWspolczynnikMocy();

        napedy[i] -> ustawWspolczynnikMocy(wspolczynnik_mocy);                
      }

      return 0;
    } //!<  Metoda uaktualniająca zmienne wewnętrzne obiektu tej klasy.

    int zatrzymaj() {
      wyswietl("\n\nPalecKciuk.zatrzymaj()", nr_palca);

      for (int i=0; i<2; i++) {
        napedy[i] -> zatrzymaj();
      }

      serwo -> zatrzymaj();

      return 0;
    }  //!<  Metoda zatrzymująca ruch palca odowłując się do metody zatrzymującej napędy i metody zatrzymującej serwomechanizm znajdujęce się w danym palcu.

    int zegnijKat(float c, float a, float b, float maxnacisk) {
      if (DEBUG_MODE) wyswietl("\n PalecKciuk.zegnijKat(3zm)", nr_palca);

      maksymalny_nacisk_N = maxnacisk;
      float wspolczynnik_mocy = zwrocWspolczynnikMocy();

      napedy[0] -> ustawWartosc(a, wspolczynnik_mocy);
      napedy[1] -> ustawWartosc(b, wspolczynnik_mocy);
      serwo -> ustawWartosc(c);
    
      return 0;
    } //!<  Metoda ustawiająca wartość kątów pod jakim mają się zgiąć napędy znajdujące sie w palcu - naped[0] i npaed[1] oraz kąt pod jakim ma się obrócić serwomechanizm.


    int wyprost() {
      wyswietl("\n\nPalecKciuk.wyprost()", nr_palca);

      zegnijKat(30, 0, 0, 10);

      return 0;
    } //!<  Metoda ustawiająca wszystkie napędy w danym palcu oraz serwomechanizm na osiągnięcie położenia początkowego. 
};

#endif
