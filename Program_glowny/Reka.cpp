/**
 * @file
 * @author  Violetta Munar Ernandes 
 * @version 1.0
 */

#ifndef Reka_H
#define Reka_H

#include "Arduino.h"

#include "Wyswietlanie.cpp"

#include "Palec.cpp"
#include "Czujnik.cpp"


/*! \brief Klasa Reka.
*          Jest to główna klasa programu. Zarządza pracą wszystkich palców oraz czujnika prądu.
*/
class Reka {
  private:

  /**
  * Metoda zwracająca wartość, która informuje, czy wszystkie palce zakończyły swój ruch.
  * @return Wartość boolowska, informująca o zakończeniu ruchu wszystkich palców.
  */
    bool czyRuchPalcowZakonczony () {
      for (int i=0; i<IL_PALCOW; i++) {
        if (palce[i] -> czyRuchZakonczony() == false) {
          return false;
        }
      }
      return true;
    } //!< Metoda zwracająca wartość, która informuje, czy wszystkie palce zakończyły swój ruch.

/**
  * Metoda blokująca wykonywanie programu do momentu zakończenia wykonywania danego chwytu (ruchu).
  */

   int poczekajDoZakonczeniaRuchu(float czas_czekania) {
    float licznik_czasu_sek = 0.0;
      do {
        uaktualnij();
        delay( (int) (dt_sec*1000) );
        licznik_czasu_sek += dt_sec;
         Serial.println(" ");
        Serial.print("licznik czau ");
        Serial.println(licznik_czasu_sek);
        Serial.print("czyRuchPalcowZakonczony ");
        Serial.println(czyRuchPalcowZakonczony() );
        Serial.print("czas_czekania ");
        Serial.println(czas_czekania );
      } while (czyRuchPalcowZakonczony() == false &&  licznik_czasu_sek < czas_czekania);

      return 0;
   } //!< Metoda blokująca wykonywanie programu do momentu zakończenia wykonywania danego chwytu (ruchu).

   int poczekajDoZakonczeniaRuchu() {
      do {
        uaktualnij();
        delay( (int) (dt_sec*1000) );
      } while (czyRuchPalcowZakonczony() == false);

      return 0;
   } //!< Metoda blokująca wykonywanie programu do momentu zakończenia wykonywania danego chwytu (ruchu).

 /**
  * \param TRYB_ZATRZYMANIA_NORMALNY Zmienna określająca zatrzymanie ruchu i wznowienie pracy  
  * \param  TRYB_ZATRZYMANIA_AWARYJNY Zmienna określająca zatrzymanie ruchu i wznawianie pracy ręki po uprzednim resecie do ustawien poczatkowych  
  */
  public:
    static const char TRYB_ZATRZYMANIA_NORMALNY = 0;  
    static const char TRYB_ZATRZYMANIA_AWARYJNY = 1;

    static const char IL_PALCOW = 5;

    float dt_sec = 4.0/100.0;

    Palec* palce[IL_PALCOW];
    CzujnikPradu* czujnik_pradu;
    float natezenie_zatrzymania_amp;


/**
  * Konstruktor klasy Reka.
  * @param palce Tablica wskaźników na obiekty palców.
  * @param czujnik_pradu Wskaźnik do obiektu czujnika prądu.
  * @param nat_zatrz Zmienna natężenia zatrzymania ruchu w trybie awaryjnym.
  * @param dt Czas próbkowania - steruje ile razy na sekundę wykonywane są obliczenia.
  */

    Reka(Palec* palce[], CzujnikPradu* czujnik_pradu, float nat_zatrz, float dt) {
      this -> palce[0] = palce[0];
      this -> palce[1] = palce[1];
      this -> palce[2] = palce[2];
      this -> palce[3] = palce[3];
      this -> palce[4] = palce[4];
      this -> dt_sec = dt;
      this -> czujnik_pradu = czujnik_pradu;
      this -> natezenie_zatrzymania_amp = nat_zatrz;
    } //!< Konstruktor klasy Reka przypisujący wartości otrzymanych parametrów do zmiennych lokalnych.

/*! \brief Metoda uaktualniająca wszystkie elementy składowe ręki. 
  *         Jest propagowana wgłąb kolejnych elementów składowych.
  */
    int uaktualnij() {

      Serial.println("\n\nREKA");

      if (czujnik_pradu -> zwrocWartosc() > natezenie_zatrzymania_amp) {
        zatrzymaj(TRYB_ZATRZYMANIA_NORMALNY);
      }

      for (int i=0; i<IL_PALCOW; i++) {
        Serial.print("\n  PALEC "); Serial.println(i);
        palce[i] -> uaktualnij();
      }

        return 0;
    } //!< Metoda uaktualniająca wszystkie elementy składowe ręki. Jest propagowana wgłąb kolejnych elementów składowych.

