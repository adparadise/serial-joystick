const int buttonPin = 5;
const int buttonMask = 1;

int buttonState = 0;
int isHigh = 0;
int isChanged = 0;
int bitField = 46;

void setup() {
  pinMode(buttonPin, INPUT);
  Serial.begin(9600);
}

void loop() {
  buttonState = digitalRead(buttonPin);
  isHigh = bitField & buttonMask;
  if (buttonState == HIGH && !isHigh ) {
    bitField = bitField | buttonMask;
    isChanged = 1;
  } else if (buttonState != HIGH && isHigh) {
    bitField = bitField & ~buttonMask;
    isChanged = 1;
  }

  if (isChanged) {
    Serial.write(bitField);
    isChanged = 0;
    delay(6);
  }
}
