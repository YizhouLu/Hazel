#pragma once

#include "Hazel.h"

class Sandbox2D : public Hazel::Layer
{
public:
    Sandbox2D();
    virtual ~Sandbox2D() = default;

    virtual void OnAttach() override;
    virtual void OnDetach() override;

    virtual void OnUpdate(Hazel::Timestep dt) override;
    virtual void OnImGuiRender() override;
    virtual void OnEvent(Hazel::Event& e) override;
private:
    Hazel::OrthographicCameraController m_CameraController;

    // Temp
    // Hazel::Ref<Hazel::VertexArray> m_SquareVertexArray;
    // Hazel::Ref<Hazel::Shader> m_FlatColorShader;

    Hazel::Ref<Hazel::Texture2D> m_CheckerBoardTexture;

    struct ProfileResult {
        const char* Name;
        float Time;
    };

    std::vector<ProfileResult> m_ProfileResults;

    glm::vec4 m_SquareColor = { 1.0f, 0.5f, 0.25f, 1.0f };
};
