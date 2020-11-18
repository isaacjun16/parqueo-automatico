#include <Servo.h>
#include <SPI.h>
#include <MFRC522.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

#define RST_PIN  41
#define SS_PIN 53

MFRC522 mfrc522(SS_PIN, RST_PIN);
byte ActualUID[4];

byte usuario1[4] = {0xD7, 0x89, 0xAF, 0x19};
byte usuario2[4] = {0x69, 0x1E, 0xEB, 0x8C};
byte usuario3[4] = {0xE9, 0x2F, 0xC1, 0x8B};
byte usuario4[4] = {0xB9, 0x14, 0x5A, 0x8D};

Servo servoMotor;

int pinPulseServo = 2;
int pos = 0;

int pinOutInfrarojo = 3;


int pinOutCNY1 = 6;
int pinOutCNY2 = 7;

int pinTrigUSonico = 9;
int pinEchoUSonico = 10;

int pinJoystickZ = 4;
int pinJoystickX = 0;
int pinJoystickY = 1;

bool parqueo[4] = {true, true, true, true};
const int totalParqueos = 4;

void setup() {
  Serial.begin(9600);
  
  servoMotor.attach(pinPulseServo);

  pinMode(pinOutInfrarojo, INPUT);
  
  pinMode(pinOutCNY1, INPUT);
  pinMode(pinOutCNY2, INPUT);
  
  pinMode(pinEchoUSonico, INPUT); 
  pinMode(pinTrigUSonico, OUTPUT);    

  pinMode(pinJoystickZ, INPUT);
  digitalWrite(pinJoystickZ, HIGH);
  
  servoMotor.write(90);

  SPI.begin();
  mfrc522.PCD_Init();

  lcd.init();
  lcd.backlight();
}

void loop() {
  
  //infrarojo sensor parqueo 4          ##########################################################################################################
  int valInfrarojo = 0;
  valInfrarojo = digitalRead(pinOutInfrarojo);

  if(valInfrarojo == HIGH) {
    Serial.print("Libre 4: ");
    parqueo[3] = true;
  } else {
    Serial.print("Ocupado 4: ");
    parqueo[3] = false;
  }
  Serial.println(valInfrarojo);

  //CNY70 sensor parqueo 3          ##########################################################################################################
  int valCny1 = 0;
  valCny1 = digitalRead(pinOutCNY1);

  if(valCny1 == LOW) {
    Serial.print("Libre 3: ");
    parqueo[2] = true;
  } else {
    Serial.print("Ocupado 3: ");
    parqueo[2] = false;
  }
  Serial.println(valCny1);
  
  //CNY70 sensor parqueo 2          ##########################################################################################################
  int valCny2 = 0;
  valCny2 = digitalRead(pinOutCNY2);

  if(valCny2 == LOW) {
    Serial.print("Libre 2: ");
    parqueo[1] = true;
  } else {
    Serial.print("Ocupado 2: ");
    parqueo[1] = false;
  }
  Serial.println(valCny2);

  //UltraSonico sensor parqueo 1          ##########################################################################################################
  int cm = 0;
  cm = ping(pinTrigUSonico, pinEchoUSonico);

  if(cm != 20) {
    Serial.print("Ocupado 1: ");
    parqueo[0] = false;
  } else {
    Serial.print("Libre 1: ");
    parqueo[0] = true;
  }
  Serial.print(" distancia > ");
  Serial.println(cm);

  //Joystick          ##########################################################################################################
  int z, x, y;
  z = digitalRead(pinJoystickZ);
  x = analogRead(pinJoystickX);
  y = analogRead(pinJoystickY);

  Serial.print("Boton: ");
  Serial.print(z);

  Serial.print(", Eje X: ");
  Serial.print(x);

  Serial.print(", Eje Y: ");
  Serial.println(y);

  
  //ServoMotor          ##########################################################################################################
  int motorPos = servoMotor.read();
  Serial.print("Posicion motor: ");
  Serial.println(motorPos);

  if(y <= 10 && motorPos == 90) {
    abrirTalanquera();
  } else if(y >= 1000 && motorPos == 180) {
    cerrarTalanquera();
  }

  //RFID           ##########################################################################################################
  leerCartaID();

  mensajeBienvenida();
  Serial.println("####################");
  delay(1000);
}

void mensajeBienvenida() {
  int libres = 0;
  
  Serial.print("*Ocupados: ");
  for(int i = 0; i < totalParqueos; i++) {
    if(parqueo[i] == true) {
      libres++;
    } else {
      Serial.print(i);
      Serial.print(" ");
    }
  }
  Serial.println();

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Bienvenido! Hay ");

  lcd.setCursor(0,1);
  lcd.print(libres);
  lcd.print(" lugares libres");
}

void abrirTalanquera() {
  Serial.println("Abriendo talanquera");
  for (pos = 90; pos <= 180; pos += 1) { 
    servoMotor.write(pos); 
    delay(15); 
  }
}

