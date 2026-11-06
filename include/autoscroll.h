#ifndef TINTA_AUTOSCROLL_H
#define TINTA_AUTOSCROLL_H

#include "app.h"

#include <atomic>

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

// Steps autoscroll once for each refresh of the screen the window is on. A
// 16 ms timer ticks every 15.6 or 31.2 ms instead, out of step with the
// screen: the page moved in uneven jumps, about 40 a second, which on a
// 144 or 240 Hz screen looks choppy. The thread waits for each vertical
// blank and posts WM_APP_AUTOSCROLL_FRAME; the window steps to that blank's
// time, so each frame moves the same distance, and draws at once
struct AutoScrollPacer {
    std::atomic<bool> stop{false};
    std::atomic<bool> pending{false};  // a frame posted and not yet taken
    std::atomic<double> blankMs{0};    // when the last refresh came
};

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
// One step to the time given; false once it has ended
bool autoScrollStep(App& app, HWND hwnd, double ms);
// TIMER_AUTOSCROLL: reads where the pointer is, then steps
bool autoScrollTick(App& app, HWND hwnd, double ms);
// WM_APP_AUTOSCROLL_FRAME, posted at each refresh of the screen: steps to
// the refresh's time and draws
void autoScrollFrame(App& app, HWND hwnd);
// Ends it: a key, the wheel, lost focus or capture, another document
void autoScrollEnd(App& app, HWND hwnd);
// The speed for the pointer this many pixels from the mark along one way,
// in pixels per ms, signed as the offset; 0 within the mark. Up to 80 DIP
// past the mark it rises in step with the distance, then follows a gentle
// curve, which on its own stops at a screen in about a tenth of a second.
// Past the knee, when the `range` pixels
// to scroll are too long to cross quickly at that speed, the curve is
// boosted more the farther out, to the screen's edge, `room` pixels from
// the mark that way, where it crosses the whole range in 0.35 s. An edge
// nearer than 200 DIP past the knee, or none known, counts as that far;
// with no range, the curve alone
float autoScrollSpeed(const App& app, float offset, float room = 0, float range = 0);
// The mark's radius in pixels: within it nothing moves
float autoScrollMarkRadius(const App& app);
// The mark, drawn above the page while active
void renderAutoScrollMark(App& app);

#endif  // TINTA_AUTOSCROLL_H
