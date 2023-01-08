#include <iostream>
#include "Capp.hpp"

int main(int argc, char **argv)
{
    Capp app;
    return app.run() ? 0 : -1;
}