    // CHWYTY

/**
  * Metoda realizująca chwyt szczypcowy.
  * @param pusty_chwyt Zmienna określająca rodzaj wykonywanego chwytu (pusty lub chwyt przedmiotu)
  * @param zmienna określająca maksywmalną siłę z jaką ma być chwytany dany przedmiot.
  */
    int chwytSzczypcowy(float max_sila) {
      Serial.println("\n\n\nTODO: Reka.chwytSzczypcowy()");

       palce[0] -> zegnijKat(45, 60, 60, max_sila);
       palce[1] -> zegnijKat(0, 90, 60, max_sila);
       palce[2] -> zegnijKat(0, 60, 60, max_sila);
       palce[3] -> zegnijKat(0, 60, 60, max_sila);
       palce[4] -> zegnijKat(0, 60, 60, max_sila);
       
       this -> poczekajDoZakonczeniaRuchu(6.0);
      zatrzymaj();
       
      return 0;
    } //!< Metoda realizująca chwyt szczypcowy.

/**
  * Metoda realizująca chwyt cylindryczny.
  */
    int chwytCylindryczny(float max_sila) {
      Serial.println("\n\n\nTODO: Reka.chwytCylind()");
      
       palce[0] -> zegnijKat(55, 90, 60, max_sila);
       palce[1] -> zegnijKat(0, 90, 90, max_sila);
       palce[2] -> zegnijKat(0, 90, 60, max_sila);
       palce[3] -> zegnijKat(0, 90, 90, max_sila);
       palce[4] -> zegnijKat(0, 60, 60, max_sila);
//      for (int i=0; i<IL_PALCOW; i++) {
//        palce[i] -> zegnijKat(120, 90, 60, max_sila);
//      }
//      //  palce[4] -> zegnijKat(120, 90, 90, max_sila);
      this -> poczekajDoZakonczeniaRuchu(5.0);
      zatrzymaj();
      return 0;
    } //!< Metoda realizująca chwyt cylindryczny.

/**
  * Metoda realizująca chwyt sferyczny.
  */
    int chwytSferyczny(float max_sila) {
      Serial.println("\n\n\nTODO: Reka.chwytSferyczny()");

       palce[0] -> zegnijKat(45, 60, 60, max_sila);
       palce[1] -> zegnijKat(0, 90, 60, max_sila);
       palce[2] -> zegnijKat(0, 60, 60, max_sila);
       palce[3] -> zegnijKat(0, 60, 60, max_sila);
       palce[4] -> zegnijKat(0, 60, 60, max_sila);

        this -> poczekajDoZakonczeniaRuchu(5.0);
         zatrzymaj();

      return 0;
    } //!< Metoda realizująca chwyt sferyczny.

/**
  * Metoda realizująca chwyt hakowy.
  */
    int chwytHakowy(float max_sila) {
      Serial.println("\n\n\nTODO: Reka.chwytHakowy()");
       
       palce[0] -> zegnijKat(45, 90, 30, max_sila);
       palce[1] -> zegnijKat(0, 0, 90, max_sila);
       palce[2] -> zegnijKat(0, 0, 90, max_sila);
       palce[3] -> zegnijKat(0, 0, 90, max_sila);
       palce[4] -> zegnijKat(0, 0, 90, max_sila);

        this -> poczekajDoZakonczeniaRuchu(5.0);
        zatrzymaj();

      return 0;
    } //!< Metoda realizująca chwyt hakowy.

/**
  * Metoda realizująca chwyt dłoniowy.
  */
    int chwytDloniowy(float max_sila) {
      Serial.println("\n\n\nTODO: Reka.chwytDloniowy()");
      
       palce[0] -> zegnijKat(45, 90, 90, max_sila);
       palce[1] -> zegnijKat(0, 90, 90, max_sila);
       palce[2] -> zegnijKat(0, 90, 60, max_sila);
       palce[3] -> zegnijKat(0, 90, 60, max_sila);
       palce[4] -> zegnijKat(0, 90, 60, max_sila);
       
        this -> poczekajDoZakonczeniaRuchu(5.0);
        zatrzymaj();

      return 0;
    }//!< Metoda realizująca chwyt dłoniowy.

/**
  * Metoda realizująca chwyt lateralny.
  */
    int chwytLateralny(float max_sila) {
      Serial.println("\n\n\n Reka.chwytLateralny()");
      
       palce[0] -> zegnijKat(45, 60, 30, max_sila);
       palce[1] -> zegnijKat(0, 30, 60, max_sila);
       palce[2] -> zegnijKat(0, 60, 60, max_sila);
       palce[3] -> zegnijKat(0, 90, 60, max_sila);
       palce[4] -> zegnijKat(0, 60, 60, max_sila);
       
       this -> poczekajDoZakonczeniaRuchu(5.0);
       zatrzymaj();

      return 0;
    } //!< Metoda realizująca chwyt lateralny.


/**
  * Metoda realizująca ruch pojedynczy kazdego palca.
  */
    int ruchSekwencyjny(float max_sila) {
      Serial.println("\n\n\n Reka.chwytLateralny()");
            
      for (int i=0; i<IL_PALCOW; i++) {
        
       
         palce[i] -> zegnijKat(45, 30, 30, max_sila);
//       palce[0] -> zegnijKat(145, 60, 60, max_sila);
//       palce[1] -> zegnijKat(0, 60, 60, max_sila);
//       palce[2] -> zegnijKat(0, 60, 60, max_sila);
//       palce[3] -> zegnijKat(0, 30, 30, max_sila);
//       palce[4] -> zegnijKat(0, 60, 60, max_sila);
  
        poczekajDoZakonczeniaRuchu();
       // this -> poczekajDoZakonczeniaRuchu(1.5);

        delay(1000);

        //wyprost_kalibr();
        palce[i] -> wyprost();        
        
        this -> poczekajDoZakonczeniaRuchu();
      
    } //!< Metoda realizująca chwyt lateralny.
    return 0;
 }

/**
  * Metoda realizująca wyprostowanie palców ręki.
  */
//    int wyprost_kalibr() {
//      Serial.println("\n\n\n Reka.wyprost_kalibr()");
//
//       //tu nastepuje maksymalne wyprostowanie palcow przez dana ilosc czasu
//      // aby wyprostowac wszystkie katy
//      for (int i=0; i<IL_PALCOW; i++) {
//        palce[i] -> napedy[0] -> ustawWartosc(-20, 1);
//        palce[i] -> napedy[1] -> ustawWartosc(-20, 1);
//        palce[i] -> napedy[0] -> uaktualnij();
//        palce[i] -> napedy[1] -> uaktualnij();
//      }
//
//      delay(1000);
//
//      // tu powinny juz byc wszystkie palce wyprostowane
//      // jesli enkodery wskazuja inaczej, to wskazuja zle i trzeba je wyzerowac
//      
//      for (int i=0; i<IL_PALCOW; i++) {
//          palce[i] -> zerujWskazaniaEnkoderow();
//        }
//
//        for (int i=0; i<IL_PALCOW; i++) {
//        palce[i] -> napedy[0] -> ustawWartosc(0, 1);
//        palce[i] -> napedy[1] -> ustawWartosc(0, 1);
//        palce[i] -> napedy[0] -> uaktualnij();
//        palce[i] -> napedy[1] -> uaktualnij();
//      }
//
//      return 0;
//    } //!< Metoda realizująca wyprostowanie palców ręki.

/**
  * Metoda realizująca wyprostowanie palców ręki.
  */
    int wyprost() {
      Serial.println("\n\n\n Reka.wyprost()");

      for (int i=0; i<IL_PALCOW; i++) {
        palce[i] -> wyprost();
      }

       this -> poczekajDoZakonczeniaRuchu(5);

      return 0;
    } //!< Metoda realizująca wyprostowanie palców ręki.

