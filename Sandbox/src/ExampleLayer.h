#pragma once

#include "Hazel.h"

class ExampleLayer : public Hazel::Layer 
{
public:
    ExampleLayer();
    virtual ~ExampleLayer() = default;

    virtual void OnAttach() override;
    virtual void OnDetach() override;

    virtual void OnUpdate(Hazel::Timestep dt) override;
    virtual void OnImGuiRender() override;
    virtual void OnEvent(Hazel::Event& e) override;

private:
    Hazel::ShaderLibrary m_ShaderLibrary;
    Hazel::Ref<Hazel::Shader> m_Shader;
    Hazel::Ref<Hazel::VertexArray> m_VertexArray;

    Hazel::Ref<Hazel::Shader> m_FlatColorShader;
    Hazel::Ref<Hazel::VertexArray> m_SquareVertexArray;

    Hazel::Ref<Hazel::Texture2D> m_CheckerBoardTexture, m_ChernoLogoTexture;

    Hazel::OrthographicCameraController m_CameraController;

    glm::vec3 m_SquareColor = { 1.0f, 0.5f, 0.25f };
};