# Middle-click autoscroll

**Not yet validated on Windows.** The change was written on a Linux machine
with no Windows toolchain. Nothing below has been compiled or run yet: the
build, `middle_click_autoscroll` and the manual checks are still to do.

## Request

On Windows, pressing the middle button in an application that supports it
starts autoscroll: moving the pointer above or below where it was pressed
sets the direction and speed. Tinta's middle button only closed tabs and
panned sheets.

## Change

`src/autoscroll.cpp` (new) drives the same scroll targets as touch panning
(`touchTargetAt`, now with `touchScrollPosition`, `touchScrollRange` and
`touchScrollTo` exposed from `src/touch.cpp`):

- **Where**: the middle button pressed on the page, the editor's text, the
  preview beside it, the Contents, file browser or search results list, or
  Help. It does not start on the tab strip (where it still closes a tab),
  on a sheet (which still pans with the middle button held), on scrollbars,
  menus, dialogs or overlays, during a selection drag, or on a page with
  nothing to scroll.
- **The mark**: a round mark at the press, drawn above everything, with a
  chevron for each way it can go and the way it is going in the theme's
  accent. The cursor is the up-down arrow, or the four-way arrow when the
  page is wider than the window, in which case it scrolls across too.
- **Speed**: nothing within 14 DIP of the mark. Past that, speed in DIP per
  ms = 0.0032 x (DIP past the mark)^1.3, capped at 8: about 500 DIP/s at 50
  DIP past the mark and 5000 DIP/s at 300. The timer ticks every 16 ms, and a
  late tick moves at most 50 ms' worth. At an end it waits for the pointer to
  turn back.
- **Click or hold**: released without leaving the mark, it stays on (the
  window keeps the mouse, and each tick reads the cursor so the pointer
  steers outside the window too). Dragged off the mark with the button held,
  it stops at the release.
- **Stopping**: any button press (that press and its own release do nothing
  else, so a right click opens no menu), the wheel, a change of tab, file or
  mode, closing Help, lost focus or capture. Esc stops it and does nothing
  else. Another key stops it and then acts (Home goes to the top). Shift,
  Ctrl, Alt and Windows pressed alone leave it going.
- Touch panning on a Live page now reaches the end of the page as the wheel
  does (`livePageHeight`), since both use the same range.

## Fixture

`tests/fixtures/autoscroll-middle-click.md`: 41 sections with headings,
wrapped paragraphs with emphasis, bold, inline code and links, nested
lists, quotes every fifth section, a 14-column table and a long Python line
for sideways scrolling at narrow widths, and a jump link to the end.

## Automated checks

`middle_click_autoscroll` (new, `tests/autoscroll_tests.cpp`, isolated
portable run). In themes 0 and 5, at 1050 and 650 px, at 1x and 1.5x:

- the speed curve: zero within the mark, rising with distance, the same up
  and down, capped, about 0.5 DIP/ms at 50 DIP past the mark, at any scale;
- no start on the tab strip, the scrollbar, an open overlay, during a
  selection drag, or (at 1050 px, 1x) on a page that fits the window;
- a click: still on after the release; the distance run below the mark
  matches the speed; above it runs back; on the mark it waits; a late tick
  moves at most 50 ms; a click stops it, and only that button's release is
  taken; a lost release is not waited for past the next press;
- at the page's end it stays on and runs back up when the pointer turns;
  Esc stops it without scrolling, Shift alone leaves it going, Home stops
  it and goes to the top;
- a hold dragged off the mark stops at the release; one that stays on the
  mark acts as a click;
- across a page wider than the window (at 650 px), only across;
- the mark paints; entering the editor stops it; the editor's text, and the
  preview (taking the editor with it), the Contents list and Help run on
  their own; closing Help stops it.

The window procedure's routing (wheel, button messages, focus and capture
loss, the cursor read on each tick) is not covered by the harness, which
calls the handlers directly.

## Manual checks still to do

In the built app, with the fixture, at normal and narrow widths, in a light
and a dark theme:

1. Middle-click mid-page; move below, above, far below; check the mark,
   the cursor and the speed; stop with a click, a right click (no menu),
   the wheel, Esc and Home.
2. Hold the middle button, drag down, release: it stops there.
3. Move the pointer outside the window while it runs after a click.
4. Narrow the window until the table scrolls across; check the four-way
   mark and the sideways scroll.
5. In Source with the preview, on the editor and on the preview; in Live;
   on the Contents (Tab), the file browser (B) and Help (F1).
6. Middle-click a tab (it closes) and a sheet (it pans), as before.
7. Alt+Tab away while it runs: it stops.
