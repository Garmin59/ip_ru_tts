#include "error.h"
#include "global.h"   // for mLED_TOGGLE

void error_handler()
{
  __disable_irq ();
  while (1)
  {
  }
}