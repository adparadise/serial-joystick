
#undef DEBUG

#define KEY_CODE_LEFT_ALT	0x04
#define KEY_CODE_LEFT_CTRL	0x01

#define KEY_CODE_RIGHT_ARROW	0x4f
#define KEY_CODE_LEFT_ARROW	0x50
#define KEY_CODE_DOWN_ARROW	0x51
#define KEY_CODE_UP_ARROW	0x52
#define KEY_CODE_R		0x15
#define KEY_CODE_F		0x09
#define KEY_CODE_D		0x07
#define KEY_CODE_G		0x0A
#define KEY_CODE_A		0x04
#define KEY_CODE_S		0x16


typedef struct keyReport_t {
  uint8_t modifiers;
  uint8_t reserved;
  uint8_t keycodes[6]; // 8 buttons per byte
} keyReport_t;

keyReport_t keyReport;
uint8_t nextKeycodeIndex;

const int buttonCount = 12;
int buttonPins[12] = {3,4,5,6,7,8,
                      A0,A1,A2,A3,A4,A5};
// Mapping of pins to keycodes
uint8_t buttonKeyCodes[12] = {
  KEY_CODE_LEFT_ARROW, KEY_CODE_DOWN_ARROW, KEY_CODE_RIGHT_ARROW, KEY_CODE_UP_ARROW,
  KEY_CODE_LEFT_CTRL, KEY_CODE_LEFT_ALT,
  KEY_CODE_G, KEY_CODE_D, KEY_CODE_R, KEY_CODE_F,
  KEY_CODE_A, KEY_CODE_S
};
#define BUTTON_INDEX_CTRL	5
#define BUTTON_INDEX_ALT	4

// Memory to hold the read states of the buttons, allowing us to track
// changes.
const int fieldCount = 4;
const int bitsPerField = 6;
uint8_t bitFields[4] = { 0x00, 0x40, 0x80, 0xc0 };


void setup(void);
void loop(void);

void clearKeyReport(keyReport_t *report);
void setButton(keyReport_t *report, uint8_t button);
void sendKeyReport(keyReport_t *report);


// Reset our Key Report to allow for a new state.
void clearKeyReport(keyReport_t *report) {
  uint8_t index;

  keyReport.modifiers = 0;
  keyReport.reserved = 0;
  for (index = 0; index < 6; index++) {
    keyReport.keycodes[index] = 0;
  }
  nextKeycodeIndex = 0;
}

// Send an HID report to the USB interface
void sendKeyReport(struct keyReport_t *report)
{
#ifndef DEBUG
    uint8_t ledStatus;
    Serial.write((uint8_t *)report, sizeof(keyReport_t));
    ledStatus = Serial.read();
#else
    Serial.print(report->modifiers, HEX);
    Serial.print("  ");
    Serial.print(report->keycodes[0], HEX);
    Serial.print(" ");
    Serial.print(report->keycodes[1], HEX);
    Serial.print(" ");
    Serial.print(report->keycodes[2], HEX);
    Serial.print(" ");
    Serial.print(report->keycodes[3], HEX);
    Serial.print(" ");
    Serial.print(report->keycodes[4], HEX);
    Serial.print(" ");
    Serial.print(report->keycodes[5], HEX);
    Serial.println("");
#endif
}

// Reflect the changed button state in the key report.
void setButton(keyReport_t *report, uint8_t button)
{
  uint8_t keycode;

  if (button == BUTTON_INDEX_CTRL) {
    report->modifiers |= KEY_CODE_LEFT_CTRL;
  } else if (button == BUTTON_INDEX_ALT) {
    report->modifiers |= KEY_CODE_LEFT_ALT;
  } else {
    if (nextKeycodeIndex < 6) {
      keycode = buttonKeyCodes[button];
      report->keycodes[nextKeycodeIndex] = keycode;
      nextKeycodeIndex += 1;
    } else {
      // set this report to "phantom" mode.
    }
  }
}


// Translate our in-memory representation of our buttons into a
// KeyReport.
void buildKeyReport (keyReport_t *report) {
  uint8_t buttonIndex;
  int isHigh, buttonMask, isChanged, buttonState;
  int fieldIndex = 0;

  clearKeyReport(report);

  isChanged = 0;
  for (buttonIndex = 0; buttonIndex < buttonCount; buttonIndex++) {
    fieldIndex = buttonIndex / bitsPerField;
    buttonMask = 1 << (buttonIndex % bitsPerField);
    buttonState = digitalRead(buttonPins[buttonIndex]);
    isHigh = bitFields[fieldIndex] & buttonMask;

    if (!isHigh) {
      setButton(report, buttonIndex);
    }
  }
}

void setup() {
  uint8_t index, fieldIndex, buttonMask;

  Serial.begin(9600);
  delay(200);

  clearKeyReport(&keyReport);

  for (index = 0; index < buttonCount; index++) {
    pinMode(buttonPins[index], INPUT);

    fieldIndex = index / bitsPerField;
    buttonMask = 1 << (index % bitsPerField);
    bitFields[fieldIndex] = bitFields[fieldIndex] & ~buttonMask;
  }
}

void loop() {
  uint8_t buttonIndex;
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
      isChanged = 1;
    } else if (buttonState != HIGH && isHigh) {
      // switch was released
      bitFields[fieldIndex] = bitFields[fieldIndex] & ~buttonMask;
      isChanged = 1;
    }
  }


  if (isChanged) {
    buildKeyReport(&keyReport);
    sendKeyReport(&keyReport);
    delay(6);
  }
}



