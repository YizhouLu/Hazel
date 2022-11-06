#include "ExampleLayer.h"

#include <imgui/imgui.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

ExampleLayer::ExampleLayer()
    : Layer("Example"), m_CameraController(1280.0f / 720.0f, true)
{
    /////////////////
    // VertexArray //
    /////////////////

    m_VertexArray = Hazel::VertexArray::Create();

    ////////////
    // Buffer //
    ////////////

    float vertices[3 * 7] = {
    	-0.5f, -0.5f, 0.0f, 0.8f, 0.2f, 0.8f, 1.0f,
    	 0.5f, -0.5f, 0.0f, 0.2f, 0.3f, 0.8f, 1.0f,
    	 0.0f,  0.5f, 0.0f, 0.8f, 0.8f, 0.2f, 1.0f
    };
    Hazel::Ref<Hazel::VertexBuffer> vertexBuffer = Hazel::VertexBuffer::Create(vertices, sizeof(vertices));
    Hazel::BufferLayout layout = {
        { Hazel::ShaderDataType::Float3, "a_Position" },
        { Hazel::ShaderDataType::Float4, "a_Color" }
    };
    vertexBuffer->SetLayout(layout);
    m_VertexArray->AddVertexBuffer(vertexBuffer);
    
    uint32_t indices[3] = { 0, 1, 2 };
    Hazel::Ref<Hazel::IndexBuffer> indexBuffer = Hazel::IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t));
    m_VertexArray->SetIndexBuffer(indexBuffer);

    // second set of VertexArray with Buffer
    m_SquareVertexArray = Hazel::VertexArray::Create();

    float squareVertices[5 * 4] = {
		-0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
		 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
		 0.5f,  0.5f, 0.0f, 1.0f, 1.0f,
		-0.5f,  0.5f, 0.0f, 0.0f, 1.0f
	};
    Hazel::Ref<Hazel::VertexBuffer> squareVertexBuffer = Hazel::VertexBuffer::Create(squareVertices, sizeof(squareVertices));
    squareVertexBuffer->SetLayout({
        { Hazel::ShaderDataType::Float3, "a_Position" },
        { Hazel::ShaderDataType::Float2, "a_TexCoord" }
    });
    m_SquareVertexArray->AddVertexBuffer(squareVertexBuffer);

    uint32_t squareIndices[6] = { 0, 1, 2, 2, 3, 0 };
    Hazel::Ref<Hazel::IndexBuffer> squareIndexBuffer = Hazel::IndexBuffer::Create(squareIndices, sizeof(squareIndices) / sizeof(uint32_t));
    m_SquareVertexArray->SetIndexBuffer(squareIndexBuffer);

    ////////////
    // Shader //
    ////////////

    std::string vertexSrc = R"(
        #version 330 core

        layout(location = 0) in vec3 a_Position;
        layout(location = 1) in vec4 a_Color;

        uniform mat4 u_ViewProjection;
        uniform mat4 u_Transform;

        out vec3 v_Position;
        out vec4 v_Color;

        void main() {
            v_Position = a_Position;
            v_Color = a_Color;
            gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);
        }
    )";

    std::string fragmentSrc = R"(
        #version 330 core

        layout(location = 0) out vec4 color;

        in vec3 v_Position;
        in vec4 v_Color;

        void main() {
    		color = vec4(v_Position * 0.5 + 0.5, 1.0);
            color = v_Color;
        }
    )";

    m_Shader = Hazel::Shader::Create("VertexPosColor", vertexSrc, fragmentSrc);

    // second set of Shader
    std::string flatColorShaderVertexSrc = R"(
        #version 330 core

        layout(location = 0) in vec3 a_Position;

        uniform mat4 u_ViewProjection;
        uniform mat4 u_Transform;

        out vec3 v_Position;

        void main() {
            v_Position = a_Position;
            gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);
        }
    )";

    std::string flatColorShaderFragmentSrc = R"(
        #version 330 core

        layout(location = 0) out vec4 color;

        in vec3 v_Position;

        uniform vec3 u_Color;

        void main() {
    		color = vec4(u_Color, 1.0);
        }
    )";

    m_FlatColorShader = Hazel::Shader::Create("FlatColor", flatColorShaderVertexSrc, flatColorShaderFragmentSrc);

    auto textureShader = m_ShaderLibrary.Load("assets/shaders/Texture.glsl");

	m_CheckerBoardTexture = Hazel::Texture2D::Create("assets/textures/Checkerboard.png");
	m_ChernoLogoTexture = Hazel::Texture2D::Create("assets/textures/ChernoLogo.png");

	textureShader->Bind();
	textureShader->SetInt("u_Texture", 0);
}

void ExampleLayer::OnAttach()
{
}

void ExampleLayer::OnDetach()
{
}

void ExampleLayer::OnUpdate(Hazel::Timestep dt) {
    // Update
    m_CameraController.OnUpdate(dt);

    // Render
    Hazel::RenderCommand::SetClearColor({ 0.9f, 1.0f, 0.75f, 1.0f });
    Hazel::RenderCommand::Clear();

    Hazel::Renderer::BeginScene(m_CameraController.GetCamera());

    m_FlatColorShader->Bind();
    m_FlatColorShader->SetFloat3("u_Color", m_SquareColor);
    
    glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f));
    for (int y = 0; y < 20; y++) {
        for (int x = 0; x < 20; x++) {
            glm::vec3 pos(x * 0.11f, y * 0.11f, 0.0f);
            glm::mat4 transform = glm::translate(glm::mat4(1.0f), pos) * scale;
            Hazel::Renderer::Submit(m_FlatColorShader, m_SquareVertexArray, transform);
        }
    }

    auto textureShader = m_ShaderLibrary.Get("Texture");

    m_CheckerBoardTexture->Bind();
    Hazel::Renderer::Submit(textureShader, m_SquareVertexArray, glm::scale(glm::mat4(1.0f), glm::vec3(1.5f)));

    m_ChernoLogoTexture->Bind();
    Hazel::Renderer::Submit(textureShader, m_SquareVertexArray, glm::scale(glm::mat4(1.0f), glm::vec3(1.5f)));

    // Hazel::Renderer::Submit(m_Shader, m_VertexArray);

    Hazel::Renderer::EndScene();
}

void ExampleLayer::OnImGuiRender() {
	ImGui::Begin("Settings");
	ImGui::ColorEdit3("Color Settings", glm::value_ptr(m_SquareColor));
	ImGui::End();
}

void ExampleLayer::OnEvent(Hazel::Event& e) {
    m_CameraController.OnEvent(e);
}
