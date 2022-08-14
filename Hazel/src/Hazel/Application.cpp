#include "Application.h"

#include <stdio.h>

namespace Hazel {

Application::Application()
{
    printf("This is the base Application!\n");
}

Application::~Application()
{
}

void Application::Run() {
    while (true);
}

}
