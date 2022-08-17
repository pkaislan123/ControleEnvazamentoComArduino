#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <EEPROM.h>

LiquidCrystal_I2C lcd(0x20, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE); //I2C address for PCF8574 in proteus


//pinouts

#define valve_nozzle_1 7
#define valve_nozzle_2 6
#define valve_nozzle_3 5
#define valve_nozzle_4 4


#define nozzle_ctrl_1 A0
#define nozzle_ctrl_2 A1
#define nozzle_ctrl_3 A2
#define nozzle_ctrl_4 A3

#define sensor1 8
#define sensor2 9
#define sensor3 10
#define sensor4 11
#define sensor5 12

#define menuButton 13


unsigned long time_valve_nozzle_1 = millis();
unsigned long time_valve_nozzle_2 = millis();
unsigned long time_valve_nozzle_3 = millis();
unsigned long time_valve_nozzle_4 = millis();


unsigned long delayExecutionTimer = millis();
int delayExecutionTime = 0;




unsigned long time_clear_lcd = millis();



/* variaveis */
const int row_nozzle_1_2 = 1;
const int row_nozzle_3_4 = 3;

boolean startMain = false;
boolean startOne = false;

boolean nozzle1_stopped = true;
boolean nozzle2_stopped = true;
boolean nozzle3_stopped = true;
boolean nozzle4_stopped = true;


boolean nozzle1_start = false;
boolean nozzle2_start = false;
boolean nozzle3_start = false;
boolean nozzle4_start = false;

boolean inMenu = false;
int paramsCounter = 0;
boolean enterMenu = false;

int valueTimeCtrlNozzle1 = 0;
int valueTimeCtrlNozzle2 = 0;
int valueTimeCtrlNozzle3 = 0;
int valueTimeCtrlNozzle4 = 0;
int operationMode = 0;
int executionMode = 0;

byte charInjecting[] = {
  B00100,
  B00100,
  B00100,
  B00100,
  B00000,
  B00100,
  B01110,
  B10101
};

byte charDetected[] = {
  B00100,
  B00100,
  B00100,
  B00100,
  B00000,
  B00100,
  B01110,
  B01110
};

byte charNotDetected[] = {
  B00100,
  B00100,
  B10101,
  B00110,
  B01100,
  B10111,
  B01110,
  B01110
};


byte charEndCurseOpen[] = {
  B00010,
  B00100,
  B01000,
  B10000,
  B11111,
  B11111,
  B11111,
  B00000
};

byte charEndCurseClosed[] = {
  B00000,
  B00000,
  B00111,
  B00011,
  B00101,
  B01000,
  B10000,
  B00000
};


void setup()
{
  lcd.begin(20, 4);
  lcd.backlight();

  lcd.createChar(0, charDetected);
  lcd.createChar(1, charNotDetected);
  lcd.createChar(2, charInjecting);
  lcd.createChar(3, charEndCurseOpen);
  lcd.createChar(4, charEndCurseClosed);

  //pinout mode

  pinMode(valve_nozzle_1, OUTPUT);
  pinMode(valve_nozzle_2, OUTPUT);
  pinMode(valve_nozzle_3, OUTPUT);
  pinMode(valve_nozzle_4, OUTPUT);

  pinMode(sensor1, INPUT);
  pinMode(sensor2, INPUT);
  pinMode(sensor3, INPUT);
  pinMode(sensor4, INPUT);
  pinMode(sensor5, INPUT);

  pinMode(nozzle_ctrl_4, INPUT_PULLUP);

  pinMode(menuButton, INPUT);


  digitalWrite(valve_nozzle_1, HIGH);
  digitalWrite(valve_nozzle_2, HIGH);
  digitalWrite(valve_nozzle_3, HIGH);
  digitalWrite(valve_nozzle_4, HIGH);

  getParams();


}

void getParams() {
  int parte1 = EEPROM.read(0);
  int parte2 = EEPROM.read(1);
  delayExecutionTime = (parte1 * 256) + parte2;

  int parte3 = EEPROM.read(2);
  int parte4 = EEPROM.read(3);
  valueTimeCtrlNozzle1 = (parte3 * 256) + parte4;

  int parte5 = EEPROM.read(4);
  int parte6 = EEPROM.read(5);
  valueTimeCtrlNozzle2 = (parte5 * 256) + parte6;


  int parte7 = EEPROM.read(6);
  int parte8 = EEPROM.read(7);
  valueTimeCtrlNozzle3 = (parte7 * 256) + parte8;


  int parte9 = EEPROM.read(8);
  int parte10 = EEPROM.read(9);
  valueTimeCtrlNozzle4 = (parte9 * 256) + parte10;

  operationMode =  EEPROM.read(10);

  executionMode = EEPROM.read(11);
}

