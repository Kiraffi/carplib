#ifndef CARPMOUSE_HH
#define CARPMOUSE_HH

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

CARP_FN void carp_mouse_resetState(void);
CARP_FN void carp_mouse_setButtonState(CarpMouseButton button, bool down);
CARP_FN void carp_mouse_setPosition(int x, int y);
CARP_FN void carp_mouse_addWheelMovement(int x, int y);

CARP_FN int carp_mouse_getPositionX(void);
CARP_FN int carp_mouse_getPositionY(void);
CARP_FN int carp_mouse_getWheelX(void);
CARP_FN int carp_mouse_getWheelY(void);

CARP_FN bool carp_mouse_wasButtonPressed(CarpMouseButton button);
CARP_FN bool carp_mouse_wasButtonReleased(CarpMouseButton button);
CARP_FN bool carp_mouse_isButtonDown(CarpMouseButton button);


#endif // CARPMOUSE_HH
