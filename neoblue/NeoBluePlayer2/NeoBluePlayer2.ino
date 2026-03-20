#include <Bluepad32.h>

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

const int listaPines[] = {NEO_UP, NEO_DOWN, NEO_LEFT, NEO_RIGHT, NEO_A, NEO_B, NEO_C, NEO_D, NEO_START, NEO_SEL};
const int numPines = 10;
const int stickThreshold = 350; 
ControllerPtr myController = nullptr;

void resetPines() {
    for(int i=0; i < numPines; i++) digitalWrite(listaPines[i], HIGH);
    digitalWrite(LED_PLACA, LOW);
}

void onConnectedController(ControllerPtr ctl) {
    if (myController != nullptr) { ctl->disconnect(); return; }
    myController = ctl;
    ctl->setRumble(0xc0, 0x40);
    
    // Envía señal por Pin 17
    Serial2.write('C');
}

void onDisconnectedController(ControllerPtr ctl) {
    if (myController == ctl) {
        myController = nullptr;
        Serial2.write('D');
        resetPines();
    }
}

void setup() {
    Serial.begin(115200);
    // TX=17 (donde sueldas el cable de salida)
    Serial2.begin(115200, SERIAL_8N1, 16, 17);
    
    for(int i=0; i < numPines; i++) pinMode(listaPines[i], OUTPUT);
    pinMode(LED_PLACA, OUTPUT);
    resetPines();
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
    delay(1);
}