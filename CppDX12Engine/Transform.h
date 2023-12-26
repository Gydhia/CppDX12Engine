#pragma once

#include <DirectXMath.h>

using namespace DirectX;

class Transform
{
private:
    XMFLOAT3 vDir;      // Axis Z
    XMFLOAT3 vUp;       // Axis Y
    XMFLOAT3 vRight;    // Axis X

    XMFLOAT3 vPos;       // Position
    XMFLOAT4 qRot;       // Rotation (as a quaternion)
    XMFLOAT3 vScale;     // Scale

    XMMATRIX mPos;       // Position matrix
    XMMATRIX mRot;       // Rotation matrix
    XMMATRIX mScale;     // Scale matrix

    XMMATRIX matrix;     // Transformation matrix

    void UpdateMatrix();          // Update the matrix after a change
    void UpdateRotationFromQuaternion();
    void UpdateRotationfromMatrix();

    void RotateYaw(float angle);
    void RotatePitch(float angle);
    void RotateRoll(float angle);

public:
    Transform();
    void Update() {};
    XMMATRIX* GetmRot() { return &mRot; }
    XMMATRIX* GetmPos() { return &mPos; }
    XMMATRIX* GetmScale() { return &mScale; }
    XMMATRIX* GetMatrix() { return &matrix; }

    XMFLOAT3* GetVPos() { return &vPos; }
    void SetVPos(DirectX::XMFLOAT3 newPos) { vPos = newPos; }
    XMFLOAT4* GetQRot() { return &qRot; }
    XMFLOAT3* GetVScale() { return &vScale; }

    XMFLOAT3* GetVDir() { return &vDir; }
    XMFLOAT3* GetVUp() { return &vUp; }
    XMFLOAT3* GetVRight() { return &vRight; }

    void Copy(Transform* toCopy);

    void TranslateByPos(float x, float y, float z);
    void TranslateByVector(DirectX::XMFLOAT3 pos);

    void ScaleUniform(float scale);
    void Scale(float scaleX, float scaleY, float scaleZ);
    void Move(float dist);
};
