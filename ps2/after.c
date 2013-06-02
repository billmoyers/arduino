void setup() {
	pinMode(btn1Pin, INPUT);
	pinMode(btn2Pin, INPUT);
	pinMode(clkPin, OUTPUT);
	pinMode(datPin, OUTPUT);
	Serial.begin(115200);
}

void setClk() {
	digitalWrite(clkPin, !state.Keyboard.clk);
}

void setDat() {
	if (state.Keyboard.datSignalOffset < state.Keyboard.datSignalLength) {
		Serial.print("setDat: ");
		Serial.print(state.Keyboard.datSignalOffset);
		Serial.print("/");
		Serial.println(state.Keyboard.datSignalLength);
		digitalWrite(datPin, !state.Keyboard.dat);
	} else {
		state.Keyboard.dat = 1;
		state.Keyboard.datSignalLength = 0;
		digitalWrite(datPin, !state.Keyboard.dat);
	}
}

void pollInput() {
	Serial.println("pollInput: begin");
	int v1 = digitalRead(btn1Pin);
	int v2 = digitalRead(btn2Pin);
	
	if (v1 == LOW || v2 == LOW) {
		Serial.println("pollInput: got something");
		int numScanCodes = 1;
		state.Keyboard.scanCodes[0] = 0xFD;
		int k = 0;
		for (int i = 0; i < numScanCodes; i++) {
			//Start bit.
			state.Keyboard.datSignal[k++] = 0;
			//Payload bits.
			int parityBit = 1;
			for (int j = 0; j < 8; j++) {
				int b = (state.Keyboard.scanCodes[i] & (1 << j)) != 0;
				if (b) {
					parityBit = !parityBit;
				}
				state.Keyboard.datSignal[k++] = b;
			}
			//Parity bit.
			state.Keyboard.datSignal[k++] = parityBit;
			//Stop bit.
			state.Keyboard.datSignal[k++] = 1;
		}

		//Set to begin transmission.
		state.Keyboard.datSignalOffset = 0;
		state.Keyboard.datSignalLength = k;
	}
}

void loop() {
	Keyboard();
}

