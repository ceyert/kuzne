#include "Keyboard.h"
#include "Status.h"
#include "Kernel.h"
#include "task/Process.h"
#include "task/Task.h"
#include "Classic.h"

static struct Keyboard *keyboard_list_head = 0;
static struct Keyboard *keyboard_list_last = 0;

void keyboard_init() {
    keyboard_insert(classic_init());
}

int keyboard_insert(struct Keyboard *keyboard) {
    int res = 0;
    if (keyboard->init == 0) {
        res = -EINVARG;
        goto out;
    }

    if (keyboard_list_last) {
        keyboard_list_last->next = keyboard;
        keyboard_list_last = keyboard;

    } else {
        keyboard_list_head = keyboard;
        keyboard_list_last = keyboard;
    }

    res = keyboard->init();
    out:
    return res;
}

static int keyboard_get_tail_index(struct Process *process) {
    return process->keyboard.tail % sizeof(process->keyboard.buffer);
}

void keyboard_backspace(struct Process *process) {
    process->keyboard.tail -= 1;
    int real_index = keyboard_get_tail_index(process);
    process->keyboard.buffer[real_index] = 0x00;
}

void keyboard_set_capslock(struct Keyboard *keyboard, caps_lock_t state) {
    keyboard->capslockState = state;
}

caps_lock_t keyboard_get_capslock(struct Keyboard *keyboard) {
    return keyboard->capslockState;
}

void keyboard_push(char c) {
    struct Process *process = process_current();
    if (!process) {
        return;
    }

    if (c == 0) {
        return;
    }

    int real_index = keyboard_get_tail_index(process);
    process->keyboard.buffer[real_index] = c;
    process->keyboard.tail++;
}

char keyboard_pop() {
    if (!task_current()) {
        return 0;
    }

    struct Process *process = task_current()->process;
    int real_index = process->keyboard.head % sizeof(process->keyboard.buffer);
    char c = process->keyboard.buffer[real_index];
    if (c == 0x00) {
        // Nothing to pop return zero.
        return 0;
    }

    process->keyboard.buffer[real_index] = 0;
    process->keyboard.head++;
    return c;
}