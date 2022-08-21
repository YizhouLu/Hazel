#include "Application.h"

#include "Hazel/Events/ApplicationEvent.h"
#include "Hazel/Log.h"

#include <stdio.h>

namespace Hazel {

Application::Application()
{
}

Application::~Application()
{
}

void Application::Run() {
    WindowResizeEvent e(1280, 720);
    if (e.IsInCategory(EventCategoryMouseButton)) {
        HZ_TRACE(e);
    }
    else {
        HZ_WARN(e);
    }
    

    printf("Running!");
}

}
