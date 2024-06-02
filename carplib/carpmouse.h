#ifndef MYMOUSE_HH
#define MYMOUSE_HH

#include "carptype.h"

typedef enum CarpMouseButton
{
    CarpMouseButton_Invalid = -1,
    CarpMouseButton_Left = 1,
    CarpMouseButton_Middle = 2,
    CarpMouseButton_Right = 3,
    CarpMouseButton_Button4 = 4,
    CarpMouseButton_Button5 = 5,

} CarpMouseButton;

void carp_mouse_resetState();
void carp_mouse_setButtonState(CarpMouseButton button, bool down);
void carp_mouse_setPosition(int x, int y);
void carp_mouse_addWheelMovement(int x, int y);

int carp_mouse_getPositionX();
int carp_mouse_getPositionY();
int carp_mouse_getWheelX();
int carp_mouse_getWheelY();

bool carp_mouse_wasButtonPressed(CarpMouseButton button);
bool carp_mouse_wasButtonReleased(CarpMouseButton button);
bool carp_mouse_isButtonDown(CarpMouseButton button);


#endif // MYMOUSE_HH