void configDelayExecution() {
  delay(1000);
  int parte1 = EEPROM.read(0);
  int parte2 = EEPROM.read(1);
  int valor_original = (parte1 * 256) + parte2;
  boolean save = false;
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Parametrizacao");
  lcd.setCursor(0, 1);
  lcd.print("Valor Atual:");
  lcd.setCursor(14, 1);
  lcd.print(valor_original);
  lcd.setCursor(0, 2);
  lcd.print("Atrazo de Execucao: ");
  int reader = digitalRead(menuButton);
  int value = 0;
  while ( reader == 0) {

    value = map(analogRead(nozzle_ctrl_1), 0, 1023, 0, 10000);
    value = value / 100;
    value = value * 100;


    lcd.setCursor(0, 3);
    lcd.print("          ");
    lcd.setCursor(0, 3);
    lcd.print(value);

    for (int i = 0; i < 20; i++) {
      reader = digitalRead(menuButton);
      if (reader == 1) {
        save = true;
        break;
      }
    }
    delay(500);
    int selection = map(analogRead(nozzle_ctrl_3), 0, 1023, 0, 8);
    if (selection != 0) {
      break;
      save = false;
    }

  }

  if (save) {
    EEPROM.write(0, value / 256);
    EEPROM.write(1, value % 256);
    lcd.setCursor(0, 1);
    lcd.print("                  ");
    lcd.setCursor(0, 1);
    lcd.print("Valor Atual:");
    lcd.setCursor(14, 1);
    lcd.print(value);
    delay(2000);
  }
}


void configNozzle1Time() {
  delay(1000);
  int parte1 = EEPROM.read(2);
  int parte2 = EEPROM.read(3);
  int valor_original = (parte1 * 256) + parte2;
  boolean save = false;
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Parametrizacao");
  lcd.setCursor(0, 1);
  lcd.print("Valor Atual:");
  lcd.setCursor(14, 1);
  lcd.print(valor_original);
  lcd.setCursor(0, 2);
  lcd.print("Tempo Bico 1:");
  int reader = digitalRead(menuButton);
  int value = 0;
  while ( reader == 0) {

    value = map(analogRead(nozzle_ctrl_1), 0, 1023, 0, 10000);
    value = value / 100;
    value = value * 100;


    lcd.setCursor(0, 3);
    lcd.print("          ");
    lcd.setCursor(0, 3);
    lcd.print(value);

    for (int i = 0; i < 20; i++) {
      reader = digitalRead(menuButton);
      if (reader == 1) {
        break;
      }
    }
    delay(500);
    int selection = map(analogRead(nozzle_ctrl_3), 0, 1023, 0, 8);
    if (selection != 1) {
      break;
      save = false;
    }
  }
  if (save) {
    EEPROM.write(2, value / 256);
    EEPROM.write(3, value % 256);
    lcd.setCursor(0, 1);
    lcd.print("                  ");
    lcd.setCursor(0, 1);
    lcd.print("Valor Atual:");
    lcd.setCursor(14, 1);
    lcd.print(value);
    delay(2000);
  }
}

void configNozzle2Time() {
  delay(1000);
  int parte1 = EEPROM.read(4);
  int parte2 = EEPROM.read(5);
  int valor_original = (parte1 * 256) + parte2;
  boolean save = false;
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Parametrizacao");
  lcd.setCursor(0, 1);
  lcd.print("Valor Atual:");
  lcd.setCursor(14, 1);
  lcd.print(valor_original);
  lcd.setCursor(0, 2);
  lcd.print("Tempo Bico 2:");
  int reader = digitalRead(menuButton);
  int value = 0;
  while ( reader == 0) {

    value = map(analogRead(nozzle_ctrl_1), 0, 1023, 0, 10000);
    value = value / 100;
    value = value * 100;


    lcd.setCursor(0, 3);
    lcd.print("          ");
    lcd.setCursor(0, 3);
    lcd.print(value);

    for (int i = 0; i < 20; i++) {
      reader = digitalRead(menuButton);
      if (reader == 1) {
        save = true;
        break;
      }
    }
    delay(500);
    int selection = map(analogRead(nozzle_ctrl_3), 0, 1023, 0, 8);
    if (selection != 2) {
      break;
      save = false;
    }
  }
  if (save) {
    EEPROM.write(4, value / 256);
    EEPROM.write(5, value % 256);
    lcd.setCursor(0, 1);
    lcd.print("                  ");
    lcd.setCursor(0, 1);
    lcd.print("Valor Atual:");
    lcd.setCursor(14, 1);
    lcd.print(value);
    delay(2000);
  }

}


