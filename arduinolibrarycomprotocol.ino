#ifndef SEND_PROTOCOL_H
#define SEND_PROTOCOL_H

#include <Arduino.h>
#include <stdarg.h>

class send_protocol {
    private:
  byte num_of_trans_pins;
  byte* pins;
  byte clock_in;
  byte clock_out;
  byte message_size;
  byte last_state;
  byte cout_state;
  byte cin_state;
  bool istimedout = false;
  int timeOut = 100;

    public:
    send_protocol(
      byte inclock,
      byte outclock,
      byte msg_size, 
      byte count, ...) {
      clock_in = inclock;
      clock_out = outclock;
      message_size = msg_size;
      num_of_trans_pins = count;
      pins = new byte[count];
      va_list args;
      va_start(args, count);

      for (byte i = 0; i < count; i++) {
             pins[i] = va_arg(args, int);
      }
      va_end(args);
      }

      void begin() {
        pinMode(clock_in, INPUT);
        pinMode(clock_out, OUTPUT);
        digitalWrite(clock_out, LOW);
        cout_state = false;
        last_state = digitalRead(clock_in);
        for (byte i = 0; i < num_of_trans_pins; i++) {
          pinMode(pins[i], OUTPUT);
        }
      }

      void send_byte(uint32_t message, int key) {
        istimedout = false;

        for (byte r = 0; r < (8 + num_of_trans_pins - 1) / num_of_trans_pins; r++) {
          for (byte i = 0; i < num_of_trans_pins; i++) {
            digitalWrite(pins[i], (key >> (r * num_of_trans_pins + i)) & 1);
          }
          if (cout_state == true) {
              digitalWrite(clock_out, LOW);
              cout_state = false;
            } else if (cout_state == false) {
              digitalWrite(clock_out, HIGH);
              cout_state = true;
            }
          unsigned long start = millis();
          while ((cin_state == last_state) && !(istimedout)) {
            cin_state = digitalRead(clock_in);
            if ((millis() - start) >= timeOut) {
              istimedout = true;
            }
          }
          if (istimedout) {
            break;
          }
          last_state = cin_state;
        }
        if (istimedout) {
          return;
        }

        for (byte r = 0; r < (message_size + num_of_trans_pins - 1) / num_of_trans_pins; r++) {
          for (byte i = 0; i < num_of_trans_pins; i++) {
            digitalWrite(pins[i], (message >> (r * num_of_trans_pins + i)) & 1);
          }
          if (cout_state == true) {
              digitalWrite(clock_out, LOW);
              cout_state = false;
            } else if (cout_state == false) {
              digitalWrite(clock_out, HIGH);
              cout_state = true;
            }
          unsigned long start = millis();
          while ((cin_state == last_state) && !(istimedout)) {
            cin_state = digitalRead(clock_in);
            if ((millis() - start) >= timeOut) {
              istimedout = true;
            }
          }
          if (istimedout) {
            break;
          }
          last_state = cin_state;
        }
      }
};

#endif
