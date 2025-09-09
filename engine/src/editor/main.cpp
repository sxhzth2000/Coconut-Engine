//
// Created by tanhao on 2025/8/23.
//
#include <iostream>
#include "runtime/engine.h"
#include "runtime/function/render/window_system.h"

int main()
{

    Coconut::CoconutEngine* engine = new Coconut::CoconutEngine();
    engine->startEngine();
    engine->run();

    return 0;
}
