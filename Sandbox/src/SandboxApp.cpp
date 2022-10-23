#include <Hazel.h>

#include "imgui/imgui.h"

class ExampleLayer : public Hazel::Layer {
public:
    ExampleLayer()
        : Layer("Example"), m_Camera(-1.6f, 1.6f, -0.9f, 0.9f)
    {
        /////////////////
        // VertexArray //
        /////////////////

        m_VertexArray.reset(Hazel::VertexArray::Create());

        ////////////
        // Buffer //
        ////////////

        float vertices[3 * 7] = {
	    	-0.5f, -0.5f, 0.0f, 0.8f, 0.2f, 0.8f, 1.0f,
	    	 0.5f, -0.5f, 0.0f, 0.2f, 0.3f, 0.8f, 1.0f,
	    	 0.0f,  0.5f, 0.0f, 0.8f, 0.8f, 0.2f, 1.0f
	    };
        std::shared_ptr<Hazel::VertexBuffer> vertexBuffer;
        vertexBuffer.reset(Hazel::VertexBuffer::Create(vertices, sizeof(vertices)));
        Hazel::BufferLayout layout = {
            { Hazel::ShaderDataType::Float3, "a_Position" },
            { Hazel::ShaderDataType::Float4, "a_Color" }
        };
        vertexBuffer->SetLayout(layout);
        m_VertexArray->AddVertexBuffer(vertexBuffer);
        
        uint32_t indices[3] = { 0, 1, 2 };
        std::shared_ptr<Hazel::IndexBuffer> indexBuffer;
        indexBuffer.reset(Hazel::IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t)));
        m_VertexArray->SetIndexBuffer(indexBuffer);

        // second set of VertexArray with Buffer
        m_SquareVertexArray.reset(Hazel::VertexArray::Create());

        float squareVertices[3 * 4] = {
	    	-0.75f, -0.75f, 0.0f,
	    	 0.75f, -0.75f, 0.0f,
	    	 0.75f,  0.75f, 0.0f,
	    	-0.75f,  0.75f, 0.0f
	    };
        std::shared_ptr<Hazel::VertexBuffer> squareVertexBuffer;
        squareVertexBuffer.reset(Hazel::VertexBuffer::Create(squareVertices, sizeof(squareVertices)));
        squareVertexBuffer->SetLayout({
            { Hazel::ShaderDataType::Float3, "a_Position" }
        });
        m_SquareVertexArray->AddVertexBuffer(squareVertexBuffer);

        uint32_t squareIndices[6] = { 0, 1, 2, 2, 3, 0 };
	    std::shared_ptr<Hazel::IndexBuffer> squareIndexBuffer;
	    squareIndexBuffer.reset(Hazel::IndexBuffer::Create(squareIndices, sizeof(squareIndices) / sizeof(uint32_t)));
	    m_SquareVertexArray->SetIndexBuffer(squareIndexBuffer);

        ////////////
        // Shader //
        ////////////

        std::string vertexSrc = R"(
            #version 330 core

            layout(location = 0) in vec3 a_Position;
            layout(location = 1) in vec4 a_Color;

            uniform mat4 u_ViewProjection;

            out vec3 v_Position;
            out vec4 v_Color;

            void main() {
                v_Position = a_Position;
                v_Color = a_Color;
                gl_Position = u_ViewProjection* vec4(a_Position, 1.0);
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

        m_Shader.reset(new Hazel::Shader(vertexSrc, fragmentSrc));

        std::string blueShaderVertexSrc = R"(
            #version 330 core

            layout(location = 0) in vec3 a_Position;

            uniform mat4 u_ViewProjection;

            out vec3 v_Position;

            void main() {
                v_Position = a_Position;
                gl_Position = u_ViewProjection * vec4(a_Position, 1.0);
            }
        )";

        std::string blueShaderFragmentSrc = R"(
            #version 330 core

            layout(location = 0) out vec4 color;

            in vec3 v_Position;

            void main() {
	    		color = vec4(0.2, 0.3, 0.8, 1.0);
            }
        )";

        m_BlueShader.reset(new Hazel::Shader(blueShaderVertexSrc, blueShaderFragmentSrc));
    }

    void OnUpdate() override {
        // if (Hazel::Input::IsKeyPressed(HZ_KEY_TAB)) {
        //     HZ_TRACE("Tab key is pressed (poll)!");
        // }

        Hazel::RenderCommand::SetClearColor({ 0.5f, 0.5f, 1.2f, 1 });
        Hazel::RenderCommand::Clear();

        m_Camera.SetPosition({ 0.5f, 0.5f, 0.0f });
        m_Camera.SetRotation(45.0f);

        Hazel::Renderer::BeginScene(m_Camera);

        Hazel::Renderer::Submit(m_BlueShader, m_SquareVertexArray);
        Hazel::Renderer::Submit(m_Shader, m_VertexArray);

        Hazel::Renderer::EndScene();
    }

    virtual void OnImGuiRender() override {
		// ImGui::Begin("Test");
		// ImGui::Text("Hello World");
		// ImGui::End();
	}

    void OnEvent(Hazel::Event& event) override {
        // if (event.GetEventType() == Hazel::EventType::KeyPressed) {
        //     Hazel::KeyPressedEvent& e = (Hazel::KeyPressedEvent&)event;
        //     if (e.GetKeyCode() == HZ_KEY_TAB) {
        //         HZ_TRACE("Tab key is pressed (event)!");
        //     }
        //     HZ_TRACE("{0}", (char)e.GetKeyCode());
        // }
    }

private:
    std::shared_ptr<Hazel::Shader> m_Shader;
    std::shared_ptr<Hazel::VertexArray> m_VertexArray;

    std::shared_ptr<Hazel::Shader> m_BlueShader;
    std::shared_ptr<Hazel::VertexArray> m_SquareVertexArray;

    Hazel::OrthographicCamera m_Camera;
};

class Sandbox : public Hazel::Application {
public:
    Sandbox() {
        PushLayer(new ExampleLayer());
    }

    ~Sandbox() {

    }
};

Hazel::Application* Hazel::CreateApplication() {
    return new Sandbox();
}