#include <X11/Xlib.h>
#include <X11/extensions/XTest.h>

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

const char * DEFAULT_DISPLAY = ":0";
const int DEFAULT_DELAY = 0;


typedef struct {
  unsigned long time;
  unsigned long key;
  int release;
} KeyEvent;

void help() {
    printf("USAGE: keyreplayer filepath [--display / -D <display>] [--delay / -d <seconds>]\n\
    display  target X display                   (default %s)\n\
    delay    number of seconds to wait          (default %d)\n",
        DEFAULT_DISPLAY, DEFAULT_DELAY);
    exit(0);
}

int readFileCodes(char * filename, KeyEvent ** arr) {
    FILE *fp = fopen(filename, "r");
    char buffer[100];

    if(fgets(buffer, 100, fp) == 0){  // ignore header
        printf("Malformed file headers\n");
        fclose(fp);
        exit(0);
    }

    KeyEvent KeyEventBuffer;
    int line_num = 0;
    int size = 10;
    *arr = malloc(size * sizeof(KeyEvent));

    while (fscanf(fp, "%lu,%lu,%d",  &KeyEventBuffer.time, &KeyEventBuffer.key, &KeyEventBuffer.release) == 3) {
        if(line_num >= size) {
            size *= 2;
            *arr = realloc(*arr, size * sizeof(KeyEvent));
        }
        (*arr)[line_num].time = KeyEventBuffer.time;
        (*arr)[line_num].key = KeyEventBuffer.key;
        (*arr)[line_num].release = KeyEventBuffer.release;
        ++line_num;
    }

    fclose(fp);

    return line_num;
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

    printf("Parsing %s...\n", filename);

    KeyEvent * key_events = NULL;
    int num_events = readFileCodes(filename, &key_events);
    printf("Done loading %d key events.\n", num_events);


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

    // Test for XTest extension
    int xi_opcode;
    int queryEvent, queryError;
    if (! XQueryExtension(disp, "XTEST", &xi_opcode, &queryEvent, &queryError)) {
        fprintf(stderr, "XTest extension not available\n"); return 1;
    }

    unsigned long previous_time = 0;
    for(int i=0; i<num_events; ++i) {
        // Get the keycode
        unsigned int keycode = XKeysymToKeycode(disp, key_events[i].key);

        // Simulate the keypress / keyrelease
        XTestFakeKeyEvent(disp, keycode, key_events[i].release, key_events[i].time - previous_time);
        previous_time = key_events[i].time;
    }

    // Clear the X buffer which actually sends the key press
    XFlush(disp);

    free(key_events);

    // Disconnect from X
    XCloseDisplay(disp);

    return 0;
}