void configNozzle3Time() {
  delay(1000);
  int parte1 = EEPROM.read(6);
  int parte2 = EEPROM.read(7);
  int valor_original = (parte1 * 256) + parte2;
  boolean save = false;
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Parametrizacao");
  lcd.setCursor(0, 1);
  lcd.print("Valor Atual:");
  lcd.setCursor(14, 1);
  lcd.print(valor_original);
  lcd.setCursor(0, 2);
  lcd.print("Tempo Bico 3:");
  int reader = digitalRead(menuButton);
  int value = 0;
  while ( reader == 0) {

    value = map(analogRead(nozzle_ctrl_1), 0, 1023, 0, 10000);
    value = value / 100;
    value = value * 100;


    lcd.setCursor(0, 3);
    lcd.print("          ");
    lcd.setCursor(0, 3);
    lcd.print(value);

    for (int i = 0; i < 20; i++) {
      reader = digitalRead(menuButton);
      if (reader == 1) {
        break;
      }
    }
    delay(500);
    int selection = map(analogRead(nozzle_ctrl_3), 0, 1023, 0, 8);
    if (selection != 3) {
      break;
      save = false;
    }
  }
  if (save) {
    EEPROM.write(6, value / 256);
    EEPROM.write(7, value % 256);
    lcd.setCursor(0, 1);
    lcd.print("                  ");
    lcd.setCursor(0, 1);
    lcd.print("Valor Atual:");
    lcd.setCursor(14, 1);
    lcd.print(value);
    delay(2000);
  }

}


void configNozzle4Time() {
  delay(1000);
  int parte1 = EEPROM.read(8);
  int parte2 = EEPROM.read(9);
  int valor_original = (parte1 * 256) + parte2;
  boolean save = false;
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Parametrizacao");
  lcd.setCursor(0, 1);
  lcd.print("Valor Atual:");
  lcd.setCursor(14, 1);
  lcd.print(valor_original);
  lcd.setCursor(0, 2);
  lcd.print("Tempo Bico 4:");
  int reader = digitalRead(menuButton);
  int value = 0;
  while ( reader == 0) {

    value = map(analogRead(nozzle_ctrl_1), 0, 1023, 0, 10000);
    value = value / 100;
    value = value * 100;


    lcd.setCursor(0, 3);
    lcd.print("          ");
    lcd.setCursor(0, 3);
    lcd.print(value);

    for (int i = 0; i < 20; i++) {
      reader = digitalRead(menuButton);
      if (reader == 1) {
        break;
      }
    }

    delay(500);
    int selection = map(analogRead(nozzle_ctrl_3), 0, 1023, 0, 8);
    if (selection != 4) {
      break;
      save = false;
    }
  }
  if (save) {
    EEPROM.write(8, value / 256);
    EEPROM.write(9, value % 256);
    lcd.setCursor(0, 1);
    lcd.print("                  ");
    lcd.setCursor(0, 1);
    lcd.print("Valor Atual:");
    lcd.setCursor(14, 1);
    lcd.print(value);
    delay(2000);
  }

}


