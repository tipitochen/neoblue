#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Bluepad32.h>

// --- PINES MVS P1 ---
#define NEO_UP    14
#define NEO_DOWN  27
#define NEO_LEFT  26
#define NEO_RIGHT 25
#define NEO_A     33
#define NEO_B     32
#define NEO_C      4   
#define NEO_D      5   
#define NEO_START 19
#define NEO_SEL   18
#define LED_PLACA  2 

LiquidCrystal_I2C lcd(0x3F, 20, 4);
const int stickThreshold = 350;
ControllerPtr myController = nullptr;

byte caraFeliz[8] = {
  B00000, B01010, B01010, B00000, B10001, B01110, B00000, B00000
};

void mostrarNeoBlue() {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("====================");
    lcd.setCursor(0, 3);
    lcd.print("====================");
    lcd.setCursor(0, 1);
    lcd.print("|  ");
    lcd.write(0);
    lcd.print("  NEO BLUE  ");
    lcd.write(0);
    lcd.print("  |");
    lcd.setCursor(0, 2);
    lcd.print("|      NIKITA      |");
}

void alertaConexion(String p) {
    for(int i=0; i<4; i++) {
        lcd.clear();
        lcd.backlight();
        lcd.setCursor(0, 0);
        lcd.print("********************");
        int posP = (20 - p.length()) / 2;
        lcd.setCursor(posP, 1);
        lcd.print(p);
        
        lcd.setCursor(5, 2);
        lcd.print("CONECTADO!");
        
        lcd.setCursor(0, 3);
        lcd.print("********************");
        delay(300);
        lcd.noBacklight();
        delay(100);
    }
    lcd.backlight();
    mostrarNeoBlue();
}

void onConnectedController(ControllerPtr ctl) {
    if (myController != nullptr) { ctl->disconnect(); return; }
    myController = ctl;
    ctl->setRumble(0xc0, 0x40);
    alertaConexion("PLAYER 1");
}

void onDisconnectedController(ControllerPtr ctl) {
    if (myController == ctl) {
        myController = nullptr;
        int p[] = {14, 27, 26, 25, 33, 32, 4, 5, 19, 18};
        for(int i=0; i<10; i++) digitalWrite(p[i], HIGH);
        digitalWrite(LED_PLACA, LOW);
        mostrarNeoBlue(); 
    }
}

void setup() {
    Serial.begin(115200);
    // RX=16 (donde sueldas el cable), TX=17
    Serial2.begin(115200, SERIAL_8N1, 16, 17);

    lcd.init();
    lcd.backlight();
    lcd.createChar(0, caraFeliz);
    mostrarNeoBlue();

    int pines[] = {14, 27, 26, 25, 33, 32, 4, 5, 19, 18, LED_PLACA};
    for(int i=0; i<11; i++) {
        pinMode(pines[i], OUTPUT);
        if(pines[i] != LED_PLACA) digitalWrite(pines[i], HIGH);
    }
    BP32.setup(&onConnectedController, &onDisconnectedController);
}

void loop() {
    BP32.update();
    if (myController && myController->isConnected()) {
        uint16_t b = myController->buttons();
        uint8_t d = myController->dpad();
        uint8_t m = myController->miscButtons();
        int32_t x = myController->axisX();
        int32_t y = myController->axisY();
        digitalWrite(NEO_UP,    !((d & 0x01) || (y < -stickThreshold)));
        digitalWrite(NEO_DOWN,  !((d & 0x02) || (y > stickThreshold)));
        digitalWrite(NEO_LEFT,  !((d & 0x08) || (x < -stickThreshold)));
        digitalWrite(NEO_RIGHT, !((d & 0x04) || (x > stickThreshold)));
        digitalWrite(NEO_A,     !(b & 0x0001));
        digitalWrite(NEO_B,     !(b & 0x0002));
        digitalWrite(NEO_C,     !(b & 0x0004));
        digitalWrite(NEO_D,     !(b & 0x0008));
        digitalWrite(NEO_START, !(m & 0x04));
        digitalWrite(NEO_SEL,   !(m & 0x02));
        digitalWrite(LED_PLACA, HIGH);
    } else {
        digitalWrite(LED_PLACA, LOW);
    }

    if (Serial2.available() > 0) {
        if (Serial2.read() == 'C') {
            alertaConexion("PLAYER 2");
        }
    }
}