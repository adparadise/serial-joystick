
#undef DEBUG

#define NUM_BUTTONS	40
#define NUM_AXES	8	       // 8 axes, X, Y, Z, etc

typedef struct joyReport_t {
    int16_t axis[NUM_AXES];
    uint8_t button[(NUM_BUTTONS+7)/8]; // 8 buttons per byte
} joyReport_t;

joyReport_t joyReport;

void setup(void);
void loop(void);
void setButton(joyReport_t *joy, uint8_t button);
void clearButton(joyReport_t *joy, uint8_t button);
void sendJoyReport(joyReport_t *report);


// Send an HID report to the USB interface
void sendJoyReport(struct joyReport_t *report)
{
#ifndef DEBUG
    Serial.write((uint8_t *)report, sizeof(joyReport_t));
#else
    // dump human readable output for debugging
    for (uint8_t ind=0; ind<NUM_AXES; ind++) {
	Serial.print("axis[");
	Serial.print(ind);
	Serial.print("]= ");
	Serial.print(report->axis[ind]);
	Serial.print(" ");
    }
    Serial.println();
    for (uint8_t ind=0; ind<NUM_BUTTONS/8; ind++) {
	Serial.print("button[");
	Serial.print(ind);
	Serial.print("]= ");
	Serial.print(report->button[ind], HEX);
	Serial.print(" ");
    }
    Serial.println();
#endif
}

// turn a button on
void setButton(joyReport_t *joy, uint8_t button)
{
    uint8_t index = button/8;
    uint8_t bit = button - 8*index;

    joy->button[index] |= 1 << bit;
}

// turn a button off
void clearButton(joyReport_t *joy, uint8_t button)
{
    uint8_t index = button/8;
    uint8_t bit = button - 8*index;

    joy->button[index] &= ~(1 << bit);
}


int buttonIndex;
const int buttonCount = 16;
int buttonPins[16] = {3,4,5,6,7,8,
                      A0,A1,A2,A3,A4,A5};
const int fieldCount = 4;
const int bitsPerField = 6;
uint8_t bitFields[4] = { 0x00, 0x40, 0x80, 0xc0 };

void setup() {
  Serial.begin(115200);
  delay(200);

  for (uint8_t ind=0; ind<8; ind++) {
    joyReport.axis[ind] = ind*1000;
  }
  for (uint8_t ind=0; ind<sizeof(joyReport.button); ind++) {
    joyReport.button[ind] = 0;
  }

  for (buttonIndex = 0; buttonIndex < buttonCount; buttonIndex++) {
    pinMode(buttonPins[buttonIndex], INPUT);
  }
}

void loop() {
  int isHigh, buttonMask, isChanged, buttonState;
  int fieldIndex = 0;

  isChanged = 0;
  for (buttonIndex = 0; buttonIndex < buttonCount; buttonIndex++) {
    fieldIndex = buttonIndex / bitsPerField;
    buttonMask = 1 << (buttonIndex % bitsPerField);
    buttonState = digitalRead(buttonPins[buttonIndex]);
    isHigh = bitFields[fieldIndex] & buttonMask;
    if (buttonState == HIGH && !isHigh) {
      // switch was engaged
      bitFields[fieldIndex] = bitFields[fieldIndex] | buttonMask;
      setButton(&joyReport, buttonIndex);
      isChanged = 1;
    } else if (buttonState != HIGH && isHigh) {
      // switch was released
      bitFields[fieldIndex] = bitFields[fieldIndex] & ~buttonMask;
      clearButton(&joyReport, buttonIndex);
      isChanged = 1;
    }
  }

  if (isChanged) {
    sendJoyReport(&joyReport);
    delay(6);
  }
}

