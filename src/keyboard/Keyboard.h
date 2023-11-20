#ifndef KEYBOARD_H
#define KEYBOARD_H

#define KEYBOARD_CAPS_LOCK_ON 1
#define KEYBOARD_CAPS_LOCK_OFF 0

typedef int caps_lock_t;

// storage symbol decleration
struct Process;

typedef int (*KEYBOARD_INIT_FUNCTION)();

struct Keyboard {
    KEYBOARD_INIT_FUNCTION init;
    char name[20];

    caps_lock_t capslockState;

    struct Keyboard *next;
};

extern void keyboard_init();

extern void keyboard_backspace(struct Process *process);

extern void keyboard_push(char c);

extern char keyboard_pop();

extern int keyboard_insert(struct Keyboard *keyboard);

extern void keyboard_set_capslock(struct Keyboard *keyboard, caps_lock_t state);

extern caps_lock_t keyboard_get_capslock(struct Keyboard *keyboard);

#endif