    // JEDEN PALEC
    // palce[nr].zegnijKat([d], a, b, c);

/**
  *  
  * @param tryb_zatrz tryb zatrzymania programu.
  */
    int zatrzymaj(int tryb_zatrz) {
      wyswietl("\n\n\nReka.zatrzymaj(tryb)", tryb_zatrz);

      switch (tryb_zatrz) {

        case TRYB_ZATRZYMANIA_NORMALNY:
          for (int i=0; i<IL_PALCOW; i++) {
            palce[i] -> zatrzymaj();
          }
          break;

        case TRYB_ZATRZYMANIA_AWARYJNY:
          resetuj_do_ustawien_poczatkowych();
          for (int i=0; i<IL_PALCOW; i++) {
            palce[i] -> zatrzymaj();
          }
          break;
      } 
      return 0;
    } //!< Metoda realizująca zatrzymanie w wybranym przez użytkownika trybie (Normalny, Awaryjny).

    int zatrzymaj() {
      Serial.println("\n\n\nReka.zatrzymaj()");

      return zatrzymaj(TRYB_ZATRZYMANIA_NORMALNY);
    } //!< Metoda realizująca zatrzymanie w trybie normalnym.

    
    int resetuj_do_ustawien_poczatkowych() {

     for (int i=0; i<IL_PALCOW; i++) {
        palce[i] -> wyprost();

      return 0;

    }  
  } //!< Metoda resetująca rękę do ustawien poczatkowych, oraz jej posiadajacych elementow.
};

#endif
