# Middle-click autoscroll

Written on a Linux machine, then built and checked on Windows 11 (26200)
with Visual Studio 2022 Build Tools (MSVC, `/W4`) and its bundled CMake. See
[Windows results](#windows-results) at the end.

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
  DIP past the mark and 5000 DIP/s at 300. It steps once for each refresh
  of the screen the window is on (see Frame pacing below), and a late step
  moves at most 50 ms' worth. At an end it waits for the pointer to turn
  back.
- **Frame pacing**: a thread waits for each vertical blank of the window's
  monitor (DXGI `WaitForVBlank`, the output found again if the window moves
  to another screen) and posts `WM_APP_AUTOSCROLL_FRAME`, one at a time.
  The window steps to that blank's time and draws at once (`UpdateWindow`).
  Where no output can be waited on, the thread posts about every 15 ms;
  where the thread cannot start, `TIMER_AUTOSCROLL` (16 ms) steps it as
  before. The thread stops with autoscroll and holds its own reference, so
  ending never waits for it.
- **Click or hold**: released without leaving the mark, it stays on (the
  window keeps the mouse, and each tick reads the cursor so the pointer
  steers outside the window too). Dragged off the mark with the button held,
  it stops at the release.
- **Stopping**: any button press (that press and its own release do nothing
  else, so a right click opens no menu), the wheel, a change of tab, file or
  mode, closing Help, lost focus or capture. Esc stops it and does nothing
  else. Another key stops it and then acts (Home goes to the top), and so
  does Alt with a key (`WM_SYSKEYDOWN`: Alt+Left/Right history, Alt+Up/Down
  line moves, Alt+F4). Shift, Ctrl, Alt and Windows pressed alone leave it
  going. Both key paths go through `autoScrollKey`.
- **Other scrolling meanwhile**: autoscroll keeps its own sub-pixel
  position, but each step first compares the target with where it last left
  it. If something else moved it by more than a pixel (a jump, a relayout),
  it goes on from there instead of pulling the page back.
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
- Alt alone leaves it going; Alt with Left, and Alt with Down, stop it;
- scrolled elsewhere while it runs (1500 DIP down, then back to the top), it
  runs on from the new place each time. Without the step's check, the page
  was pulled back to where autoscroll had left it, and these checks failed;
- a hold dragged off the mark stops at the release; one that stays on the
  mark acts as a click;
- across a page wider than the window (at 650 px), only across;
- the mark paints; entering the editor stops it; the editor's text, and the
  preview (taking the editor with it), the Contents list and Help run on
  their own; closing Help stops it.

The window procedure's routing (wheel, button messages, focus and capture
loss, the cursor read on each tick) is not covered by the harness, which
calls the handlers directly.

## Windows results

### Build

`cmake -S . -B build` and `cmake --build build --config Release`: built
first time, with no errors and no warnings at `/W4`. No source change was
needed to build. After the Alt-key and other-scrolling fixes it still
builds with no warnings.

### Automated

- `ctest -C Release -R middle_click_autoscroll`: passed, printing
  `Middle-click autoscroll: 0 failures` (all eight theme/width/scale runs
  and the short page). After the fixes, `-R
  "middle_click_autoscroll|touch|input"` passed 5 of 5.
- Whole suite, run serially, before the clipboard fixes: 61 of 62 passed.
  The exception was `sheet_canvas`, which failed now and then at "the sheet
  copies as a Mermaid flowchart". It read the clipboard straight after
  writing it, and one `OpenClipboard` call failed with access denied
  while another clipboard client briefly had it open.
- Under `ctest -j 6`, six more tests failed; each passed when run again
  serially. All of them use the system clipboard, so tests running at the
  same time overwrote each other's copies.
- Both causes are fixed in separate commits on this branch. The app now waits
  up to about 200 ms for a busy clipboard, and the tests read their copies
  back with a retry. ctest gives the clipboard tests a `clipboard` resource
  lock, and the Save-dialog tests a `foreground` lock.
- On the combined branch, run on a quiet machine: 0 warnings; the whole suite
  passed 62 of 62 twice serially and twice with `-j 6` (about 26 s each).
  `wrapped_cursor_navigation`, `search_results_panel` and `copy_file_path`
  had each failed once while the real-window driver was sending real input
  to the desktop at the same time. Run again on their own, each passed 20 of
  20.

### Real window

The built `tinta.exe` was run as a portable copy in
`build/autoscroll-manual/` (git-ignored: settings, screenshots and CSV
results stay there) with the fixture open. Real mouse and keyboard input came
through `SendInput`, so it passed through the message loop, the capture and
the timer. Motion was judged by comparing three `PrintWindow` captures 300
ms apart. The driver is `build/autoscroll-manual/drive-window.ps1` (Windows
PowerShell 5.1). It brings Tinta to the front past the foreground lock, and
it refuses to press a button unless Tinta is in front, so no click reaches
another application. A watcher records every other window that takes the
front during a run.