void configModeOperation() {
  delay(1000);
  int valor_original = EEPROM.read(10);
  boolean save = false;
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Parametrizacao");
  lcd.setCursor(0, 1);
  lcd.print("Valor Atual:");
  lcd.setCursor(14, 1);

  if (valor_original > 170) {
    lcd.print("Fixo");
  } else {
    lcd.print("Ajuste");

  }

  lcd.setCursor(0, 2);
  lcd.print("Modo Operacao:");
  int reader = digitalRead(menuButton);
  int value = 0;
  while ( reader == 0) {

    value = map(analogRead(nozzle_ctrl_1), 0, 1023, 100, 200);

    lcd.setCursor(0, 3);
    lcd.print("          ");
    lcd.setCursor(0, 3);
    if (value > 170) {
      lcd.print("Fixo");
    } else {
      lcd.print("Ajuste");

    }

    for (int i = 0; i < 20; i++) {
      reader = digitalRead(menuButton);
      if (reader == 1) {
        save = true;
        break;
      }
    }

    delay(500);
    int selection = map(analogRead(nozzle_ctrl_3), 0, 1023, 0, 8);
    if (selection != 5) {
      break;
      save = false;
    }
  }
  if (save) {
    EEPROM.write(10, value);
    lcd.setCursor(0, 1);
    lcd.print("                  ");
    lcd.setCursor(0, 1);
    lcd.print("Valor Atual:");
    lcd.setCursor(14, 1);
    if (value > 170) {
      lcd.print("Fixo");
    } else {
      lcd.print("Ajuste");

    }
    delay(2000);
  }

}

void configModeExecution() {
  delay(1000);
  int valor_original = EEPROM.read(11);
  boolean save = false;
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Parametrizacao");
  lcd.setCursor(0, 1);
  lcd.print("Valor Atual:");
  lcd.setCursor(14, 1);

  if (valor_original > 170) {
    lcd.print("Direto");
  } else {
    lcd.print("Atrazo");

  }

  lcd.setCursor(0, 2);
  lcd.print("Modo Execucao:");
  int reader = digitalRead(menuButton);
  int value = 0;
  while ( reader == 0) {

    value = map(analogRead(nozzle_ctrl_1), 0, 1023, 100, 200);

    lcd.setCursor(0, 3);
    lcd.print("          ");
    lcd.setCursor(0, 3);
    if (value > 170) {
      lcd.print("Direto");
    } else {
      lcd.print("Atrazo");

    }

    for (int i = 0; i < 20; i++) {
      reader = digitalRead(menuButton);
      if (reader == 1) {
        save = true;
        break;
      }
    }

    delay(500);
    int selection = map(analogRead(nozzle_ctrl_3), 0, 1023, 0, 8);
    if (selection != 6) {
      break;
      save = false;
    }
  }
  if (save) {
    EEPROM.write(11, value);
    lcd.setCursor(0, 1);
    lcd.print("                  ");
    lcd.setCursor(0, 1);
    lcd.print("Valor Atual:");
    lcd.setCursor(14, 1);
    if (value > 170) {
      lcd.print("Direto");
    } else {
      lcd.print("Atrazo");

    }
    delay(2000);
  }

}

void menuExit() {

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Parametrizacao");
  boolean exitMenu = false;
  lcd.setCursor(0, 2);
  lcd.print("Sair");
  int reader = digitalRead(menuButton);
  while ( reader == 0) {



    delay(500);
    int selection = map(analogRead(nozzle_ctrl_3), 0, 1023, 0, 8);
    if (  selection <  7) {
      break;
    }

    for (int i = 0; i < 20; i++) {
      reader = digitalRead(menuButton);
      if (reader == 1) {
        exitMenu = true;
        break;
      }
    }

  }
  if (exitMenu) {
    getParams();
    inMenu = false;
    delay(1000);

  }

}



