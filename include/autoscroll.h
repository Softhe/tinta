#ifndef TINTA_AUTOSCROLL_H
#define TINTA_AUTOSCROLL_H

#include "app.h"

// Middle-click autoscroll, as Windows applications have it. The middle
// button pressed on something that scrolls - the page, the editor's text,
// the preview beside it, a side panel's list, Help - sets a mark there;
// the pointer's distance above or below it (and left or right, on a page
// wider than the window) sets the way and the speed, nothing within the
// mark itself. Released where it was pressed, autoscroll carries on until
// the next click, key or wheel turn; dragged off the mark and released,
// it stops at the release. Esc stops it and does nothing else; another key
// stops it and goes on to do what it does. The tab strip's middle-click
// and a sheet's middle-button pan are unchanged.

bool autoScrollActive(const App& app);
// WM_MBUTTONDOWN at a client point: true when autoscroll starts there
bool autoScrollBegin(App& app, HWND hwnd, POINT pt, double ms);
// WM_MBUTTONUP at a client point: ends a hold that left the mark (by the
// release point too); a click stays on
void autoScrollMiddleUp(App& app, HWND hwnd, POINT pt, double ms);
// WM_MOUSEMOVE while active (the window holds the mouse)
void autoScrollPointer(App& app, HWND hwnd, POINT pt);
// Any button pressed while active ends it; true when the press is taken,
// and its release (the WM_xBUTTONUP named) must do nothing either: the
// window procedure asks autoScrollTakesRelease with each release
bool autoScrollTakesPress(App& app, HWND hwnd, UINT release);
bool autoScrollTakesRelease(App& app, UINT release);
// A key down (WM_KEYDOWN or WM_SYSKEYDOWN) while active: any key but
// Shift, Ctrl, Alt or Windows alone ends it; true when it did
bool autoScrollKey(App& app, HWND hwnd, WPARAM vk);
// TIMER_AUTOSCROLL: one step; false once it has ended
bool autoScrollStep(App& app, HWND hwnd, double ms);
// Ends it: a key, the wheel, lost focus or capture, another document
void autoScrollEnd(App& app, HWND hwnd);
// The speed for the pointer this many pixels from the mark along one way,
// in pixels per ms, signed as the offset; 0 within the mark
float autoScrollSpeed(const App& app, float offset);
// The mark's radius in pixels: within it nothing moves
float autoScrollMarkRadius(const App& app);
// The mark, drawn above the page while active
void renderAutoScrollMark(App& app);

#endif  // TINTA_AUTOSCROLL_H
