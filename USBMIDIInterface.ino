/* Create a "class compliant " USB to 3 MIDI IN and 3 MIDI OUT interface.

   MIDI receive (6N138 optocoupler) input circuit and series resistor
   outputs need to be connected to Serial1, Serial2 and Serial3.

   You must select MIDIx4 from the "Tools > USB Type" menu

   This example code is in the public domain.
*/

#include <MIDI.h>

#define LED 13 // led pin
#define LED_DURATION_MILLISECONDS 15
#define SINGLE_THRU 19
#define ALL_THRU 20

// Create the Serial MIDI ports
MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDI1);
MIDI_CREATE_INSTANCE(HardwareSerial, Serial2, MIDI2);
MIDI_CREATE_INSTANCE(HardwareSerial, Serial3, MIDI3);

// A variable to know how long the LED has been turned on
elapsedMillis ledOnMillis;


void setup() {
  Serial.begin(115200);
  pinMode(SINGLE_THRU, INPUT_PULLUP);
  pinMode(ALL_THRU, INPUT_PULLUP);
  pinMode(LED, OUTPUT);
  digitalWrite(LED, LOW);
  MIDI1.begin(MIDI_CHANNEL_OMNI);
  MIDI2.begin(MIDI_CHANNEL_OMNI);
  MIDI3.begin(MIDI_CHANNEL_OMNI);
}


void loop() {
  bool activity = false;
  bool singleThru = !digitalRead(SINGLE_THRU);
  bool allThru = !digitalRead(ALL_THRU);

  if (MIDI1.read()) {
    // get a MIDI IN1 (Serial) message
    byte type = MIDI1.getType();
    byte channel = MIDI1.getChannel();
    byte data1 = MIDI1.getData1();
    byte data2 = MIDI1.getData2();

    // forward the message to USB MIDI virtual cable #0
    if (type != midi::SystemExclusive) {
      // Normal messages, simply give the data to the usbMIDI.send()
      usbMIDI.send(type, data1, data2, channel, 0);
    } else {
      // SysEx messages are special.  The message length is given in data1 & data2
      unsigned int SysExLength = data1 + data2 * 256;
      usbMIDI.sendSysEx(SysExLength, MIDI1.getSysExArray(), true, 0);
    }
    activity = true;
  }

  if (MIDI2.read()) {
    // get a MIDI IN2 (Serial) message
    byte type = MIDI2.getType();
    byte channel = MIDI2.getChannel();
    byte data1 = MIDI2.getData1();
    byte data2 = MIDI2.getData2();

    // forward the message to USB MIDI virtual cable #1
    if (type != midi::SystemExclusive) {
      // Normal messages, simply give the data to the usbMIDI.send()
      usbMIDI.send(type, data1, data2, channel, 1);
    } else {
      // SysEx messages are special.  The message length is given in data1 & data2
      unsigned int SysExLength = data1 + data2 * 256;
      usbMIDI.sendSysEx(SysExLength, MIDI1.getSysExArray(), true, 1);
    }
    activity = true;
  }

  if (MIDI3.read()) {
    // get a MIDI IN1 (Serial) message
    byte type = MIDI3.getType();
    byte channel = MIDI3.getChannel();
    byte data1 = MIDI3.getData1();
    byte data2 = MIDI3.getData2();

    // forward the message to USB MIDI virtual cable #0
    if (type != midi::SystemExclusive) {
      // Normal messages, simply give the data to the usbMIDI.send()
      usbMIDI.send(type, data1, data2, channel, 2);
    } else {
      // SysEx messages are special.  The message length is given in data1 & data2
      unsigned int SysExLength = data1 + data2 * 256;
      usbMIDI.sendSysEx(SysExLength, MIDI1.getSysExArray(), true, 2);
    }
    activity = true;
  }

  if (usbMIDI.read()) {
    // get the USB MIDI message, defined by these 5 numbers (except SysEX)
    byte type = usbMIDI.getType();
    byte channel = usbMIDI.getChannel();
    byte data1 = usbMIDI.getData1();
    byte data2 = usbMIDI.getData2();
    byte cable = usbMIDI.getCable();

    // forward this message to 1 of the 3 Serial MIDI OUT ports
    if (type != usbMIDI.SystemExclusive) {
      // Normal messages, first we must convert usbMIDI's type (an ordinary
      // byte) to the MIDI library's special MidiType.
      midi::MidiType mtype = (midi::MidiType)type;

      // Then simply give the data to the MIDI library send()
      switch (cable) {
        case 0:
          MIDI1.send(mtype, data1, data2, channel);
          break;
        case 1:
          MIDI2.send(mtype, data1, data2, channel);
          break;
        case 2:
          MIDI3.send(mtype, data1, data2, channel);
          break;
        case 3:
          MIDI1.send(mtype, data1, data2, channel);
          MIDI2.send(mtype, data1, data2, channel);
          MIDI3.send(mtype, data1, data2, channel);
          break;
      }

    } else {
      // SysEx messages are special.  The message length is given in data1 & data2
      unsigned int SysExLength = data1 + data2 * 256;
      switch (cable) {
        case 0:
          MIDI1.sendSysEx(SysExLength, usbMIDI.getSysExArray(), true);
          break;
        case 1:
          MIDI2.sendSysEx(SysExLength, usbMIDI.getSysExArray(), true);
          break;
        case 2:
          MIDI3.sendSysEx(SysExLength, usbMIDI.getSysExArray(), true);
          break;
        case 3:
          MIDI1.sendSysEx(SysExLength, usbMIDI.getSysExArray(), true);
          MIDI2.sendSysEx(SysExLength, usbMIDI.getSysExArray(), true);
          MIDI3.sendSysEx(SysExLength, usbMIDI.getSysExArray(), true);
          break;
      }
    }
    activity = true;
  }

  // blink the LED when any activity has happened
  if (activity) {
    digitalWriteFast(LED, HIGH); // LED on
    ledOnMillis = 0;
  }
  if (ledOnMillis > LED_DURATION_MILLISECONDS) {
    digitalWriteFast(LED, LOW);  // LED off
  }

}