void loop() {

  if (inMenu) {

    if (!enterMenu) {
      while (inMenu) {
        int selection = map(analogRead(nozzle_ctrl_3), 0, 1023, 0, 8);

        if (selection == 0) {
          configDelayExecution();

        } else if (selection == 1) {
          configNozzle1Time();

        } else if (selection == 2) {
          configNozzle2Time();

        } else if (selection == 3) {
          configNozzle3Time();

        } else if (selection == 4) {
          configNozzle4Time();

        } else if (selection == 5) {
          configModeOperation();
        }
        else if (selection == 6) {
          configModeExecution();
        }
        else if (selection >= 7) {
          menuExit();

        }
      }
      lcd.clear();

    }

  } else {

    if (!startMain) {
      if (digitalRead(sensor5) == HIGH) {
        startProcess();
      } else {
        //endCurseOpen
        lcd.setCursor(19, 0);
        lcd.write(byte(3));
        if (digitalRead(menuButton) == HIGH) {
          inMenu = true;
        }
      }
    } else {
      //execution
      lcd.setCursor(19, 0);
      lcd.write(byte(4));
    }

    int timeClearLcd = 0;

    if (operationMode > 170) {
      timeClearLcd = 5000;
    } else {
      timeClearLcd = 500;
    }

    if ((millis() - time_clear_lcd) > timeClearLcd) {

      if (operationMode > 170) {

      } else {
        valueTimeCtrlNozzle1 = remapTimeNozzleControl(analogRead(nozzle_ctrl_1), 8000, 100, 0);
        valueTimeCtrlNozzle2 = remapTimeNozzleControl(analogRead(nozzle_ctrl_2), 8000, 100, 0);
        valueTimeCtrlNozzle3 = remapTimeNozzleControl(analogRead(nozzle_ctrl_3), 8000, 100, 0);
        valueTimeCtrlNozzle4 = remapTimeNozzleControl(analogRead(nozzle_ctrl_4), 16000, 100, 1);
      }



      clearLcd(row_nozzle_1_2, 5, 19);

      printInLcd(row_nozzle_1_2, 2, "B1:");
      printInLcd(row_nozzle_1_2, 5, valueTimeCtrlNozzle1 );


      printInLcd(row_nozzle_1_2, 11, "B2:");
      printInLcd(row_nozzle_1_2, 14,  valueTimeCtrlNozzle2);

      clearLcd(row_nozzle_3_4, 5, 19);

      printInLcd(row_nozzle_3_4, 2, "B3:");
      printInLcd(row_nozzle_3_4, 5, valueTimeCtrlNozzle3);


      printInLcd(row_nozzle_3_4, 11, "B4:");
      printInLcd(row_nozzle_3_4, 14, valueTimeCtrlNozzle4 );
      time_clear_lcd = millis();
    }


    if (digitalRead(sensor1)) {
      lcd.setCursor(4, 0);
      lcd.write(byte(0));
    } else {
      lcd.setCursor(4, 0);
      lcd.write(byte(1));
    }


    if (digitalRead(sensor2)) {
      lcd.setCursor(13, 0);
      lcd.write(byte(0));
    } else {
      lcd.setCursor(13, 0);
      lcd.write(byte(1));
    }


    if (digitalRead(sensor3)) {
      lcd.setCursor(4, 2);
      lcd.write(byte(0));
    } else {
      lcd.setCursor(4, 2);
      lcd.write(byte(1));
    }


    if (digitalRead(sensor4)) {
      lcd.setCursor(13, 2);
      lcd.write(byte(0));
    } else {
      lcd.setCursor(13, 2);
      lcd.write(byte(1));
    }

    if (executionMode > 170) {
      //lcd.print("Direto");

      //start
      if (startMain && startOne) {
        if (digitalRead(sensor1) == HIGH && valueTimeCtrlNozzle1 > 100) {
          digitalWrite(valve_nozzle_1, LOW);
          lcd.setCursor(5, 0);
          lcd.write(byte(2));
        } else {
          nozzle1_stopped = true;
        }

        if (digitalRead(sensor2) == HIGH && valueTimeCtrlNozzle2 > 100 ) {
          digitalWrite(valve_nozzle_2, LOW);
          lcd.setCursor(14, 0);
          lcd.write(byte(2));
        } else {
          nozzle2_stopped = true;
        }

        if (digitalRead(sensor3) == HIGH && valueTimeCtrlNozzle3 > 100) {
          digitalWrite(valve_nozzle_3, LOW);
          lcd.setCursor(5, 2);
          lcd.write(byte(2));
        } else {
          nozzle3_stopped = true;
        }

        if (digitalRead(sensor4) == HIGH && valueTimeCtrlNozzle4 > 100) {
          digitalWrite(valve_nozzle_4, LOW);
          lcd.setCursor(14, 2);
          lcd.write(byte(2));
        } else {
          nozzle4_stopped = true;
        }
        startOne = false;
      }
      //stop
      if (startMain && !startOne) {

        if ((millis() - time_valve_nozzle_1) > valueTimeCtrlNozzle1) {
          digitalWrite(valve_nozzle_1, HIGH);
          lcd.setCursor(5, 0);
          lcd.print(" ");
          nozzle1_stopped = true;

        }

        if ((millis() - time_valve_nozzle_2) > valueTimeCtrlNozzle2) {
          digitalWrite(valve_nozzle_2, HIGH);
          lcd.setCursor(14, 0);
          lcd.print(" ");
          nozzle2_stopped = true;

        }

        if ((millis() - time_valve_nozzle_3) > valueTimeCtrlNozzle3) {
          digitalWrite(valve_nozzle_3, HIGH);
          lcd.setCursor(5, 2);
          lcd.print(" ");
          nozzle3_stopped = true;

        }

        if ((millis() - time_valve_nozzle_4) > valueTimeCtrlNozzle4) {
          digitalWrite(valve_nozzle_4, HIGH);
          lcd.setCursor(14, 2);
          lcd.print(" ");
          nozzle4_stopped = true;

        }



        if (nozzle1_stopped && nozzle2_stopped && nozzle3_stopped && nozzle4_stopped) {
          startMain = false;
        }

      }
    } else {
      // lcd.print("Atrazo");

      //start
      if (startMain && startOne) {

        if ((millis() - time_valve_nozzle_1) > valueTimeCtrlNozzle1 && !nozzle1_start) {
          if (digitalRead(sensor1) == HIGH) {
            digitalWrite(valve_nozzle_1, LOW);
            lcd.setCursor(5, 0);
            lcd.write(byte(2));
            nozzle1_start = true;
          }
        }

        if ((millis() - time_valve_nozzle_2) > valueTimeCtrlNozzle2 && !nozzle2_start) {
          if (digitalRead(sensor2) == HIGH) {
            digitalWrite(valve_nozzle_2, LOW);
            lcd.setCursor(14, 0);
            lcd.write(byte(2));
            nozzle2_start = true;
          }
        }

        if ((millis() - time_valve_nozzle_3) > valueTimeCtrlNozzle3 && !nozzle3_start) {
          if (digitalRead(sensor3) == HIGH) {
            digitalWrite(valve_nozzle_3, LOW);
            lcd.setCursor(5, 2);
            lcd.write(byte(2));
            nozzle3_start = true;
          }
        }

        if ((millis() - time_valve_nozzle_4) > valueTimeCtrlNozzle4 && !nozzle4_start) {
          if (digitalRead(sensor4) == HIGH) {
            digitalWrite(valve_nozzle_4, LOW);
            lcd.setCursor(14, 2);
            lcd.write(byte(2));
            nozzle4_start = true;
          }

        }



        if (nozzle1_start && nozzle2_start && nozzle3_start && nozzle4_start) {
          startOne = false;
        }

      }

      //stop
      for (int i = 0; i < 20; i++) {
        if (digitalRead(sensor5) == LOW && startMain && !startOne  ) {

          digitalWrite(valve_nozzle_1, HIGH);
          lcd.setCursor(5, 0);
          lcd.print(" ");
          nozzle1_start = false;

          digitalWrite(valve_nozzle_2, HIGH);
          lcd.setCursor(14, 0);
          lcd.print(" ");
          nozzle2_start = false;

          digitalWrite(valve_nozzle_3, HIGH);
          lcd.setCursor(5, 2);
          lcd.print(" ");
          nozzle3_start = false;

          digitalWrite(valve_nozzle_4, HIGH);
          lcd.setCursor(14, 2);
          lcd.print(" ");
          nozzle4_start = false;

          startMain = false;
        }
      }

    }





  }
}




int remapTimeNozzleControl(int value, int maxi, int offset, int format) {
  int sample = 0;
  if (format == 0) {
    sample = map(value, 0, 1023, 0, maxi);
  } else {
    sample = map(value, 0, 1023, maxi, 0);
  }
  sample = sample / 100;
  sample = sample * 100;
  sample = sample - offset;
  return sample;
}



void clearLcd(int row, int columnStar, int columnEnd) {
  for (int column = columnStar; column <= columnEnd; column++) {
    lcd.setCursor(column, row);
    lcd.print(" ");
  }
}

void printInLcd(int row, int column, String msg) {
  lcd.setCursor(column, row);
  lcd.print(msg);
}

void printInLcd(int row, int column, int msg) {
  lcd.setCursor(column, row);
  lcd.print(msg);
}


void startProcess() {

  if (!startMain) {

    startMain = true;
    startOne = true;

    nozzle1_stopped = false;
    nozzle2_stopped = false;
    nozzle3_stopped = false;
    nozzle4_stopped = false;


    delay(delayExecutionTime);
    time_valve_nozzle_1 = millis();
    time_valve_nozzle_2 = millis();
    time_valve_nozzle_3 = millis();
    time_valve_nozzle_4 = millis();
    delay(100);

  }
}
