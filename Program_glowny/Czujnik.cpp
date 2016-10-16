/**
 * @file
 * @author  Violetta Munar Ernandes 
 * @version 1.0
 */

#ifndef Czujnik_H
#define Czujnik_H

#include "Arduino.h"

#include "Wyswietlanie.cpp"


/*! \brief Klasa abstrakcyjna Czujnik.
*          Klasa zawierająca wirtualne metody które przy wywoływaniu nadpisywane są przez metody klasy dziedziczącej.
*/
class Czujnik {
  public:

    virtual int   uaktualnij()    = 0; //!< metoda bazowa uaktualnij() 
    virtual float zwrocWartosc()  = 0; //!< metoda zwracająca dowolną wartość
};


/*! \brief Klasa ta zawiera metody liczące siłę nacisku i zwracające wartość tego nacisku do programu głównego.
*/
class CzujnikNacisku : public Czujnik {
  private:
   
    int uaktualnij() {
      if (DEBUG_MODE) Serial.println("CzujnikNacisku.uaktualnij()");

      int naciskADC = analogRead(FSR_Pin);
      FSR_Voltage = (naciskADC/1023.0)* 3.3;
//      Serial.print("Voltage reading in V = ");
//      Serial.println(FSR_Voltage);
      fsrResistance = 3.3 - FSR_Voltage;     
      fsrResistance *= 1.95;                // 1.95KOM resistor
      fsrResistance /= FSR_Voltage;
//      Serial.print("FSR resistance in Kohms = ");
//      Serial.println(fsrResistance);

      FSR_Force = pow(fsrResistance, -1.1637385058);
      FSR_Force*= 1369.8967136477;
      FSR_Force*=9.80665;
      FSR_Force /= 1000;
//      Serial.print("Force in Newtons: ");
//      Serial.println(FSR_Force);

      float e = 2.718281828459045;
      FSR_Force = (pow(e, ((FSR_Force + 8.8505)/2.5413))*9.80665 )/1000.0;

      // tu trzeba bedzie odjac troche, zeby w spoczynku bylo 0N
      FSR_Force -= 0.32;
      
      nacisk_N = FSR_Force;

      return 0;
    } //!< Metoda uaktualniająca zmienne wewnętrzne obiektu tej klasy. Liczony jest tu siła nacisku na czujnik nacisku w Newtonach.

    

  public:
    float nacisk_N;
    float nacisk_max_n;
    int FSR_Pin;
    float fsrResistance;
    float FSR_Voltage;
    float FSR_Force;
       
   /*! \brief  Konstruktor - po otrzymaniu paramtru pin_czuj przypisuję go do zmiennej odpowiadającej pinu z którego odczytywana jest wartosc nacisku i ustawia ten pin jako wejściowy.
    * \param pin_czuj Wyjście cyfrowe odpowiadające za komunikację czujnika z płytką Arduino Due.
    */   
    CzujnikNacisku(int pin_czuj) {
      FSR_Pin = pin_czuj;
      pinMode(FSR_Pin, INPUT);
      nacisk_N = 0.0;
      nacisk_max_n = 10.0;
    } //!< Konstruktor - po otrzymaniu paramtru pin_czuj przypisuję go do zmiennej odpowiadającej pinu z którego odczytywana jest wartosc nacisku i ustawia ten pin jako wejściowy.
      
      float zwrocWartosc() {
      return this -> zwrocNaciskN();
    } //!< metoda zwracająca wartość nacisku. Nadpisuje metodę z klasy bazowej.
    
     /**
    *\return Wartość siły w Newtonach.
    */
      
    float zwrocNaciskN() {
      uaktualnij();

      return nacisk_N;
    } //!< metoda zwracająca wartość nacisku w Newtonach.

     /**
    * \return Wartość siły w Newtonach.
    */

};

/*! \brief Klasa CzujnikPradu dziedziczy metody uaktualnij() i zwrocWartosc() z abstrakcyjnej klasy Czujnik.
*/
class CzujnikPradu : public Czujnik {
  private:
  /**
  * Metoda uaktualniająca zmienne wewnętrzne obiektu tej klasy.
  * Liczony jest tu prąd w Amperach.
  */
  int uaktualnij() {

      if (DEBUG_MODE) wyswietl("CzujnikPradu.uaktualnij()", AmpsRMS);

      readValue = analogRead(sensorIn);
      Voltage = readValue/1023.0*3.3;
      AmpsRMS = (15.7895*Voltage)-39.47368;

     return 0;
    } //!< Metoda przelicza liczbę odczytaną z przetwornika A/C na wartość pobieranego prądu w [A]

  public:

    int sensorIn = 0;
    //int mVperAmp = 66; // use 100 for 20A Module and 185 for 5 Module
    double Voltage = 0;
    double AmpsRMS = 0;
    float readValue = 0;

    /**
    * \param pin_czujnika Pin odpowiedzialny za komunikację z Arduino Due.
    */
    CzujnikPradu(int pin_czujnika) {
      this -> sensorIn = pin_czujnika;
      this -> AmpsRMS = 0.0;
      pinMode(sensorIn, INPUT);
    }  //!< Konstruktor zawirający deklarację zmiennych i ustawia otrzymany parametr jako pin wejściowy.
   /**
     \return Prąd w Amperach.
   */
    float zwrocWartosc() {    
      return this -> zwrocNatezenieAmp();
    } //!< metoda zwracająca dowolną wartość. Nadpisuje metodę z klasy bazowej.

