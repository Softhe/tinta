// Middle-click autoscroll: the middle button sets a mark, and the pointer's
// distance from it scrolls what it was pressed on. See autoscroll.h.
#include "autoscroll.h"

#include "input.h"
#include "touch.h"

#include <algorithm>
#include <cmath>

namespace {

constexpr float kMarkDip = 14.0f;       // the mark's radius: nothing moves within it
constexpr float kSpeedFactor = 0.0032f;  // DIP per ms at 1 DIP past the mark, before the curve
constexpr float kSpeedPower = 1.3f;      // farther goes faster, more than in step
constexpr float kMaxDipPerMs = 8.0f;     // a screen in about a tenth of a second at most
constexpr double kMaxStepMs = 50;        // a late timer does not leap

// Which way the pointer sets it going: -1, 0 or 1 along each way
int heading(const App& app, float offset) {
    return autoScrollSpeed(app, offset) > 0 ? 1 : autoScrollSpeed(app, offset) < 0 ? -1 : 0;
}

void setCursor(const App::AutoScroll& a) {
    static HCURSOR both = LoadCursor(nullptr, IDC_SIZEALL);
    static HCURSOR upDown = LoadCursor(nullptr, IDC_SIZENS);
    static HCURSOR across = LoadCursor(nullptr, IDC_SIZEWE);
    SetCursor(a.acrossX && a.acrossY ? both : a.acrossX ? across : upDown);
}

bool sameContext(const App& app) {
    const auto& a = app.autoScroll;
    return app.activeTab == a.tab && app.currentFile == a.file && app.editMode == a.edit &&
           (a.target != TouchTarget::Help || app.showHelp);
}

}  // namespace

float autoScrollMarkRadius(const App& app) { return dpi(app, kMarkDip); }

float autoScrollSpeed(const App& app, float offset) {
    const float scale = std::max(app.contentScale, 0.01f);
    const float past = std::fabs(offset) / scale - kMarkDip;
    if (past <= 0) return 0;
    const float dipPerMs = std::min(kSpeedFactor * std::pow(past, kSpeedPower), kMaxDipPerMs);
    return std::copysign(dipPerMs * scale, offset);
}

bool autoScrollActive(const App& app) { return app.autoScroll.active; }

bool autoScrollBegin(App& app, HWND hwnd, POINT pt, double ms) {
    auto& a = app.autoScroll;
    if (a.active || app.selecting || app.touch.active) return false;
    PanelList list = PanelList::None;
    const TouchTarget target = touchTargetAt(app, pt, list);
    // A sheet pans with the middle button held, as before
    if (target == TouchTarget::None || target == TouchTarget::Sheet) return false;
    float maxX = 0, maxY = 0;
    touchScrollRange(app, target, list, maxX, maxY);
    if (maxX <= 0 && maxY <= 0) return false;  // nothing to scroll
    a = App::AutoScroll{};
    a.active = a.held = true;
    a.target = target;
    a.list = list;
    a.acrossX = maxX > 0;
    a.acrossY = maxY > 0;
    a.origin = a.pointer = pt;
    a.ms = ms;
    a.tab = app.activeTab;
    a.file = app.currentFile;
    a.edit = app.editMode;
    touchScrollPosition(app, target, list, a.x, a.y);
    a.leftX = a.x;
    a.leftY = a.y;
    // Moves keep coming outside the window, and the next click anywhere
    // comes here to end it
    SetCapture(hwnd);
    SetTimer(hwnd, TIMER_AUTOSCROLL, 16, nullptr);
    setCursor(a);
    InvalidateRect(hwnd, nullptr, FALSE);
    return true;
}

void autoScrollMiddleUp(App& app, HWND hwnd, double ms) {
    auto& a = app.autoScroll;
    if (!a.active || !a.held) return;
    autoScrollStep(app, hwnd, ms);
    if (!a.active) return;
    a.held = false;
    // Dragged off the mark and let go: it was held to scroll, and stops
    if (a.moved) autoScrollEnd(app, hwnd);
}

void autoScrollPointer(App& app, HWND hwnd, POINT pt) {
    auto& a = app.autoScroll;
    if (!a.active) return;
    const float oldX = (float)(a.pointer.x - a.origin.x), oldY = (float)(a.pointer.y - a.origin.y);
    a.pointer = pt;
    const float dx = (float)(pt.x - a.origin.x), dy = (float)(pt.y - a.origin.y);
    if (a.held && std::hypot(dx, dy) > autoScrollMarkRadius(app)) a.moved = true;
    setCursor(a);
    // The mark shows the way it is going
    if (heading(app, dx) != heading(app, oldX) || heading(app, dy) != heading(app, oldY))
        InvalidateRect(hwnd, nullptr, FALSE);
}

bool autoScrollTakesPress(App& app, HWND hwnd, UINT release) {
    auto& a = app.autoScroll;
    if (!a.active) {
        // A release lost with the capture is not waited for past the next press
        a.swallowUp = 0;
        return false;
    }
    autoScrollEnd(app, hwnd);
    a.swallowUp = release;
    return true;
}

