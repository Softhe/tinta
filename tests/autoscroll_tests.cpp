// Middle-click autoscroll: the middle button sets a mark on what it is
// pressed on, and the pointer's distance from the mark scrolls it, faster
// the farther. A click leaves it running until the next click, key or wheel
// turn; a drag off the mark stops at the release.
#include "autoscroll.h"
#include "d2d_init.h"
#include "editor.h"
#include "editrail.h"
#include "input.h"
#include "keystroke.h"
#include "overlays.h"
#include "render.h"
#include "settings.h"
#include "sidepanels.h"
#include "touch.h"
#include "utils.h"

#include <cmath>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>

namespace {
int failures = 0;
void check(bool ok, const std::string& message) {
    if (!ok) { if (failures < 60) std::cerr << "FAIL autoscroll: " << message << '\n'; ++failures; }
}

double ms = 1000.0;

POINT shifted(POINT p, int dx, int dy) { return POINT{p.x + dx, p.y + dy}; }

// The timer's steps over a span, 16 ms apart
void run(App& app, double span) {
    for (double t = 0; t < span; t += 16) autoScrollStep(app, app.hwnd, ms += 16);
}

void editFrame(App& app) {
    ensureLayoutComplete(app);
    app.renderTarget->BeginDraw();
    renderEditor(app, editorPaneWidth(app));
    check(SUCCEEDED(app.renderTarget->EndDraw()), "the editor paints");
    syncPreviewToEditor(app);
}

void resetPage(App& app) {
    autoScrollEnd(app, app.hwnd);
    app.autoScroll.swallowUp = 0;
    app.scrollX = app.targetScrollX = app.scrollY = app.targetScrollY = 0;
}

void scenario(App& app, const char* path, int width, float scale) {
    const std::string where = " (" + std::to_string(width) + ", " + std::to_string(scale).substr(0, 3) + "x)";
    app.contentScale = scale;
    app.width = (int)(width * scale);
    app.height = (int)(800 * scale);
    updateTextFormats(app);
    check(openDocumentInViewer(app, toWide(path)), "the fixture opens" + where);
    ensureLayoutComplete(app);
    app.verticalScrollbarVisible = app.contentHeight > app.height;
    resetPage(app);
    const float maxY = app.contentHeight - app.height;
    check(maxY > 2000, "the page is several screens long" + where);
    const POINT middle{app.width / 2, app.height / 2};
    const float mark = autoScrollMarkRadius(app);

    // The speed: nothing within the mark, then faster the farther, the
    // same either way, up to a cap; in DIP, so the same at any scale
    check(autoScrollSpeed(app, 0) == 0 && autoScrollSpeed(app, mark * 0.9f) == 0 &&
              autoScrollSpeed(app, -mark * 0.9f) == 0,
          "nothing moves within the mark" + where);
    float previous = 0;
    bool rising = true, even = true;
    for (float off = mark + 1; off < dpi(app, 1000); off += dpi(app, 10)) {
        const float v = autoScrollSpeed(app, off);
        if (v < previous) rising = false;
        if (std::fabs(autoScrollSpeed(app, -off) + v) > 1e-5f) even = false;
        previous = v;
    }
    check(rising && even, "farther from the mark goes faster, the same up and down" + where);
    check(std::fabs(autoScrollSpeed(app, dpi(app, 100000)) - dpi(app, 8)) < 1e-3f,
          "the speed has a cap" + where);
    const float at64 = autoScrollSpeed(app, dpi(app, 64)) / scale;
    check(at64 > 0.4f && at64 < 0.6f, "50 DIP past the mark runs about 500 DIP a second" + where + ": " +
                                          std::to_string(at64));

    // Where it starts: the page, not the tab strip, the scrollbar or an
    // overlay, and not during a selection drag
    check(!autoScrollBegin(app, app.hwnd, POINT{app.width / 2, (LONG)(chromeTopHeight(app) / 2)}, ms) &&
              !autoScrollBegin(app, app.hwnd, POINT{app.width - (LONG)dpi(app, 4), app.height / 2}, ms),
          "the tab strip and the scrollbar keep their middle click" + where);
    app.showSettings = true;
    check(!autoScrollBegin(app, app.hwnd, middle, ms), "an open overlay does not autoscroll" + where);
    app.showSettings = false;
    app.selecting = true;
    check(!autoScrollBegin(app, app.hwnd, middle, ms), "a selection drag does not autoscroll" + where);
    app.selecting = false;
    check(!autoScrollActive(app), "nothing has started" + where);

    // A click: released on the mark, it keeps going
    check(autoScrollBegin(app, app.hwnd, middle, ms), "a middle click on the page starts autoscroll" + where);
    check(app.autoScroll.target == TouchTarget::Document && app.autoScroll.acrossY,
          "it drives the page, down and up" + where);
    autoScrollMiddleUp(app, app.hwnd, app.autoScroll.pointer, ms += 80);
    check(autoScrollActive(app) && !app.autoScroll.held && app.scrollY == 0,
          "released on the mark it stays on, and has not moved" + where);
    // Below the mark the page runs down at the speed for that distance
    const int below = (int)dpi(app, 100);
    autoScrollPointer(app, app.hwnd, shifted(middle, 0, below));
    const double start = ms;
    run(app, 1000);
    const float expected = autoScrollSpeed(app, (float)below) * (float)(ms - start);
    check(std::fabs(app.scrollY - expected) < 1 && app.scrollX == 0,
          "below the mark the page runs down at that speed" + where + ": " + std::to_string(app.scrollY) +
              " for " + std::to_string(expected));
    const float down = app.scrollY;
    autoScrollPointer(app, app.hwnd, shifted(middle, 0, -below / 2));
    run(app, 300);
    check(app.scrollY < down && app.scrollY > 0, "above the mark it runs back up" + where);
    const float still = app.scrollY;
    autoScrollPointer(app, app.hwnd, shifted(middle, (int)(mark * 0.5f), -(int)(mark * 0.5f)));
    run(app, 500);
    check(app.scrollY == still && autoScrollActive(app), "back on the mark it waits" + where);
    // A late timer does not leap
    autoScrollPointer(app, app.hwnd, shifted(middle, 0, below));
    autoScrollStep(app, app.hwnd, ms += 2000);
    check(app.scrollY - still <= autoScrollSpeed(app, (float)below) * 50 + 0.5f, "a late step moves at most 50 ms on" + where);
    // A click ends it, and that click and its release do nothing else
    check(autoScrollTakesPress(app, app.hwnd, WM_LBUTTONUP) && !autoScrollActive(app), "a click stops autoscroll" + where);
    check(!autoScrollTakesRelease(app, WM_MBUTTONUP), "another button's release is not the click's" + where);
    check(autoScrollTakesRelease(app, WM_LBUTTONUP) && !autoScrollTakesRelease(app, WM_LBUTTONUP),
          "only that click's release is taken" + where);
    check(!autoScrollTakesPress(app, app.hwnd, WM_LBUTTONUP), "later clicks go on as usual" + where);
    // A release lost with the capture is not waited for past the next press
    check(autoScrollBegin(app, app.hwnd, middle, ms += 500), "it starts again for a lost release" + where);
    autoScrollTakesPress(app, app.hwnd, WM_RBUTTONUP);
    check(!autoScrollTakesPress(app, app.hwnd, WM_RBUTTONUP) && !autoScrollTakesRelease(app, WM_RBUTTONUP),
          "the next press clears a release that never came" + where);
    const float stopped = app.scrollY;
    autoScrollStep(app, app.hwnd, ms += 16);
    check(app.scrollY == stopped, "once stopped, a stray step does nothing" + where);

    // Its steps come at each refresh of the screen, from a thread of its own
    check(autoScrollBegin(app, app.hwnd, middle, ms += 500) && app.autoScroll.pacer,
          "it starts a pacer for the screen's refreshes" + where);
    const std::weak_ptr<AutoScrollPacer> pacer = app.autoScroll.pacer;
    MSG frame{};
    bool posted = false;
    for (int i = 0; i < 100 && !posted; ++i) {
        posted = PeekMessageW(&frame, app.hwnd, WM_APP_AUTOSCROLL_FRAME, WM_APP_AUTOSCROLL_FRAME, PM_REMOVE);
        if (!posted) Sleep(5);
    }
    check(posted, "the pacer posts a frame for the next refresh" + where);
    // A frame reads the pointer and steps to the refresh's time: the mark set
    // above where the pointer really is, so it runs down at that distance
    {
        auto& as = app.autoScroll;
        POINT cursor{};
        check(GetCursorPos(&cursor) && ScreenToClient(app.hwnd, &cursor), "the pointer can be read" + where);
        as.origin = shifted(cursor, 0, -below);
        as.held = false;
        while (PeekMessageW(&frame, app.hwnd, WM_APP_AUTOSCROLL_FRAME, WM_APP_AUTOSCROLL_FRAME, PM_REMOVE)) {}
        as.pacer->pending = true;  // as the pacer leaves it once it has posted
        as.pacer->blankMs = as.ms + 16;
        const float before = app.scrollY;
        autoScrollFrame(app, app.hwnd);
        const float framed = autoScrollSpeed(app, (float)below) * 16;
        check(autoScrollActive(app) && std::fabs(app.scrollY - before - framed) < 1,
              "a frame runs the page on by the refresh's time" + where + ": " +
                  std::to_string(app.scrollY - before) + " for " + std::to_string(framed));
        check(as.pacer && !as.pacer->pending, "after drawing, the frame lets the next refresh post" + where);
        posted = false;
        for (int i = 0; i < 100 && !posted; ++i) {
            posted = PeekMessageW(&frame, app.hwnd, WM_APP_AUTOSCROLL_FRAME, WM_APP_AUTOSCROLL_FRAME, PM_REMOVE);
            if (!posted) Sleep(5);
        }
        check(posted, "and the next refresh posts another" + where);
    }
    const float framedTo = app.scrollY;
    autoScrollEnd(app, app.hwnd);
    check(!app.autoScroll.pacer, "ending it lets the pacer go" + where);
    for (int i = 0; i < 100 && !pacer.expired(); ++i) Sleep(5);
    check(pacer.expired(), "the pacer's thread ends with it" + where);
    autoScrollFrame(app, app.hwnd);
    check(app.scrollY == framedTo && !autoScrollActive(app), "a frame posted before it ended does nothing" + where);
    while (PeekMessageW(&frame, app.hwnd, WM_APP_AUTOSCROLL_FRAME, WM_APP_AUTOSCROLL_FRAME, PM_REMOVE)) {}

    // At the end it waits for the pointer to turn back
    app.scrollY = app.targetScrollY = maxY - dpi(app, 50);
    check(autoScrollBegin(app, app.hwnd, middle, ms += 500), "it starts near the end" + where);
    autoScrollMiddleUp(app, app.hwnd, app.autoScroll.pointer, ms += 50);
    autoScrollPointer(app, app.hwnd, shifted(middle, 0, (int)dpi(app, 300)));
    run(app, 500);
    check(std::fabs(app.scrollY - maxY) < 0.5f && autoScrollActive(app), "it stops at the page's end and stays on" + where);
    autoScrollPointer(app, app.hwnd, shifted(middle, 0, -(int)dpi(app, 100)));
    run(app, 200);
    check(app.scrollY < maxY - 1, "turned back above the mark, it runs up from the end" + where);
    // Esc stops it and does nothing else
    const float beforeEsc = app.scrollY;
    check(keystroke(app, VK_ESCAPE) && !autoScrollActive(app) && app.scrollY == beforeEsc,
          "Esc stops autoscroll and only that" + where);

    // Another key stops it and acts; Shift alone leaves it going
    check(autoScrollBegin(app, app.hwnd, middle, ms += 500), "it starts again" + where);
    autoScrollMiddleUp(app, app.hwnd, app.autoScroll.pointer, ms += 50);
    keystroke(app, VK_SHIFT, KeyMods{false, true, false});
    check(autoScrollActive(app), "Shift alone leaves it going" + where);
    keystroke(app, VK_HOME);
    check(!autoScrollActive(app) && app.targetScrollY == 0, "Home stops it and goes to the top" + where);
    resetPage(app);

    // Alt keys come as WM_SYSKEYDOWN: Alt alone leaves it going, Alt with
    // a key stops it (and the key goes on to act)
    for (WPARAM key : {(WPARAM)VK_LEFT, (WPARAM)VK_DOWN}) {
        check(autoScrollBegin(app, app.hwnd, middle, ms += 500), "it starts for Alt" + where);
        autoScrollMiddleUp(app, app.hwnd, app.autoScroll.pointer, ms += 50);
        check(!autoScrollKey(app, app.hwnd, VK_MENU) && autoScrollActive(app), "Alt alone leaves it going" + where);
        check(autoScrollKey(app, app.hwnd, key) && !autoScrollActive(app),
              "Alt with " + std::string(key == VK_LEFT ? "Left" : "Down") + " stops it" + where);
    }
    resetPage(app);

    // Scrolled elsewhere meanwhile (a jump, a relayout), it goes on from
    // there rather than pulling the page back
    check(autoScrollBegin(app, app.hwnd, middle, ms += 500), "it starts for a jump" + where);
    autoScrollMiddleUp(app, app.hwnd, app.autoScroll.pointer, ms += 50);
    autoScrollPointer(app, app.hwnd, shifted(middle, 0, (int)dpi(app, 100)));
    run(app, 300);
    const float beforeJump = app.scrollY;
    const float jumpTo = beforeJump + dpi(app, 1500);
    app.scrollY = app.targetScrollY = jumpTo;
    run(app, 100);
    check(app.scrollY > jumpTo && app.scrollY < jumpTo + dpi(app, 200),
          "after a jump down it runs on from the new place" + where + ": " + std::to_string(app.scrollY) +
              " from " + std::to_string(jumpTo));
    app.scrollY = app.targetScrollY = 0;
    run(app, 100);
    check(app.scrollY > 0 && app.scrollY < dpi(app, 200),
          "after a jump to the top it runs on from the top" + where + ": " + std::to_string(app.scrollY));
    resetPage(app);

    // A hold: dragged off the mark, the release stops it; a hold that
    // stays on the mark is a click
    check(autoScrollBegin(app, app.hwnd, middle, ms += 500), "a held middle button starts autoscroll" + where);
    autoScrollPointer(app, app.hwnd, shifted(middle, 0, (int)dpi(app, 80)));
    run(app, 300);
    check(app.autoScroll.held && app.autoScroll.moved && app.scrollY > 0, "held off the mark, the page runs" + where);
    autoScrollMiddleUp(app, app.hwnd, app.autoScroll.pointer, ms += 16);
    check(!autoScrollActive(app) && !autoScrollTakesRelease(app, WM_MBUTTONUP), "the release stops it there" + where);
    const float held = app.scrollY;
    autoScrollStep(app, app.hwnd, ms += 16);
    check(app.scrollY == held, "nothing runs after the release" + where);
    // Released off the mark before any move came: the release point says it was a drag
    check(autoScrollBegin(app, app.hwnd, middle, ms += 500), "a hold starts for a quick release" + where);
    autoScrollMiddleUp(app, app.hwnd, shifted(middle, 0, (int)dpi(app, 80)), ms += 16);
    check(!autoScrollActive(app), "released off the mark with no move before it, it stops" + where);
    resetPage(app);
    check(autoScrollBegin(app, app.hwnd, middle, ms += 500), "another hold starts" + where);
    autoScrollPointer(app, app.hwnd, shifted(middle, (int)(mark * 0.6f), 0));
    autoScrollMiddleUp(app, app.hwnd, app.autoScroll.pointer, ms += 400);
    check(autoScrollActive(app), "a hold that kept to the mark leaves it on, as a click does" + where);
    resetPage(app);

    // Sideways: a page wider than the window runs across too
    const float across = app.contentWidth - documentViewportWidth(app);
    if (across > dpi(app, 20)) {
        check(autoScrollBegin(app, app.hwnd, middle, ms += 500) && app.autoScroll.acrossX,
              "a wide page autoscrolls across as well" + where);
        autoScrollPointer(app, app.hwnd, shifted(middle, (int)dpi(app, 80), 0));
        run(app, 300);
        check(app.scrollX > 0 && app.scrollY == 0, "right of the mark it runs across, and only across" + where);
        resetPage(app);
    } else {
        check(width > 900, "the narrow window gives the fixture a sideways scroll" + where);
    }

    // The mark paints, over the page, the way it is going in the accent
    check(autoScrollBegin(app, app.hwnd, middle, ms += 500), "it starts for the paint" + where);
    autoScrollPointer(app, app.hwnd, shifted(middle, 0, (int)dpi(app, 60)));
    app.renderTarget->BeginDraw();
    renderAutoScrollMark(app);
    check(SUCCEEDED(app.renderTarget->EndDraw()), "the mark paints" + where);

    // Another document under it ends it
    enterEditMode(app);
    check(!autoScrollStep(app, app.hwnd, ms += 16) && !autoScrollActive(app),
          "a change of mode ends autoscroll" + where);

    // Edit mode: the editor's text and the preview beside it
    app.editorShowPreview = true;
    app.editorScrollY = 0;
    editFrame(app);
    const float textX = editRailWidth(app) + (editorPaneWidth(app) - editRailWidth(app)) / 2;
    const POINT inEditor{(LONG)textX, app.height / 2};
    // Not while a selection or a text drag in the editor follows the left button
    app.editorSelecting = true;
    check(!autoScrollBegin(app, app.hwnd, inEditor, ms += 500), "an editor selection drag does not autoscroll" + where);
    app.editorSelecting = false;
    app.editorTextDrag.armed = true;
    check(!autoScrollBegin(app, app.hwnd, inEditor, ms += 500), "an editor text drag does not autoscroll" + where);
    app.editorTextDrag.armed = false;
    check(autoScrollBegin(app, app.hwnd, inEditor, ms += 500) && app.autoScroll.target == TouchTarget::Editor,
          "a middle click on the editor's text autoscrolls the editor" + where);
    autoScrollPointer(app, app.hwnd, shifted(inEditor, 0, (int)dpi(app, 80)));
    run(app, 300);
    check(app.editorScrollY > 0, "the editor runs down" + where);
    autoScrollEnd(app, app.hwnd);
    app.editorScrollY = 0;
    editFrame(app);
    const POINT inPreview{(LONG)(documentViewportX(app) + documentViewportWidth(app) / 2), app.height / 2};
    check(autoScrollBegin(app, app.hwnd, inPreview, ms += 500) && app.autoScroll.target == TouchTarget::Document,
          "a middle click on the preview autoscrolls the page" + where);
    autoScrollPointer(app, app.hwnd, shifted(inPreview, 0, (int)dpi(app, 80)));
    run(app, 300);
    check(app.scrollY > 0 && app.editorScrollY > 0, "the preview runs down and takes the editor along" + where);
    autoScrollEnd(app, app.hwnd);
    exitEditMode(app);
    ensureLayoutComplete(app);
    resetPage(app);

    // The Contents list runs; the page stays
    app.showToc = true;
    app.tocAnimation = 1;
    app.tocScroll = 0;
    app.tocPinned = false;
    app.tocFilter.clear();
    const D2D1_RECT_F rows = tocListRect(app);
    const POINT inList{(LONG)(rows.left + dpi(app, 40)), (LONG)((rows.top + rows.bottom) / 2)};
    check(autoScrollBegin(app, app.hwnd, inList, ms += 500) && app.autoScroll.target == TouchTarget::List &&
              app.autoScroll.list == PanelList::Contents,
          "a middle click on the Contents autoscrolls its list" + where);
    autoScrollPointer(app, app.hwnd, shifted(inList, 0, (int)dpi(app, 80)));
    run(app, 300);
    check(app.tocScroll > 0 && app.scrollY == 0, "the Contents list runs, the page stays" + where);
    autoScrollEnd(app, app.hwnd);
    app.showToc = false;
    app.tocAnimation = 0;

    // Help runs, and ends with Help
    app.showHelp = true;
    app.helpAnimation = 1;
    app.helpScroll = 0;
    app.renderTarget->BeginDraw();
    renderHelpOverlay(app);
    check(SUCCEEDED(app.renderTarget->EndDraw()), "help paints");
    const auto help = helpPanelRect(app);
    const POINT inHelp{(LONG)((help.left + help.right) / 2), (LONG)(app.helpScrollbarTop + app.helpVisibleHeight / 2)};
    check(autoScrollBegin(app, app.hwnd, inHelp, ms += 500) && app.autoScroll.target == TouchTarget::Help,
          "a middle click in Help autoscrolls Help" + where);
    autoScrollPointer(app, app.hwnd, shifted(inHelp, 0, (int)dpi(app, 80)));
    run(app, 300);
    check(app.helpScroll > 0 && app.scrollY == 0, "Help runs, the page stays" + where);
    app.showHelp = false;
    check(!autoScrollStep(app, app.hwnd, ms += 16) && !autoScrollActive(app), "closing Help ends it" + where);
    resetPage(app);
}

// A page that fits the window has nothing to autoscroll
void shortPage(App& app) {
    const char* name = "autoscroll-short.md";
    std::ofstream(name) << "# Short\n\nOne line, with **bold** and a [link](#short).\n";
    app.contentScale = 1;
    app.width = 1050;
    app.height = 800;
    updateTextFormats(app);
    check(openDocumentInViewer(app, toWide(name)), "the short page opens");
    ensureLayoutComplete(app);
    check(!autoScrollBegin(app, app.hwnd, POINT{app.width / 2, app.height / 2}, ms += 500) && !autoScrollActive(app),
          "a page that fits the window does not autoscroll");
}
}  // namespace

int runAutoScrollTests(const char* path) {
    auto state = std::make_unique<App>();
    App& app = *state;
    app.hwnd = CreateWindowExW(0, L"STATIC", L"Autoscroll tests", WS_POPUP, 0, 0, 1050, 800,
                               nullptr, nullptr, nullptr, nullptr);
    if (!app.hwnd || !initD2D(app) || !createRenderTarget(app)) return 2;
    Settings settings;
    settings.keyProfile = "windows";
    applyKeymap(app, settings);
    for (int theme : {0, 5}) {
        applyTheme(app, theme);
        for (int width : {1050, 650})
            for (float scale : {1.0f, 1.5f}) scenario(app, path, width, scale);
    }
    shortPage(app);
    autoScrollEnd(app, app.hwnd);
    DestroyWindow(app.hwnd);
    app.hwnd = nullptr;
    state.reset();
    CoUninitialize();
    std::cout << "Middle-click autoscroll: " << failures << " failures\n";
    return failures ? 1 : 0;
}
