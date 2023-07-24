#include "Camera.h"

#include <chrono>
#include <algorithm>

Camera::Camera()
{
    RegisterOnKeyHold([this](char KeyCode) 
    {
        this->OnKeyHold(KeyCode);
    });
	RegisterOnMouseMove([this](int dx, int dy)
	{
        this->OnMouseMove(dx,dy);
	});
}

void Camera::OnMouseMove(int dx, int dy)
{
	camPitch+= -0.004f * static_cast<float>(dy);
	camYaw+= -0.004f * static_cast<float>(dx);
}

void Camera::Update(float nDeltaTime)
{
	//Clamp pitch
	camPitch = std::clamp(camPitch, -1.50f, 1.50f);

	const auto camRotationMatrix = XMMatrixRotationRollPitchYaw(camPitch, camYaw, camRoll);
	cameraTarget = XMVector3Normalize(XMVector3TransformCoord(DefaultForward, camRotationMatrix ));

	const XMMATRIX RotateYTempMatrix = XMMatrixRotationY(camYaw);
    //TODO: Support camera roll!
	//XMMATRIX RotateZTempMatrix = XMMatrixRotationZ(camRoll);

	const XMMATRIX RotatePTempMatrix = XMMatrixRotationRollPitchYaw(camPitch, camYaw, 0);
	
	camRight = XMVector3TransformCoord(DefaultRight, RotatePTempMatrix);
	camUp = XMVector3TransformCoord(camUp, RotateYTempMatrix );
	camForward = XMVector3TransformCoord(DefaultForward, RotatePTempMatrix);
	const auto camPosition = XMVectorSet( m_Transform.GetPosition().x, m_Transform.GetPosition().y, m_Transform.GetPosition().z, 0.0f );
	
	cameraTarget = camPosition + cameraTarget;

	const auto temp = XMMatrixLookAtLH( camPosition, cameraTarget, camUp );
	XMStoreFloat4x4(&cameraViewMat, temp);
}

void Camera::SetFOV(float FOV)
{
    if(FOV == 0.f)
    {
        return;
    }
    m_FOV = FOV;
    // Calculate the projection matrix using a perspective field of view (FOV).

    // This is converted to radians by multiplying with the factor (3.14 / 180).
    // The aspect ratio is calculated by dividing the window width by the window height.
    // The near clipping plane is set to 0.1f and the far clipping plane is set to 1000.0f.
    const XMMATRIX tmpMat = XMMatrixPerspectiveFovLH(XMConvertToRadians(FOV), (float)Window::GetInstance().Width / (float)Window::GetInstance().Height, 0.1f, 1000.0f);

    // Store the resulting projection matrix into the camera's projection matrix.
    // XMStoreFloat4x4 is used to convert the XMMATRIX into a 4x4 float matrix.
    XMStoreFloat4x4(&cameraProjMat, tmpMat);
}

void Camera::Initialize()
{
   m_Transform.GetPosition() = XMFLOAT4(0.0f, 0.f, -0.f, 0.0f);
   cameraUp = XMFLOAT4(0.0f, 1.0f, 0.0f, 0.0f);
}

void Camera::OnKeyHold(char Code)
{
    // Load the camera position vector
    XMVECTOR posVec = XMLoadFloat4(&m_Transform.GetPosition());

    // Define move and rotate speeds as variables
    const float moveSpeed = 0.1f;
    const float rotateSpeed = 0.05f;

    // Use a switch statement to handle the different key codes
    switch (Code)
    {
    case 'W':
        // Move forward along the camera's forward vector
        posVec += camForward * moveSpeed;
        break;
    case 'S':
        // Move backward along the camera's forward vector
        posVec += camForward * -moveSpeed;
        break;
    case 'A':
        // Move left along the camera's right vector
        posVec += camRight * -moveSpeed;
        break;
    case 'D':
        // Move right along the camera's right vector
        posVec += camRight * moveSpeed;
        break;
    case 'Q':
        // Rotate the camera around its forward vector (roll)
        camRoll += rotateSpeed;
        break;
    case 'E':
        // Rotate the camera around its forward vector (roll)
        camRoll -= rotateSpeed;
        break;
    case VK_UP:
        // Rotate the camera around its right vector (pitch)
        camPitch += rotateSpeed;
        break;
    case VK_DOWN:
        // Rotate the camera around its right vector (pitch)
        camPitch -= rotateSpeed;
        break;
    case VK_LEFT:
        // Rotate the camera around its up vector (yaw)
        camYaw -= rotateSpeed;
        break;
    case VK_RIGHT:
        // Rotate the camera around its up vector (yaw)
        camYaw += rotateSpeed;
        break;
    default:
        return;
    }

    // Store the updated camera position vector
    XMStoreFloat4(&m_Transform.GetPosition(), posVec);
}
