#pragma once

#include "Hazel.h"

namespace Hazel {
class EditorLayer : public Hazel::Layer
{
public:
    EditorLayer();
    virtual ~EditorLayer() = default;

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
    Hazel::Ref<Hazel::FrameBuffer> m_FrameBuffer;

    Ref<Scene> m_ActiveScene;
    Entity m_SquareEntity;
    Entity m_CameraEntity;
    Entity m_SecondCamera;

    bool m_PrimaryCamera = true;

    Hazel::Ref<Hazel::Texture2D> m_CheckerBoardTexture;

    bool m_ViewportFocused = false;
    bool m_ViewportHovered = false;
    glm::vec2 m_ViewportSize = { 0.0f, 0.0f };

    // glm::vec4 m_SquareColor;
};
}