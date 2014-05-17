int buttonIndex;
const int buttonCount = 5;
int buttonPins[5] = {3,4,5,6,7};

int bitFields[3] = {128};

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
    buttonMask = 1 << buttonIndex;
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
    Serial.write(bitFields[fieldIndex]);
    delay(6);
  }
}

