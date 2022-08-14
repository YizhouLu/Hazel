#include <Hazel.h>

class Sandbox : public Hazel::Application {
public:
    Sandbox() {

        printf("This is the Sandbox!\n");

    }

    ~Sandbox() {

    }
};

Hazel::Application* Hazel::CreateApplication() {
    return new Sandbox();
}