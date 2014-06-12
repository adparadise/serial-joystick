int buttonIndex;
const int buttonCount = 16;
int buttonPins[16] = {3,4,5,6,
                      7,8,9,10,
                      11,12,A0,A1,
                      A2,A3,A4,A5};
const int fieldCount = 4;
const int bitsPerField = 6;
uint8_t bitFields[4] = { 0x00, 0x40, 0x80, 0xc0 };

void setup() {
  for (buttonIndex = 0; buttonIndex < buttonCount; buttonIndex++) {
    pinMode(buttonPins[buttonIndex], INPUT);
  }
  Serial.begin(9600);
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
      bitFields[fieldIndex] = bitFields[fieldIndex] | buttonMask;
      isChanged = 1;
    } else if (buttonState != HIGH && isHigh) {
      bitFields[fieldIndex] = bitFields[fieldIndex] & ~buttonMask;
      isChanged = 1;
    }
  }

  if (isChanged) {
    Serial.write(bitFields, fieldCount);
    delay(6);
  }
}