Final runs, after the fixes: primary monitor at 1300 and 650 px wide, twice
each, and the second monitor (negative screen coordinates) at 1300 px. All
at 96 DPI, in the dark default theme, 24 checks a run: 0 failures in all
five. The only other windows in front were Explorer's Alt+Tab switcher and
the window Alt+Tab switched to.

Two things on this machine had to be worked around first; neither is a
fault in Tinta:

- Another program holds **Home, End and PageDown as global hotkeys**
  (`RegisterHotKey` for them fails with 1409), so their key-downs never
  reach any application; only the key-ups arrive. A temporary message log
  confirmed Tinta never received `WM_KEYDOWN` for Home. The driver goes back
  to the top with the wheel instead. Home's stop-and-act behaviour is
  covered by `middle_click_autoscroll`.
- Earlier runs failed intermittently while another `input_tests` run was
  open: its "Save As" and "Name the new sheet" windows took the front, and
  Tinta, losing activation, correctly ended autoscroll. Runs with nothing
  else in front were clean.

Passed in every final run:

- an idle page stays still; a middle click then a pointer below the mark
  runs it down; back on the mark it waits;
- **after a click, the pointer moved 120 px below the window's bottom edge
  still steers it** (the per-tick `GetCursorPos`/`ScreenToClient` read;
  correct on the second monitor's negative coordinates too);
- Esc stops it and does nothing else (the app was not quit, which an
  unconsumed Esc on a bare page would do);
- **Alt+Tab while it runs stops it**, and it is still stopped on return;
- **focus lost with the middle button held stops it**; the button released
  in the other window leaves nothing stuck, and a later middle click starts
  a fresh run;
- the wheel stops it; a hold dragged off the mark stops at the release;
- a left click stops it; a right click stops it;
- **Alt held alone leaves it going; Alt+Down then stops it**, and Tinta
  stays in front;
- a pointer 40 px right of the mark scrolls across at 1300 and at 650 px,
  measured over 150 ms straight after the start. The four-way mark is drawn
  with the right chevron in the accent;
- a right click while it runs opens no context menu. Esc decides it: with a
  menu open, Esc would close the menu and Tinta would stay; Tinta quit, so no
  menu was open and the click had already stopped autoscroll.

Seen in the code on Windows: the process is Per-Monitor V2 DPI-aware, so
`GetCursorPos` + `ScreenToClient` and the mouse message `lParam` are both
physical client pixels, as `autoScrollPointer` expects; speeds are in DIP
through `contentScale`. The window class has no `CS_DBLCLKS`, so a quick
second click arrives as `WM_xBUTTONDOWN`, not `DBLCLK`, and is taken.
`WM_SETCURSOR` is not sent while the window holds the capture, so the
cursor set on each move and tick stays.

### Frame pacing

Reported after 4.0.3 from a screen recording: autoscroll looked choppy at
any speed. Measured with temporary timing in the window procedure, Release
build, a 2560x1400 window on README.md four times over, the pointer held
200 px below the mark for 3 s:

| | 240 Hz monitor | 75 Hz monitor |
|---|---|---|
| 4.0.3, 16 ms timer: steps in 3 s | 122 | - |
| 4.0.3: time between steps | 16-31 ms, mostly 31, uneven | - |
| Paced to the screen: frames in 3 s | 723 | 227 |
| Paced: time between frames | 4.1-4.2 ms (715 of 723) | 13.3-13.4 ms (224 of 227) |

Drawing a frame took 0.55-0.6 ms on average, at most about 1.1 ms. The cost
was the timer, not the drawing: Windows rounds a 16 ms timer up to its
15.6 ms tick, often two, and `WM_TIMER` waits behind other messages. The
recording showed the same: the page changed every 17 or 33 ms of the 60 fps
capture, with stalls up to 333 ms. In both paced runs the one gap of about
230 ms was the script's pause on the mark before it moved the pointer.

`middle_click_autoscroll` adds: autoscroll starts a pacer, the pacer posts a
frame for the next refresh, ending it lets the pacer go and the thread ends,
and a frame posted before the end does nothing. With the thread not started
the posting check fails in every scenario. The full suite passed 62 of 62
(`-j 6`), and the real-window driver 24 of 24 at 1300 and 650 px on the
240 Hz monitor and at 1300 px on the 75 Hz one.

### Remaining

- Home, End and PageDown during autoscroll were not driven in the real
  window: on this machine another program takes their key-downs. The
  harness covers Home.
- Hi-DPI: before 4.0.3 the second monitor was set to 150% and the driver
  passed 24 of 24 there at 144 DPI, the mark about 42 px across; the
  monitor was set back to 100%. The paced frames were not run at 150%.
- Smoothness itself is judged by eye: the measurements show even frame
  timing, not how it looks.
- Not driven in the real window: the cursor's shape (it does not show in
  `PrintWindow`) and how it looks over another application's window, a
  light theme, Source/preview, Live, Contents, file browser, Help, a tab's
  middle-click close and a sheet's pan. The harness covers each target's
  logic, but not through the window procedure.
