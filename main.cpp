#include "webxr.h"
#include <emscripten.h>

#include "webxrexample.h"

// For clarity, our main loop code is declared at the end.
static void main_loop(void*);

WebXrExample g_window;

int main(int, char**)
{
    g_window.init();

    emscripten_set_main_loop_arg(main_loop, NULL, 0, true);
}

static void main_loop(void* arg)
{
    g_window.drawLoop();
}
