#include "Transform.h"

#include <DirectXMath.h>

using namespace DirectX;

Transform::Transform()
{
	// init the axis
	vDir = XMFLOAT3(0.0f, 0.0f, 1.0f);
	vUp = XMFLOAT3(0.0f, 1.0f, 0.0f);
	vRight = XMFLOAT3(1.0f, 0.0f, 0.0f);

	vPos = XMFLOAT3(0.0f, 0.0f, 0.0f);
	vScale = XMFLOAT3(1.0f, 1.0f, 1.0f);

	qRot = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f); // Identity quaternion

	// init the different matrix
	mPos = XMMatrixIdentity();
	mRot = XMMatrixIdentity();
	mScale = XMMatrixIdentity();
	matrix = mScale * mRot * mPos;
}


void Transform::UpdateMatrix()
{
	matrix = mScale * mRot * mPos;
}

void Transform::RotateYaw(float angle)
{
	 XMMATRIX tmpQuatMat;
    tmpQuatMat = XMMatrixRotationAxis(XMLoadFloat3(&vUp), angle);
    XMVECTOR tmpQuat = XMQuaternionRotationMatrix(tmpQuatMat);
    XMVECTOR currentQuat = XMLoadFloat4(&qRot);
    XMStoreFloat4(&qRot, XMQuaternionMultiply(currentQuat, tmpQuat));
}

void Transform::RotatePitch(float angle)
{
	XMMATRIX tmpQuatMat;
	tmpQuatMat = XMMatrixRotationAxis(XMLoadFloat3(&vRight), angle);
	XMVECTOR tmpQuat = XMQuaternionRotationMatrix(tmpQuatMat);
	XMVECTOR currentQuat = XMLoadFloat4(&qRot);
	XMStoreFloat4(&qRot, XMQuaternionMultiply(currentQuat, tmpQuat));
}

void Transform::RotateRoll(float angle)
{
	XMMATRIX tmpQuatMat;
	tmpQuatMat = XMMatrixRotationAxis(XMLoadFloat3(&vDir), angle);
	XMVECTOR tmpQuat = XMQuaternionRotationMatrix(tmpQuatMat);
	XMVECTOR currentQuat = XMLoadFloat4(&qRot);
	XMStoreFloat4(&qRot, XMQuaternionMultiply(currentQuat, tmpQuat));
}

void Transform::UpdateRotationFromQuaternion()
{
	XMVECTOR quat = XMLoadFloat4(&qRot);
	mRot = XMMatrixRotationQuaternion(quat);
	UpdateRotationfromMatrix();
}

void Transform::UpdateRotationfromMatrix()
{
	XMStoreFloat3(&vRight, mRot.r[0]);
	XMStoreFloat3(&vUp, mRot.r[1]);
	XMStoreFloat3(&vDir, mRot.r[2]);
}

#pragma endregion

#pragma region Translation
void Transform::TranslateByPos(float x, float y, float z)
{
	XMMATRIX translation = XMMatrixTranslation(x, y, z);
	mPos *= translation;

	vPos.x += x;
	vPos.y += y;
	vPos.z += z;

	UpdateMatrix();
}
void Transform::TranslateByVector(XMFLOAT3 pos)
{
	XMMATRIX translation = XMMatrixTranslation(pos.x, pos.y, pos.z);
	mPos *= translation;

	vPos.x += pos.x;
	vPos.y += pos.y;
	vPos.z += pos.z;

	UpdateMatrix();
}

#pragma endregion

#pragma region Scale
void Transform::ScaleUniform(float scale)
{
	XMMATRIX scaling = XMMatrixScaling(scale, scale, scale);
	mScale *= scaling;

	vScale.x *= scale;
	vScale.y *= scale;
	vScale.z *= scale;

	UpdateMatrix();
}

void Transform::Scale(float scaleX, float scaleY, float scaleZ)
{
	XMMATRIX scaling = XMMatrixScaling(scaleX, scaleY, scaleZ);
	mScale *= scaling;

	vScale.x *= scaleX;
	vScale.y *= scaleY;
	vScale.z *= scaleZ;

	UpdateMatrix();
}
#pragma endregion

void Transform::Copy(Transform* toCopy) {
	vDir = *toCopy->GetVDir();
	vUp = *toCopy->GetVUp();
	vRight = *toCopy->GetVRight();

	vPos = *toCopy->GetVPos();
	qRot = *toCopy->GetQRot();
	vScale = *toCopy->GetVScale();

	mPos = *toCopy->GetmPos();
	mRot = *toCopy->GetmRot();
	mScale = *toCopy->GetmScale();
	matrix = *toCopy->GetMatrix();
}

void Transform::Move(float dist)
{
	vPos.x += vDir.x * dist;
	vPos.y += vDir.y * dist;
	vPos.z += vDir.z * dist;

	XMMATRIX translation = XMMatrixTranslation(vPos.x, vPos.y, vPos.z);
	mPos = translation;

	UpdateMatrix();
}