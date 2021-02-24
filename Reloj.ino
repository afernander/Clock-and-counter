#include <Keypad.h>

// Real time clock and calendar with set buttons using DS1307 and Arduino

// include LCD library code
#include <LiquidCrystal.h>
// include Wire library code (needed for I2C protocol devices)
#include <Wire.h>

const byte filas = 4;                    //define las 4 filas del teclado
const byte columnas = 3;                 //define las 3 columnas del teclado
char teclas[filas][columnas] =           //inicializa filas y columnas
{
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'*', '0', '#'}
};
byte pinesfilas[filas] = {25, 26, 27, 28}; //defines los pines digitales de las cuatro fila
byte pinescolumnas[columnas] = {22, 23, 24}; //defines los pines digitales de las tres columnas
Keypad teclado = Keypad( makeKeymap(teclas), pinesfilas,  pinescolumnas, filas, columnas ); //lectura del teclado
// LCD module connections (RS, E, D4, D5, D6, D7)
LiquidCrystal lcd(7, 6, 5, 4, 11, 10);

char k;                                    //Define k como caracter
int led = 22;
String alarma1, alarma2, alarma3, adate1, adate2, adate3;
int botonAl = 2;
int botonA2 = 18;
int botonA3 = 19;
int enter = 3;
int tiempo;

int teclados()                            //Funcion teclados
{
  int val;
  k = teclado.getKey();                    /*Llamar la función del teclado y almacenar el valor
                                         digitado en una variable (k) tipo caracter. Si no se oprime
                                         ninguna tecla el teclado retornara el carácter nulo.*/

  while ((k == '\0') || (k == '#')) //Si no se oprime ninguna tecla
  {
    k = teclado.getKey();                   //Sigue llamando al teclado
  }

  val = k - 48;                           //Poner el valor de k menos 48 a val
  return k == '*' ? 10 : val;
}

void setup() {
  pinMode(8, INPUT_PULLUP);                      // button1 is connected to pin 8
  pinMode(9, INPUT_PULLUP);                      // button2 is connected to pin 9
  // set up the LCD's number of columns and rows
  lcd.begin(16, 2);
  Wire.begin();                                  // Join i2c bus
  pinMode(botonAl, INPUT);
  pinMode(botonA2, INPUT);
  pinMode(botonA3, INPUT);
  pinMode(enter, INPUT);
  pinMode(12, OUTPUT);
  pinMode(17, OUTPUT);
}

char Time[]     = "TIME:  :  :  ";
char Calendar[] = "DATE:  /  /20  ";
byte i, second, minute, hour, date, month, year;

void DS1307_display() {
  // Convert BCD to decimal
  second = (second >> 4) * 10 + (second & 0x0F);
  minute = (minute >> 4) * 10 + (minute & 0x0F);
  hour   = (hour >> 4)   * 10 + (hour & 0x0F);
  date   = (date >> 4)   * 10 + (date & 0x0F);
  month  = (month >> 4)  * 10 + (month & 0x0F);
  year   = (year >> 4)   * 10 + (year & 0x0F);
  // End conversion
  Time[12]     = second % 10 + 48;
  Time[11]     = second / 10 + 48;
  Time[9]      = minute % 10 + 48;
  Time[8]      = minute / 10 + 48;
  Time[6]      = hour   % 10 + 48;
  Time[5]      = hour   / 10 + 48;
  Calendar[14] = year   % 10 + 48;
  Calendar[13] = year   / 10 + 48;
  Calendar[9]  = month  % 10 + 48;
  Calendar[8]  = month  / 10 + 48;
  Calendar[6]  = date   % 10 + 48;
  Calendar[5]  = date   / 10 + 48;
  lcd.setCursor(0, 0);
  lcd.print(Time);                               // Display time
  lcd.setCursor(0, 1);
  lcd.print(Calendar);                           // Display calendar
}
void blink_parameter() {
  byte j = 0;
  while (j < 10 && digitalRead(8) && digitalRead(9)) {
    j++;
    delay(25);
  }
}
byte edit(byte x, byte y, byte parameter) {
  char text[3];
  while (!digitalRead(8));                       // Wait until button (pin #8) released
  while (true) {
    while (!digitalRead(9)) {                    // If button (pin #9) is pressed
      parameter++;
      if (i == 0 && parameter > 23)              // If hours > 23 ==> hours = 0
        parameter = 0;
      if (i == 1 && parameter > 59)              // If minutes > 59 ==> minutes = 0
        parameter = 0;
      if (i == 2 && parameter > 31)              // If date > 31 ==> date = 1
        parameter = 1;
      if (i == 3 && parameter > 12)              // If month > 12 ==> month = 1
        parameter = 1;
      if (i == 4 && parameter > 99)              // If year > 99 ==> year = 0
        parameter = 0;
      sprintf(text, "%02u", parameter);
      lcd.setCursor(x, y);
      lcd.print(text);
      delay(200);                                // Wait 200ms
    }
    lcd.setCursor(x, y);
    lcd.print("  ");                             // Display two spaces
    blink_parameter();
    sprintf(text, "%02u", parameter);
    lcd.setCursor(x, y);
    lcd.print(text);
    blink_parameter();
    if (!digitalRead(8)) {                       // If button (pin #8) is pressed
      i++;                                       // Increament 'i' for the next parameter
      return parameter;                          // Return parameter value and exit
    }
  }
}

