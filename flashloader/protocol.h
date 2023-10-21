#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <ez80.h>
#include "defines.h"

void sendStatus(char state, UINT8 status, UINT32 result);
int putch(int c);
#endif