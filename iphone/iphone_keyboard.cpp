#include "xerxes.h"

char keys[256];
byte lastpressed;
byte lastkey;
StringRef bindarray[256];

void InitKeyboard() {
	for(int i=0; i < sizeof(keys); i++)
		keys[i] = 0;
}

void UpdateKeyboard() {
}