    /**
    * \return Prąd w Amperach.
    */
    float zwrocNatezenieAmp() {
      uaktualnij();

      if (DEBUG_MODE) wyswietl("CzujnikPradu.zwrocNatezenieAmp()", AmpsRMS);

      Serial.print("\nCzujnikPradu: [A]: ");Serial.print(AmpsRMS);Serial.print("; [V]: ");Serial.println(Voltage);

      return AmpsRMS;
    } //!< metoda zwracająca wartość natężenia prądu
};

/*! \brief Klasa Enkoder definiuje zachowanie Enkodera inkrementalnego, 
 *         pozwala na podstawie ilości impulsów i sekwencji zmian na impuls określić kąt obrotu.
*/

class Enkoder : public Czujnik {
  public:
    float aktualny_kat;
    int ilosc_zmian_na_imp;
    int pin_czujnika0;
    int pin_czujnika1;
    volatile int lastEncoded = 0;
    volatile long encoderValue = 0;
    long lastencoderValue = 0;
    volatile float angle = 0;

    int lastMSB = 0;
    int lastLSB = 0;

    /**
    * Konstruktor.
    * \param pin0 Pin odpowiedzialny za komunikację z Arduino Due podłączony do kanału A enkodera.
    * \param pin1 Pin odpowiedzialny za komunikację z Arduino Due podłączony do kanału B enkodera.
    * \param il_zm_na_imp Zmienna określająca ilość zmian na jeden impuls.
    * 
    */
    Enkoder(int pin0, int pin1, int il_zm_na_imp) {

      this -> pin_czujnika0 = pin0;
      this -> pin_czujnika1 = pin1;
      this -> ilosc_zmian_na_imp = il_zm_na_imp;

      pinMode(pin_czujnika0, INPUT);
      pinMode(pin_czujnika1, INPUT);

      digitalWrite(pin_czujnika0, HIGH); // ustaiwnie pinu na stan wysoki
      digitalWrite(pin_czujnika1, HIGH); // ustaiwnie pinu na stan wysoki

      this -> aktualny_kat = 0.0;
    } //!<  Konstruktor zawirający deklarację zmiennych i ustawia otrzymane parametry jako pin wejściowy.

    /**
    *  
    * Wartość kąta zawiera się w przedziale (-180, 180).
    */
    int uaktualnij() {  // to ruszamy, wartosc kata przypisujemy do zmiennej 'aktualny_kat'

      int MSB = digitalRead(pin_czujnika0); //MSB = most significant bit
      int LSB = digitalRead(pin_czujnika1); //LSB = least significant bit

      int encoded = (MSB << 1) |LSB; //converting the 2 pin value to single number
      int sum  = (lastEncoded << 2) | encoded; //adding it to the previous encoded value

      if(sum == 0b1101 || sum == 0b0100 || sum == 0b0010 || sum == 0b1011) encoderValue--;
      if(sum == 0b1110 || sum == 0b0111 || sum == 0b0001 || sum == 0b1000) encoderValue++;

      // KAT ZAWIERA SIE W PRZEDZIALE (-180 - 180)

      //---------DLA DUZEGO ENKODERA--------------------

      if (encoderValue>ilosc_zmian_na_imp*12/2) encoderValue -= ilosc_zmian_na_imp*12; // 2*12 changes -> 180*, 4*12ch -> 360*
      if (encoderValue<-ilosc_zmian_na_imp*12/2) encoderValue += ilosc_zmian_na_imp*12; // 2*12 changes -> 180*, 4*12ch -> 360*
      angle = encoderValue/ilosc_zmian_na_imp/12.0*360; // dobra dzialajaca wersja

      //---------DLA MNIEJSZEGO ENKODERA--------------------
      //  if (encoderValue>2*12) encoderValue -= 4*12; // 2*12 changes -> 180*, 4*12ch -> 360*
      //  if (encoderValue<-2*12) encoderValue += 4*12; // 2*12 changes -> 180*, 4*12ch -> 360*
      //  angle = encoderValue/4.0/12.0*360;

      lastEncoded = encoded; //store this value for next time
      aktualny_kat = angle;

      if (DEBUG_MODE) wyswietl("Enkoder.uaktualnij()", aktualny_kat);

      return 0;
    }//!< Metoda ta zlicza impulsy i rejestruje zmiany wartości na podstawie czego liczy wartość kąta w stopniach o jaki obrócił się wał silnika.
  
    //Metoda uaktualniająca zmienne wewnętrzne obiektu tej klasy.

    /**
    * Metoda dziedziczonej klasy Czujnik wywołująca metodę zwrocKat(). Nadpisuje metodę z klasy bazowej.
    * \return Kąt w stopniach.
    */
    float zwrocWartosc() {
     return this -> zwrocKat();
    }//!<  Metoda wywołuję metodę zwrocKat()

    /**
    * Metoda zwracająca wartość kąta obrotu wału silnika.
    * \return Kąt w stopniach.
    */
    float zwrocKat() {
      uaktualnij();
      
      return aktualny_kat;
    }//!<  Metoda zwracająca wartość kąta w stopniach o jak obrócił się wał silnika
     
////////////////////////////zeruj kat//////////////////////////////////////////////
    void ustawKat(float kat) {
      aktualny_kat = kat;
      
     }//!<  Metoda zwracająca wartość kąta w stopniach o jak obrócił się wał silnika 
};

#endif
