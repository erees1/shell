#include "loop.h"

int main(int argc, char *argv[]) {
    Shell *shell = new Shell();
    shell->Loop();
    delete shell;
}
