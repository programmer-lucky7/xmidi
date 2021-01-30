#include <X11/Xlib.h>
#include <X11/XKBlib.h>
#include <X11/Xutil.h>
#include <stdio.h>
#include <stdlib.h>
#include "MidiOut.h"

Display *dp;
int screen;
Window wnd;
GC gc;
unsigned long red;
int pressedkey[256] = { 0, };
const int keymap[256] = {
//	00  01  02  03  04  05  06  07  08  09  0A  0B  0C  0D  0E  0F
	-1, -1, -1, -1, -1, -1, -1, -1, -1, 45, 52, 54, 56, 58, -1, 61, // 00 ~ 0F
	71, 73, 75, -1, 78, 80, 84, 52, 53, 55, 57, 59, 60, 62, 72, 74, // 10 ~ 1F
	76, 77, 79, 81, 83, -1, 59, 61, 63, -1, 66, 68, 70, -1, 73, 75, // 20 ~ 2F
	77, 50, -1, 82, 60, 62, 64, 65, 67, 69, 71, 72, 74, 76, -1, 94, // 30 ~ 3F
	-1, -1, 48, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 90, -1, 89, // 40 ~ 4F
	91, 93, 96, -1, -1, -1, 95, -1, -1, -1, -1, -1, -1, -1, -1, -1, // 50 ~ 5F
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 92, -1, -1, -1, 87, -1, // 60 ~ 6F
	89, -1, -1, 86, -1, 88, 85, 84, -1, -1, -1, -1, -1, -1, -1, -1, // 70 ~ 7F
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, // 80 ~ 8F
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, // 90 ~ 9F
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, // A0 ~ AF
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, // B0 ~ BF
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, // C0 ~ CF
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, // D0 ~ DF
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, // E0 ~ EF
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, // F0 ~ FF
};
MidiOut *pmo;

void init_x() {
	unsigned long black, white;
	dp = XOpenDisplay((char *) 0);
	screen = DefaultScreen(dp);
	black = BlackPixel(dp, screen);
	white = WhitePixel(dp, screen);

	wnd = XCreateSimpleWindow(dp, DefaultRootWindow(dp),
			0, 0, 751, 200, 5, black, white);
	XSetStandardProperties(dp, wnd, "First X11", "1'st X11", None, NULL, 0, NULL);
	XSelectInput(dp, wnd, ExposureMask | ButtonPressMask | KeyPressMask | KeyReleaseMask | KeyReleaseMask);
	gc = XCreateGC(dp, wnd, 0, 0);
	XSetBackground(dp, gc, white);
	XSetForeground(dp, gc, black);
	XClearWindow(dp, wnd);
	XMapRaised(dp, wnd);
	XkbSetDetectableAutoRepeat(dp, 1, 0);

	XColor tmp;
	XParseColor(dp, DefaultColormap(dp, screen), "red", &tmp);
	XAllocColor(dp, DefaultColormap(dp, screen), &tmp);
	red = tmp.pixel;
}

void close_x() {
	XFreeGC(dp, gc);
	XDestroyWindow(dp, wnd);
	XCloseDisplay(dp);
	if (pmo) {
		pmo->Close();
		delete pmo;
		pmo = NULL;
	}
	exit(0);
}

void redraw() {
	const int black_table[7] = { 1, 1, 0, 1, 1, 1, 0 };
//	XSetForeground(dp, gc, WhitePixel(dp, screen));
//	XFillRectangle(dp, wnd, gc, 100, 100, 200, 200);
	XSetForeground(dp, gc, BlackPixel(dp, screen));
	XDrawRectangle(dp, wnd, gc, 0, 0, 750, 120);

	for (int i = 0; i < 74; i++) {
		XDrawRectangle(dp, wnd, gc, i * 10, 0, 10, 120);
		if (black_table[i % 7])
			XFillRectangle(dp, wnd, gc, 7 + i * 10, 0, 7, 70);
	}
}

void WndProc(XEvent *evt) {
	switch(evt->type) {
	case Expose:
		if (evt->xexpose.count == 0) {
			redraw();
		}
		break;
	case KeyPress:
		if (evt->xkey.keycode == 0x60) {
			close_x();
		}
		if (!pressedkey[evt->xkey.keycode]) {
			int note = keymap[evt->xkey.keycode];
			pressedkey[evt->xkey.keycode] = 1;
			printf("Pressed key code = 0x%02X\n", evt->xkey.keycode);
			if (note >= 0) {
				const int white_map[12] = { 1, 0, 1, 0, 1, 1, 0, 1, 0, 1, 0, 1 };
				const int offsets[12] = { 0, 0, 10, 10, 20, 30, 30, 40, 40, 50, 50, 60 };
				pmo->NoteOn(0, note, 64);
				XSetForeground(dp, gc, red);
				if (white_map[note % 12]) {
					XFillArc(dp, wnd, gc, note / 12 * 70 + offsets[note % 12], 100, 10, 10, 0, 23040);
				} else {
					XFillArc(dp, wnd, gc, note / 12 * 70 + offsets[note % 12] + 6, 40, 7, 7, 0, 23040);
				}
			}
		}
		break;
	case KeyRelease:
		pressedkey[evt->xkey.keycode] = 0;
		printf("Pressed key code = 0x%02X\n", evt->xkey.keycode);
		{
			int note = keymap[evt->xkey.keycode];
			if (note >= 0) {
				const int white_map[12] = { 1, 0, 1, 0, 1, 1, 0, 1, 0, 1, 0, 1 };
				const int offsets[12] = { 0, 0, 10, 10, 20, 30, 30, 40, 40, 50, 50, 60 };
				pmo->NoteOff(0, note, 64);
				if (white_map[note % 12]) {
					XSetForeground(dp, gc, WhitePixel(dp, screen));
					XFillArc(dp, wnd, gc, note / 12 * 70 + offsets[note % 12], 100, 10, 10, 0, 23040);
				} else {
					XSetForeground(dp, gc, BlackPixel(dp, screen));
					XFillArc(dp, wnd, gc, note / 12 * 70 + offsets[note % 12] + 6, 40, 7, 7, 0, 23040);
				}
			}
		}
		break;
	case ButtonPress:
		printf("You pressed a button at (%d, %d)\n", evt->xbutton.x, evt->xbutton.y);
		break;
	}
}

int main() {
	XEvent evt;
	KeySym key;
	char text[256];
	pmo = new MidiOut("Lucky7");
	pmo->Open();

	init_x();
	while(1) {
		XNextEvent(dp, &evt);
		WndProc(&evt);
	}
}