bool autoScrollTakesRelease(App& app, UINT release) {
    if (release == 0 || app.autoScroll.swallowUp != release) return false;
    app.autoScroll.swallowUp = 0;
    return true;
}

bool autoScrollStep(App& app, HWND hwnd, double ms) {
    auto& a = app.autoScroll;
    if (!a.active) {
        KillTimer(hwnd, TIMER_AUTOSCROLL);
        return false;
    }
    // The document under it changed: the mark belonged to the other one
    if (!sameContext(app)) {
        autoScrollEnd(app, hwnd);
        return false;
    }
    const double dt = std::min(ms - a.ms, kMaxStepMs);
    if (dt <= 0) return true;
    a.ms = ms;
    const float vx = a.acrossX ? autoScrollSpeed(app, (float)(a.pointer.x - a.origin.x)) : 0.0f;
    const float vy = a.acrossY ? autoScrollSpeed(app, (float)(a.pointer.y - a.origin.y)) : 0.0f;
    if (vx == 0 && vy == 0) return true;
    // Something else moved it meanwhile (a jump, a relayout): go on from there
    float nowX, nowY;
    touchScrollPosition(app, a.target, a.list, nowX, nowY);
    if (std::fabs(nowX - a.leftX) > 1) a.x = nowX;
    if (std::fabs(nowY - a.leftY) > 1) a.y = nowY;
    a.x += vx * (float)dt;
    a.y += vy * (float)dt;
    // Held at an end, it waits there for the pointer to turn back
    touchScrollTo(app, a.target, a.list, a.x, a.y);
    touchScrollPosition(app, a.target, a.list, a.leftX, a.leftY);
    return true;
}

bool autoScrollKey(App& app, HWND hwnd, WPARAM vk) {
    if (!app.autoScroll.active || vk == VK_SHIFT || vk == VK_CONTROL || vk == VK_MENU || vk == VK_LWIN ||
        vk == VK_RWIN)
        return false;
    autoScrollEnd(app, hwnd);
    return true;
}

void autoScrollEnd(App& app, HWND hwnd) {
    auto& a = app.autoScroll;
    if (!a.active) return;
    a.active = a.held = false;
    KillTimer(hwnd, TIMER_AUTOSCROLL);
    if (GetCapture() == hwnd) ReleaseCapture();
    // Hover and the cursor come back for where the pointer is now
    handleMouseMove(app, hwnd, MAKELPARAM(a.pointer.x, a.pointer.y));
    InvalidateRect(hwnd, nullptr, FALSE);
}

void renderAutoScrollMark(App& app) {
    const auto& a = app.autoScroll;
    if (!a.active || !app.renderTarget || !app.brush) return;
    auto* rt = app.renderTarget;
    const float cx = (float)a.origin.x, cy = (float)a.origin.y;
    const float r = autoScrollMarkRadius(app);
    const auto ring = D2D1::Ellipse(D2D1::Point2F(cx, cy), r, r);
    D2D1_COLOR_F fill = app.theme.background;
    fill.a = 0.92f;
    app.brush->SetColor(fill);
    rt->FillEllipse(ring, app.brush);
    D2D1_COLOR_F line = app.theme.text;
    line.a = 0.45f;
    app.brush->SetColor(line);
    rt->DrawEllipse(ring, app.brush, dpi(app, 1.2f));
    rt->FillEllipse(D2D1::Ellipse(D2D1::Point2F(cx, cy), dpi(app, 1.8f), dpi(app, 1.8f)), app.brush);

    // A chevron for each way it can go; the way it is going in the accent
    const int hx = heading(app, (float)(a.pointer.x - a.origin.x));
    const int hy = heading(app, (float)(a.pointer.y - a.origin.y));
    const float tip = r * 0.72f, base = r * 0.40f, half = r * 0.30f, stroke = dpi(app, 1.6f);
    auto chevron = [&](float ux, float uy, bool going) {
        D2D1_COLOR_F c = going ? app.theme.accent : app.theme.text;
        c.a = going ? 1.0f : 0.7f;
        app.brush->SetColor(c);
        // Along (ux, uy) from the centre; the arms spread across it
        const D2D1_POINT_2F t = D2D1::Point2F(cx + ux * tip, cy + uy * tip);
        const D2D1_POINT_2F l = D2D1::Point2F(cx + ux * base - uy * half, cy + uy * base + ux * half);
        const D2D1_POINT_2F rr = D2D1::Point2F(cx + ux * base + uy * half, cy + uy * base - ux * half);
        rt->DrawLine(l, t, app.brush, stroke);
        rt->DrawLine(t, rr, app.brush, stroke);
    };
    if (a.acrossY) {
        chevron(0, -1, hy < 0);
        chevron(0, 1, hy > 0);
    }
    if (a.acrossX) {
        chevron(-1, 0, hx < 0);
        chevron(1, 0, hx > 0);
    }
}
