#include "loop.h"

int main(int argc, char *argv[]) {
    Shell* shell = Shell::GetInstance();
    shell->Loop();
    Shell::cleanup();
}
