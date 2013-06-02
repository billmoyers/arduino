void setup() {
	//Setup pins.
	pinMode(btn1Pin, INPUT);
	pinMode(btn2Pin, INPUT);
	pinMode(clkPin, OUTPUT);
	pinMode(datPin, OUTPUT);

	//Setup serial debugging.
	Serial.begin(115200);
}

void setClk() {
	digitalWrite(clkPin, !state.Keyboard.clk);
}

int32_t start = 0;
int32_t end = 0;
int len = 0;

void setDat() {
	if (state.Keyboard.datSignalOffset == 0) {
		start = micros();
	}

	if (state.Keyboard.datSignalOffset < state.Keyboard.datSignalLength) {
		/*Serial.print("setDat: ");
		Serial.print(state.Keyboard.dat);
		Serial.print(" - ");
		Serial.print(state.Keyboard.datSignalOffset);
		Serial.print("/");
		Serial.println(state.Keyboard.datSignalLength);*/
		digitalWrite(datPin, !state.Keyboard.dat);
	} else {
		len = state.Keyboard.datSignalLength;

		state.Keyboard.dat = 1;
		state.Keyboard.datSignalLength = 0;
		digitalWrite(datPin, !state.Keyboard.dat);

		end = micros();

		float durationS = (end - start) / ((float)len*10e6);
		float freq = 1.0 / durationS;

		Serial.print("start=");
		Serial.println(start);
		Serial.print("end=");
		Serial.println(end);

		Serial.print("duration=");
		Serial.print(durationS*10e3);
		Serial.print("ms freq=");
		Serial.print(freq);
		Serial.println("Hz");
	}
}

void pollInput() {
	int v1 = digitalRead(btn1Pin);
	int v2 = digitalRead(btn2Pin);
	
	if (v1 == LOW || v2 == LOW) {
		int numScanCodes;
		if (v1 == LOW) {
			Serial.println("pollInput: press");
			numScanCodes = sizeof(state.Keyboard.pressScanCode) / sizeof(int8_t);
			for (int i = 0; i < numScanCodes; i++) {
				state.Keyboard.scanCodes[i] = state.Keyboard.pressScanCode[i];
			}
		} else {
			Serial.println("pollInput: release");
			numScanCodes = sizeof(state.Keyboard.releaseScanCode) / sizeof(int8_t);
			for (int i = 0; i < numScanCodes; i++) {
				state.Keyboard.scanCodes[i] = state.Keyboard.releaseScanCode[i];
			}
		}

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

