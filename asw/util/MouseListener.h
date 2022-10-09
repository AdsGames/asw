/**
 * Mouse Listener
 * Allan Legemaate
 * Listens for mouse buttons JUST pressed or JUST released
 *   since the last tick
 * 18/01/2017
 **/

#ifndef MouseListener_H
#define MouseListener_H

#define MAX_MOUSE_BUTTONS 4

class MouseListener {
 public:
  static void update();

  static unsigned int x;
  static unsigned int y;
  static unsigned char mouse_button;
  static unsigned char mouse_pressed;
  static unsigned char mouse_released;
  static int mouse_z_change;

 private:
  static unsigned char mouse_old;
  static int mouse_z_old;
};

#endif  // MouseListener_H
