/**
 * @file
 * @author  Violetta Munar Ernandes
 * @version 1.0
 */

#ifndef Wyswietlanie_H
#define Wyswietlanie_H

#define DEBUG_MODE false

#include "Arduino.h"

inline int wyswietl(String str, float i) {
 // cout << "  >> " << str  << " " << i << endl;
  Serial.print(str); Serial.print(" "); Serial.print(i);Serial.print(" ");

  return 0;
} //!< Metoda wysyłająca opis i wartość zmiennej poprez interfejs szeregowy układu Arduino Due, które mogą być wświetlane w polu monitora szeregowego 

#endif
