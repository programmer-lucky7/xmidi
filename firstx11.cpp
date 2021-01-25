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
int pressedkey[256] = { 0, };
MidiOut *pmo;

void init_x() {
	unsigned long black, white;
	dp = XOpenDisplay((char *) 0);
	screen = DefaultScreen(dp);
	black = BlackPixel(dp, screen);
	white = WhitePixel(dp, screen);

	wnd = XCreateSimpleWindow(dp, DefaultRootWindow(dp),
			0, 0, 640, 480, 5, white, black);
	XSetStandardProperties(dp, wnd, "First X11", "1'st X11", None, NULL, 0, NULL);
	XSelectInput(dp, wnd, ExposureMask | ButtonPressMask | KeyPressMask | KeyReleaseMask | KeyReleaseMask);
	gc = XCreateGC(dp, wnd, 0, 0);
	XSetBackground(dp, gc, white);
	XSetForeground(dp, gc, black);
	XClearWindow(dp, wnd);
	XMapRaised(dp, wnd);
	XkbSetDetectableAutoRepeat(dp, 1, 0);
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

int main() {
	XEvent evt;
	KeySym key;
	char text[256];
	pmo = new MidiOut("Lucky7");
	pmo->Open();

	init_x();
	while(1) {
		XNextEvent(dp, &evt);
		switch(evt.type) {
		case Expose:
			if (evt.xexpose.count = 0) {
				// redraw();
			}
			break;
		case KeyPress:
			if (evt.xkey.keycode == 0x60) {
				close_x();
			}
			if (!pressedkey[evt.xkey.keycode]) {
				int note = 0;
				pressedkey[evt.xkey.keycode] = 1;
				printf("Pressed key code = 0x%02X\n", evt.xkey.keycode);
				switch(evt.xkey.keycode) {
				case 0x34:
					note = 60;
					break;
				case 0x35:
					note = 62;
					break;
				case 0x36:
					note = 64;
					break;
				case 0x37:
					note = 65;
					break;
				case 0x38:
					note = 67;
					break;
				case 0x39:
					note = 69;
					break;
				case 0x3A:
					note = 71;
					break;
				case 0x3B:
					note = 72;
					break;
				}
				pmo->NoteOn(0, note, 64);
			}
			break;
		case KeyRelease:
			pressedkey[evt.xkey.keycode] = 0;
			printf("Pressed key code = 0x%02X\n", evt.xkey.keycode);
			{
				int note = 0;
				switch(evt.xkey.keycode) {
				case 0x34:
					note = 60;
					break;
				case 0x35:
					note = 62;
					break;
				case 0x36:
					note = 64;
					break;
				case 0x37:
					note = 65;
					break;
				case 0x38:
					note = 67;
					break;
				case 0x39:
					note = 69;
					break;
				case 0x3A:
					note = 71;
					break;
				case 0x3B:
					note = 72;
					break;
				}
				pmo->NoteOff(0, note, 64);
			}
			break;
		case ButtonPress:
			printf("You pressed a button at (%d, %d)\n", evt.xbutton.x, evt.xbutton.y);
			break;
		}
	}
}

