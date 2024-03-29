#include "EditorLayer.h"
#include <imgui/imgui.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Hazel {
EditorLayer::EditorLayer()
    : Layer("EditorLayer"), m_CameraController(1280.0f / 720.0f)
{
}

void EditorLayer::OnAttach()
{
    HZ_PROFILE_FUNCTION();

    m_CheckerBoardTexture = Texture2D::Create("assets/textures/Checkerboard.png");

    FrameBufferSpecification fbSpec;
    fbSpec.Width = 1280;
    fbSpec.Height = 720;
    m_FrameBuffer = FrameBuffer::Create(fbSpec);

    m_ActiveScene = CreateRef<Scene>();

    // Entity
    auto square = m_ActiveScene->CreateEntity("Green Square");    
    square.AddComponent<SpriteRendererComponent>(glm::vec4{ 0.0f, 1.0f, 0.0f, 1.0f });
    
    auto square2 = m_ActiveScene->CreateEntity("Red Square");
    square2.AddComponent<SpriteRendererComponent>(glm::vec4{ 1.0f, 0.0f, 0.0f, 1.0f });

    m_SquareEntity = square;

    m_CameraEntity = m_ActiveScene->CreateEntity("Camera A");
    m_CameraEntity.AddComponent<CameraComponent>();

    m_SecondCamera = m_ActiveScene->CreateEntity("Camera B");
    auto& cc = m_SecondCamera.AddComponent<CameraComponent>();
    cc.Primary = false;


    class CameraController : public ScriptableEntity
    {
    public:
        virtual void OnCreate() override {
            auto& translation = GetComponent<TransformComponent>().Translation;
            translation.x = rand() % 10 - 5.0f;
        }

        virtual void OnDestroy() override {

        }

        virtual void OnUpdate(Timestep dt) override {
            auto& translation = GetComponent<TransformComponent>().Translation;

            float speed = 5.0f;

            if (Input::IsKeyPressed(HZ_KEY_LEFT)) {
                translation.x -= speed * dt;
            }
            else if (Input::IsKeyPressed(HZ_KEY_RIGHT)) {
                translation.x += speed * dt;
            }

            if (Input::IsKeyPressed(HZ_KEY_DOWN)) {
                translation.y -= speed * dt;
            }
            else if (Input::IsKeyPressed(HZ_KEY_UP)) {
                translation.y += speed * dt;
            }
        }

    };

    m_CameraEntity.AddComponent<NativeScriptComponent>().Bind<CameraController>();
    m_SecondCamera.AddComponent<NativeScriptComponent>().Bind<CameraController>();

    m_SceneHierarchyPanel.SetContext(m_ActiveScene);
}

void EditorLayer::OnDetach()
{
    HZ_PROFILE_FUNCTION();
}

void EditorLayer::OnUpdate(Timestep dt)
{
    HZ_PROFILE_FUNCTION();

	// Resize
	if (FrameBufferSpecification spec = m_FrameBuffer->GetSpecification();
		m_ViewportSize.x > 0.0f && m_ViewportSize.y > 0.0f && // zero sized framebuffer is invalid
		(spec.Width != m_ViewportSize.x || spec.Height != m_ViewportSize.y)) {
		m_FrameBuffer->Resize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
		m_CameraController.OnResize(m_ViewportSize.x, m_ViewportSize.y);

        m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
	}

    // Update
    if (m_ViewportFocused) {
        m_CameraController.OnUpdate(dt);
    }

    // Render
    Renderer2D::ResetStats();
    m_FrameBuffer->Bind();
    RenderCommand::SetClearColor({ 167.0f / 255.0f, 239.0f / 255.0f, 228.0f / 255.0f, 1.0f });
    RenderCommand::Clear();

    // Update scene
    m_ActiveScene->OnUpdate(dt);

    m_FrameBuffer->Unbind();
}

void EditorLayer::OnImGuiRender()
{
    HZ_PROFILE_FUNCTION();

    static bool dockspaceOpen = true;
    static bool opt_fullscreen_persistant = true;
    bool opt_fullscreen = opt_fullscreen_persistant;
    static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;
    
    // We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
    // because it would be confusing to have two docking targets within each others.
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
    if (opt_fullscreen)
    {
        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->WorkPos);
        ImGui::SetNextWindowSize(viewport->WorkSize);
        ImGui::SetNextWindowViewport(viewport->ID);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
        window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
    }
    
    // When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background
    // and handle the pass-thru hole, so we ask Begin() to not render a background.
    if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode) {
        window_flags |= ImGuiWindowFlags_NoBackground;
    }
    
    // Important: note that we proceed even if Begin() returns false (aka window is collapsed).
    // This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
    // all active windows docked into it will lose their parent and become undocked.
    // We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
    // any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("DockSpace Demo", &dockspaceOpen, window_flags);
    ImGui::PopStyleVar();
    
    if (opt_fullscreen) {
        ImGui::PopStyleVar(2);
    }
    
    // DockSpace
    ImGuiIO& io = ImGui::GetIO();
    if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable) {
        ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
        ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
    }
    
    if (ImGui::BeginMenuBar()) {
        if (ImGui::BeginMenu("File")) {

			// Disabling full screen would allow the window to be moved to the front of other windows, 
			// which we can't undo at the moment without finer window depth/z control.
			//ImGui::MenuItem("Fullscreen", NULL, &opt_fullscreen_persistant);
            
            if (ImGui::MenuItem("Exit")) {
                Application::Get().Close();
            }
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }

    m_SceneHierarchyPanel.OnImGuiRender();
    
    ImGui::Begin("Stats");
    
    auto stats = Renderer2D::GetStats();
    ImGui::Text("Renderer2D Stats:");
    ImGui::Text("Draw Calls: %d", stats.DrawCalls);
    ImGui::Text("Quads: %d", stats.QuadCount);
    ImGui::Text("Vertices: %d", stats.GetTotalVertexCount());
    ImGui::Text("Indices: %d", stats.GetTotalIndexCount());
    
    ImGui::End();
    
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0,0 });
    ImGui::Begin("Viewport");

    m_ViewportFocused = ImGui::IsWindowFocused();
    m_ViewportHovered = ImGui::IsWindowHovered();
    Application::Get().GetImGuiLayer()->BlockEvents(!m_ViewportFocused || !m_ViewportHovered);

    ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
    m_ViewportSize = { viewportPanelSize.x, viewportPanelSize.y };

    uint32_t textureID = m_FrameBuffer->GetColorAttachmentRendererID();
    ImGui::Image((void*)textureID, ImVec2{ m_ViewportSize.x, m_ViewportSize.y }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });
    ImGui::End();
    ImGui::PopStyleVar();
    
    ImGui::End();
}

void EditorLayer::OnEvent(Event& e)
{
    m_CameraController.OnEvent(e);
}
}