void loop() {

  if (digitalRead(botonAl)) {
    alarma1 = alarma(1);

  } else if (digitalRead(botonA2)) {

    alarma2 = alarma(2);
  } else if (digitalRead(botonA3)) {
    alarma3 = alarma(3);

  }
  if (digitalRead(enter)) {
    showalarm();
  }

  if (digitalRead(17)) {
    temp();
  }
  check(alarma1, 1);
  check(alarma2, 2);
  check(alarma3, 3);

  //Estamos mariqueando
  if (!digitalRead(8)) {                         // If button (pin #8) is pressed
    i = 0;
    hour   = edit(5, 0, hour);
    minute = edit(8, 0, minute);
    date   = edit(5, 1, date);
    month  = edit(8, 1, month);
    year   = edit(13, 1, year);
    // Convert decimal to BCD
    minute = ((minute / 10) << 4) + (minute % 10);
    hour = ((hour / 10) << 4) + (hour % 10);
    date = ((date / 10) << 4) + (date % 10);
    month = ((month / 10) << 4) + (month % 10);
    year = ((year / 10) << 4) + (year % 10);
    // End conversion
    // Write data to DS1307 RTC
    Wire.beginTransmission(0x68);               // Start I2C protocol with DS1307 address
    Wire.write(0);                              // Send register address
    Wire.write(0);                              // Reset sesonds and start oscillator
    Wire.write(minute);                         // Write minute
    Wire.write(hour);                           // Write hour
    Wire.write(1);                              // Write day (not used)
    Wire.write(date);                           // Write date
    Wire.write(month);                          // Write month
    Wire.write(year);                           // Write year
    Wire.endTransmission();                     // Stop transmission and release the I2C bus
    delay(200);                                 // Wait 200ms
  }
  Wire.beginTransmission(0x68);                 // Start I2C protocol with DS1307 address
  Wire.write(0);                                // Send register address
  Wire.endTransmission(false);                  // I2C restart
  Wire.requestFrom(0x68, 7);                    // Request 7 bytes from DS1307 and release I2C bus at end of reading
  second = Wire.read();                         // Read seconds from register 0
  minute = Wire.read();                         // Read minuts from register 1
  hour   = Wire.read();                         // Read hour from register 2
  Wire.read();                                  // Read day from register 3 (not used)
  date   = Wire.read();                         // Read date from register 4
  month  = Wire.read();                         // Read month from register 5
  year   = Wire.read();                         // Read year from register 6
  DS1307_display();                             // Diaplay time & calendar
  delay(50);                                    // Wait 50ms
}

//Aquí estoy

