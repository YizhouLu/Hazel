#include "hzpch.h"
#include "Hazel/Renderer/OrthographicCameraController.h"

#include "Hazel/Core/Input.h"
#include "Hazel/Core/KeyCodes.h"

namespace Hazel {
OrthographicCameraController::OrthographicCameraController(float aspectRatio, bool rotation)
    : m_AspectRatio(aspectRatio), m_Camera(-m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel), m_Rotation(rotation) 
{}

void OrthographicCameraController::OnUpdate(Timestep dt)
{
    HZ_PROFILE_FUNCTION();

    if (Hazel::Input::IsKeyPressed(HZ_KEY_LEFT)) {
        m_CameraPosition.x -= m_CameraTranslationSpeed * dt;
    } else if (Hazel::Input::IsKeyPressed(HZ_KEY_RIGHT)) {
        m_CameraPosition.x += m_CameraTranslationSpeed * dt;
    }
    
    if (Hazel::Input::IsKeyPressed(HZ_KEY_DOWN)) {
        m_CameraPosition.y -= m_CameraTranslationSpeed * dt;
    } else if (Hazel::Input::IsKeyPressed(HZ_KEY_UP)) {
        m_CameraPosition.y += m_CameraTranslationSpeed * dt;
    }
    
    if (m_Rotation) {
        if (Hazel::Input::IsKeyPressed(HZ_KEY_A)) {
            m_CameraRotation += m_CameraRotationSpeed * dt;
        } else if (Hazel::Input::IsKeyPressed(HZ_KEY_D)) {
            m_CameraRotation -= m_CameraRotationSpeed * dt;
        }

        m_Camera.SetRotation(m_CameraRotation);
    }

    if (Hazel::Input::IsKeyPressed(HZ_KEY_R)) {
        m_CameraPosition = glm::vec3(0.0f);
        m_CameraRotation = 0.0f;
    }

    m_Camera.SetPosition(m_CameraPosition);

    m_CameraTranslationSpeed = m_ZoomLevel;
}

void OrthographicCameraController::OnEvent(Event& e)
{
    HZ_PROFILE_FUNCTION();

    EventDispatcher dispatcher(e);
    dispatcher.Dispatch<MouseScrolledEvent>(HZ_BIND_EVENT_FN(OrthographicCameraController::OnMouseScrolled));
    dispatcher.Dispatch<WindowResizeEvent>(HZ_BIND_EVENT_FN(OrthographicCameraController::OnWindowResized));
}

void OrthographicCameraController::OnResize(float width, float height)
{
    m_AspectRatio = width / height;
    m_Camera.SetProjection(-m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel);
}

bool OrthographicCameraController::OnMouseScrolled(MouseScrolledEvent& e)
{
    HZ_PROFILE_FUNCTION();

    m_ZoomLevel -= e.GetYOffset() * 0.25f;
    m_ZoomLevel = std::max(m_ZoomLevel, 0.25f);
    m_Camera.SetProjection(-m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel);
    return false;
}

bool OrthographicCameraController::OnWindowResized(WindowResizeEvent& e)
{
    HZ_PROFILE_FUNCTION();

    OnResize((float)e.GetWidth(), (float)e.GetHeight());
	return false;
}
}