void cerrarTalanquera() {
  Serial.println("Abriendo talanquera");
  for (pos = 180; pos >= 90; pos -= 1) { 
    servoMotor.write(pos);              
    delay(15);  
  }
}

void registrarIngreso(int noParqueo) {
  //TODO java
}

void registrarEgreso(int noParqueo) {
  //TODO java
}

int ping(int TriggerPin, int echoPin) {
  long duration, distanceCm;
  digitalWrite(TriggerPin, LOW);
  delayMicroseconds(2);
  digitalWrite(TriggerPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(TriggerPin, LOW);
  
  duration = pulseIn(echoPin, HIGH);
  distanceCm = duration * 0.034 / 2;
  
  return distanceCm;
}

void leerCartaID() {  

  if(mfrc522.PICC_IsNewCardPresent()) {
    
    if(mfrc522.PICC_ReadCardSerial()) {
      Serial.print("Card UID:");

      for(byte i = 0; i < mfrc522.uid.size; i++) {
        if(mfrc522.uid.uidByte[i] < 0x10) {
          Serial.print(" 0");
        } else {
          Serial.print(" ");
        }
        
        Serial.print(mfrc522.uid.uidByte[i], HEX);
        ActualUID[i] = mfrc522.uid.uidByte[i];
      }
      Serial.println();

      if(validarTarjetaUsuario(ActualUID, usuario1)) {
        
        asignarParqueo(1);  
      } else if(validarTarjetaUsuario(ActualUID, usuario2)) {
        
        asignarParqueo(2);  
      } else if(validarTarjetaUsuario(ActualUID, usuario3)) {
        
        asignarParqueo(3);  
      } else if(validarTarjetaUsuario(ActualUID, usuario4)) {
        
        asignarParqueo(4);  
      } else {
        
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Tarjeta no ");
        lcd.setCursor(0,1);
        lcd.print("valida");

        Serial.print("---");
        Serial.print(ActualUID[0], HEX);
        Serial.print(" ");
        Serial.print(ActualUID[1], HEX);
        Serial.print(" ");
        Serial.print(ActualUID[2], HEX);
        Serial.print(" ");
        Serial.print(ActualUID[3], HEX);
        Serial.println(",,fallo");
        
        delay(2000);
      }
      

      mfrc522.PICC_HaltA();
      
    } else {
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Tarjeta no reco-");
      lcd.setCursor(0,1);
      lcd.print("nocida o dañada");
    }
  }
  
}

void asignarParqueo(int usuario) {
  int parqueoAsignado = 0;
  for(int i = 0; i < totalParqueos; i++) {
    if(parqueo[i] == true) {
      parqueoAsignado = i + 1;
      break;
    }
  }

  lcd.setCursor(0,0);
  if(parqueoAsignado != 0) {

    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Favor utilizar");
    lcd.setCursor(0,1);
    lcd.print("parqueo: ");
    lcd.print(parqueoAsignado);

    Serial.print("---");
    if(usuario == 1) {
      
      Serial.print(usuario1[0], HEX);
      Serial.print(" ");
      Serial.print(usuario1[1], HEX);
      Serial.print(" ");
      Serial.print(usuario1[2], HEX);
      Serial.print(" ");
      Serial.print(usuario1[3], HEX);
    } else if(usuario == 2) {
      
      Serial.print(usuario2[0], HEX);
      Serial.print(" ");
      Serial.print(usuario2[1], HEX);
      Serial.print(" ");
      Serial.print(usuario2[2], HEX);
      Serial.print(" ");
      Serial.print(usuario2[3], HEX);
    } else if(usuario == 3) {
      
      Serial.print(usuario3[0], HEX);
      Serial.print(" ");
      Serial.print(usuario3[1], HEX);
      Serial.print(" ");
      Serial.print(usuario3[2], HEX);
      Serial.print(" ");
      Serial.print(usuario3[3], HEX);
    } else if(usuario == 4) {
      
      Serial.print(usuario4[0], HEX);
      Serial.print(" ");
      Serial.print(usuario4[1], HEX);
      Serial.print(" ");
      Serial.print(usuario4[2], HEX);
      Serial.print(" ");
      Serial.print(usuario4[3], HEX);
    }
    Serial.print(",");
    Serial.print(parqueoAsignado);
    Serial.println(",exito");

    int motorPos = servoMotor.read();
    if(motorPos == 90) {
      abrirTalanquera();
    } 
    delay(5000);

    motorPos = servoMotor.read();
    if(motorPos == 180) {
      cerrarTalanquera();
    }
  } else {
    
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Lo sentimos");
    lcd.setCursor(0,1);
    lcd.print("no hay parqueos");
    delay(2000);
  }
}

boolean validarTarjetaUsuario(byte usuarioActual[], byte usuario[]) {
  if(usuarioActual[0] != usuario[0]) return(false);
  if(usuarioActual[1] != usuario[1]) return(false);
  if(usuarioActual[2] != usuario[2]) return(false);
  if(usuarioActual[3] != usuario[3]) return(false);

  return true;
}