String alarma(int a) {
  String fecha;
  String alarma;
  bool terminar = true;
  int mm = 0;
  int hh = 0;
  int dd = 0;
  int mes = 0;
  int yy = 0;
  int aux;
  tiempo = 0;
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Alarm: Hour");
  for (int i = 0; i < 2; i++) {
    aux = teclados();
    tiempo *= 10;
    tiempo += aux;
    if (tiempo > 23)
      tiempo = 23;
    lcd.setCursor(0, 1);
    lcd.print(tiempo);
  }
  delay(500);
  hh = tiempo;
  aux = 0;
  tiempo = 0;
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Alarm" + String(a) + ": Minute");
  for (int i = 0; i < 2; i++) {
    aux = teclados();
    tiempo *= 10;
    tiempo += aux;
    if (tiempo > 59)
      tiempo = 59;
    lcd.setCursor(0, 1);
    lcd.print(tiempo);

  }
  mm = tiempo;
  tiempo = 0;
  delay(500);
  //ingresar dia

  aux = 0;
  tiempo = 0;
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Alarm" + String(a) + ": Day");
  for (int i = 0; i < 2; i++) {
    aux = teclados();
    tiempo *= 10;
    tiempo += aux;
    if (tiempo > 31)
      tiempo = 31;
    lcd.setCursor(0, 1);
    lcd.print(tiempo);

  }
  dd = tiempo;
  tiempo = 0;
  delay(500);
  //ingresar mes

  aux = 0;
  tiempo = 0;
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Alarm" + String(a) + ": Mounth");
  for (int i = 0; i < 2; i++) {
    aux = teclados();
    tiempo *= 10;
    tiempo += aux;
    if (tiempo > 12)
      tiempo = 12;
    lcd.setCursor(0, 1);
    lcd.print(tiempo);

  }
  mes = tiempo;
  tiempo = 0;
  delay(500);
  //año
  aux = 0;
  tiempo = 0;
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Alarm" + String(a) + ": Year");
  for (int i = 0; i < 4; i++) {
    aux = teclados();
    tiempo *= 10;
    tiempo += aux;
    if (tiempo > 9999)
      tiempo = 9999;
    lcd.setCursor(0, 1);
    lcd.print(tiempo);

  }
  yy = tiempo;
  tiempo = 0;
  delay(500);


  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Alarm" + String(a) + " set to");
  lcd.setCursor(0, 1);
  //alarma hora
  if (hh < 10) {
    alarma = "0" + (String)hh;
  } else {
    alarma = (String)hh;
  }
  alarma = alarma + ":";
  if (mm < 10) {
    alarma = alarma + "0" + (String)mm;
  } else {
    alarma = alarma + (String)mm;
  }
  
  //alarma fecha
  if (dd < 10) {
    fecha = "0" + String(dd);
  } else {
    fecha = String(dd);
  }
  fecha= fecha+"/";
  if (mes < 10) {
    fecha =fecha+ "0" + String(mes);
  } else {
    fecha = fecha+String(mes);
  }
  fecha= fecha+"/"+String(yy);
  
  String finalalarm= alarma+" "+fecha;
  lcd.print(finalalarm);
  
  delay(1000);
  lcd.clear();
  return finalalarm;
}

void check(String alr, int a) {
  
  String t = String(Time[5]) + String(Time[6]) + String(Time[7]) + String(Time[8]) 
            + String(Time[9]) 
            +" "+String(Calendar[5])+String(Calendar[6])+String(Calendar[7])+String(Calendar[8])
            +String(Calendar[9])+String(Calendar[10])+String(Calendar[11])+String(Calendar[12])
            +String(Calendar[13])+String(Calendar[14]);
String seg= String(Time[11]) + String(Time[12]);

  if (alr.equals(t)&& seg.equals("00")) {
    for (int i = 0; i <= 10; i++) {
      lcd.clear();
      digitalWrite(12, LOW);
      delay(500);
      digitalWrite(12, HIGH);
      lcd.setCursor(0, 0);
      lcd.print("Alarm " + String(a));
      lcd.setCursor(0, 1);
      lcd.print(alr);
      delay(500);
    }
    digitalWrite(12, LOW);
     lcd.clear(); 
  }
 
}

void showalarm() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("ALarm 1");
  lcd.setCursor(0, 1);
  lcd.print(alarma1);
  delay(2000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("ALarm 2");
  lcd.setCursor(0, 1);
  lcd.print(alarma2);
  delay(2000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("ALarm 3");
  lcd.setCursor(0, 1);
  lcd.print(alarma3);
  delay(2000);
  lcd.clear();

}

void temp() {

  String timer;
  bool terminar = true;
  int mm = 0;
  int ss = 0;
  int aux;
  tiempo = 0;
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Minutes");
  for (int i = 0; i < 2; i++) {
    aux = teclados();
    tiempo *= 10;
    tiempo += aux;
    lcd.setCursor(0, 1);
    lcd.print(tiempo);
  }
  delay(500);
  mm = tiempo;
  aux = 0;
  tiempo = 0;
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Seconds");
  for (int i = 0; i < 2; i++) {
    aux = teclados();
    tiempo *= 10;
    tiempo += aux;
    if (tiempo > 59)
      tiempo = 59;
    lcd.setCursor(0, 1);
    lcd.print(tiempo);

  }
  ss = tiempo;
  tiempo = 0;
  delay(500);
  //if(ss==0)ss++;
  for (ss; ss >= -1; ss--) {



    if (ss == -1)break;
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Timer");
    lcd.setCursor(0, 1);
    if (mm < 10) {
      timer = "0" + (String)mm;
    } else {
      timer = (String)mm;
    }
    timer = timer + ":";
    if (ss < 10) {
      timer = timer + "0" + (String)ss;
    } else {
      timer = timer + (String)ss;
    }
    if (mm != 0 && ss == 0) {
      mm --;
      ss += 60;
    }

    lcd.print(timer);
    delay(1000);


  }

  for (int i = 0; i <= 5; i++) {
    lcd.clear();
    digitalWrite(12, LOW);
    delay(500);
    digitalWrite(12, HIGH);
    lcd.setCursor(0, 0);
    lcd.print("Timer");
    lcd.setCursor(0, 1);
    lcd.print(timer);
    delay(500);
  }
  digitalWrite(12, LOW);
  lcd.clear();
}
