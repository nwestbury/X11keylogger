#include <X11/XKBlib.h>
#include <X11/keysym.h>
#include <X11/extensions/XInput2.h>

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>

const char * DEFAULT_DISPLAY = ":0";
const int DEFAULT_DELAY = 0;

void help() {
    printf("USAGE: keyrecorder filename [--display / -D <display>] [--delay / -d <seconds>]\n\
    display  target X display                   (default %s)\n\
    delay    number of seconds to wait          (default %d)\n",
        DEFAULT_DISPLAY, DEFAULT_DELAY);
    exit(0);
}

int main(int argc, char * argv[]) {

    const char * hostname    = DEFAULT_DISPLAY;
    int delay = 0;

    if (argc < 2 || argc > 6) { help(); }

    // Get arguments
    char * filename = argv[1];
    for (int i = 2; i < argc; i++) {
        char * arg = argv[i];
        if      (strcmp(arg, "--help") == 0 || strcmp(arg, "-h") == 0)     help();
        else if (strcmp(arg, "--display") == 0 || strcmp(arg, "-D") == 0)  hostname = argv[++i];
        else if (strcmp(arg, "--delay") == 0 || strcmp(arg, "-d") == 0)    delay = (int)strtol(argv[++i], NULL, 10);
        else { printf("Unexpected argument `%s`\n", arg); help(); }
    }

    if (delay) {
        printf("Waiting %d seconds before starting...\n", delay);
        sleep(delay);
        printf("Starting!\n");
    }

    // Set up X
    Display * disp = XOpenDisplay(hostname);
    if (NULL == disp) {
        fprintf(stderr, "Cannot open X display: %s\n", hostname);
        exit(1);
    }

    // Test for XInput 2 extension
    int xi_opcode;
    int queryEvent, queryError;
    if (! XQueryExtension(disp, "XInputExtension", &xi_opcode, &queryEvent, &queryError)) {
        // XXX Test version >=2
        fprintf(stderr, "X Input extension not available\n"); return 1;
    }

    // Register events
    Window root = DefaultRootWindow(disp);
    XIEventMask m;
    m.deviceid = XIAllMasterDevices;
    m.mask_len = XIMaskLen(XI_LASTEVENT);
    m.mask = calloc(m.mask_len, sizeof(char));
    XISetMask(m.mask, XI_RawKeyPress);
    XISetMask(m.mask, XI_RawKeyRelease);
    XISelectEvents(disp, root, &m, 1);
    XSync(disp, false);
    free(m.mask);

    // Open output file
    FILE *fp = fopen(filename, "w+");

    fprintf(fp, "time,code,release\n");

    unsigned long start_time = 0;
    KeySym s = 0;

    while (s != XK_Escape) { // Until Escape is Pressed
        XEvent event;
        XGenericEventCookie *cookie = (XGenericEventCookie*)&event.xcookie;
        XNextEvent(disp, &event);

        if (XGetEventData(disp, cookie) &&
            cookie->type == GenericEvent &&
            cookie->extension == xi_opcode &&
            (cookie->evtype == XI_RawKeyPress || cookie->evtype == XI_RawKeyRelease)) {
            XIRawEvent *ev = cookie->data;

            bool key_press = cookie->evtype == XI_RawKeyPress;

            unsigned long end_time = ev->time;
            if (start_time == 0) { // first event
                start_time = end_time;
            }

            unsigned long relative_time = end_time - start_time;

            // Ask X what it calls that key
            s = XkbKeycodeToKeysym(disp, ev->detail, 0, 0);
            if (NoSymbol == s) continue;
            
            fprintf(fp, "%ld,%ld,%d\n", relative_time, s, key_press);
        }
        fflush(stdout);
    }

    fclose(fp);
    XCloseDisplay(disp);
}