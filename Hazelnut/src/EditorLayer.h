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

    Hazel::Ref<Hazel::Texture2D> m_CheckerBoardTexture;

    glm::vec4 m_SquareColor;
};
}