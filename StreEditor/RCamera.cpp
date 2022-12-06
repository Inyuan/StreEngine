#include "RCamera.h"
#include <memory>
using namespace DirectX;

DirectX::XMFLOAT4X4 Identity4x4()
{
	static DirectX::XMFLOAT4X4 I(
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f);

	return I;
}

RCamera::RCamera()
{
	SetLens(0.25f * 3.1415926535f, 1.0f, 1.0f, 1000.0f);
	UpdateViewMatrix();
}

RCamera::~RCamera()
{
}

void RCamera::convert_to_camera_data(void * out_s_camera)
{
	struct
	{
		DirectX::XMFLOAT4X4 View = Identity4x4();
		DirectX::XMFLOAT4X4 InvView = Identity4x4();
		DirectX::XMFLOAT4X4 Proj = Identity4x4();
		DirectX::XMFLOAT4X4 InvProj = Identity4x4();
		DirectX::XMFLOAT4X4 ViewProj = Identity4x4();
		DirectX::XMFLOAT4X4 InvViewProj = Identity4x4();
		DirectX::XMFLOAT4X4 ShadowTransform = Identity4x4();
		DirectX::XMFLOAT3 EyePosW = { 0.0f, 0.0f, 0.0f };
		DirectX::XMFLOAT2 RenderTargetSize = { 0.0f, 0.0f };
		DirectX::XMFLOAT2 InvRenderTargetSize = { 0.0f, 0.0f };
		float NearZ = 0.0f;
		float FarZ = 0.0f;
		DirectX::XMFLOAT4 AmbientLight = { 0.0f, 0.0f, 0.0f, 1.0f };
	}direct_camera_data;

	
	//???强制转换？ 
	//摄像机局部坐标系 
	XMMATRIX view = GetView();
	//投影变换
	XMMATRIX proj = GetProj();

	//投影变换矩阵 世界到屏幕空间
	XMMATRIX viewProj = XMMatrixMultiply(view, proj);

	//
	XMVECTOR matrixview = XMMatrixDeterminant(view);
	XMVECTOR matrixproj = XMMatrixDeterminant(proj);
	XMVECTOR matrixviewproj = XMMatrixDeterminant(viewProj);
	//逆矩阵
	XMMATRIX invView = XMMatrixInverse(&matrixview, view);
	XMMATRIX invProj = XMMatrixInverse(&matrixproj, proj);
	XMMATRIX invViewProj = XMMatrixInverse(&matrixviewproj, viewProj);

	//???Shadow
	//RLight::LightNFWVPT lightNFWVPT;
	//((RLight*)IOGPUScene->Lightgroup[0])->GetLightMatrix(IOGPUScene->GetSceneBounds(), lightNFWVPT);
	//XMMATRIX shadowTransform = XMLoadFloat4x4(&lightNFWVPT.mShadowTransform);

	//
	XMStoreFloat4x4(&direct_camera_data.View, XMMatrixTranspose(view));
	XMStoreFloat4x4(&direct_camera_data.InvView, XMMatrixTranspose(invView));
	XMStoreFloat4x4(&direct_camera_data.Proj, XMMatrixTranspose(proj));
	XMStoreFloat4x4(&direct_camera_data.InvProj, XMMatrixTranspose(invProj));
	XMStoreFloat4x4(&direct_camera_data.ViewProj, XMMatrixTranspose(viewProj));
	XMStoreFloat4x4(&direct_camera_data.InvViewProj, XMMatrixTranspose(invViewProj));
	//XMStoreFloat4x4(&direct_camera_data.ShadowTransform, XMMatrixTranspose(shadowTransform));


	//???强制转换？
	direct_camera_data.EyePosW = GetPosition3f();
	//Scenecontants.RenderTargetSize = XMFLOAT2((float)d3dApp->mClientWidth, (float)d3dApp->mClientHeight);
	//Scenecontants.InvRenderTargetSize = XMFLOAT2(1.0f / d3dApp->mClientWidth, 1.0f / d3dApp->mClientHeight);
	direct_camera_data.NearZ = mNearZ;
	direct_camera_data.FarZ = mFarZ;
	//should be fixed in lightPass
	direct_camera_data.AmbientLight = { 0.25f, 0.25f, 0.35f, 1.0f };

	//检查一下大小
	memcpy(out_s_camera, &direct_camera_data,sizeof(direct_camera_data));
}

XMVECTOR RCamera::GetPosition()const
{
	return XMLoadFloat3(&mPosition);
}

XMFLOAT3 RCamera::GetPosition3f()const
{
	return mPosition;
}

void RCamera::SetPosition(float x, float y, float z)
{
	mPosition = XMFLOAT3(x, y, z);
	mViewDirty = true;
}

void RCamera::SetPosition(const XMFLOAT3& v)
{
	mPosition = v;
	mViewDirty = true;
}

XMVECTOR RCamera::GetRight()const
{
	return XMLoadFloat3(&mRight);
}

XMFLOAT3 RCamera::GetRight3f()const
{
	return mRight;
}

XMVECTOR RCamera::GetUp()const
{
	return XMLoadFloat3(&mUp);
}

XMFLOAT3 RCamera::GetUp3f()const
{
	return mUp;
}

XMVECTOR RCamera::GetLook()const
{
	return XMLoadFloat3(&mLook);
}

XMFLOAT3 RCamera::GetLook3f()const
{
	return mLook;
}

float RCamera::GetNearZ()const
{
	return mNearZ;
}

float RCamera::GetFarZ()const
{
	return mFarZ;
}

float RCamera::GetAspect()const
{
	return mAspect;
}

float RCamera::GetFovY()const
{
	return mFovY;
}

float RCamera::GetFovX()const
{
	float halfWidth = 0.5f * GetNearWindowWidth();
	return 2.0f * atan(halfWidth / mNearZ);
}

float RCamera::GetNearWindowWidth()const
{
	return mAspect * mNearWindowHeight;
}

float RCamera::GetNearWindowHeight()const
{
	return mNearWindowHeight;
}

float RCamera::GetFarWindowWidth()const
{
	return mAspect * mFarWindowHeight;
}

float RCamera::GetFarWindowHeight()const
{
	return mFarWindowHeight;
}

void RCamera::SetLens(float fovY, float aspect, float zn, float zf)
{
	// cache properties
	mFovY = fovY;
	mAspect = aspect;
	mNearZ = zn;
	mFarZ = zf;

	mNearWindowHeight = 2.0f * mNearZ * tanf(0.5f * mFovY);
	mFarWindowHeight = 2.0f * mFarZ * tanf(0.5f * mFovY);

	XMMATRIX P = XMMatrixPerspectiveFovLH(mFovY, mAspect, mNearZ, mFarZ);
	XMStoreFloat4x4(&mProj, P);
}

void RCamera::LookAt(FXMVECTOR pos, FXMVECTOR target, FXMVECTOR worldUp)
{
	XMVECTOR L = XMVector3Normalize(XMVectorSubtract(target, pos));
	XMVECTOR R = XMVector3Normalize(XMVector3Cross(worldUp, L));
	XMVECTOR U = XMVector3Cross(L, R);

	XMStoreFloat3(&mPosition, pos);
	XMStoreFloat3(&mLook, L);
	XMStoreFloat3(&mRight, R);
	XMStoreFloat3(&mUp, U);

	mViewDirty = true;
}

void RCamera::LookAt(const XMFLOAT3& pos, const XMFLOAT3& target, const XMFLOAT3& up)
{
	XMVECTOR P = XMLoadFloat3(&pos);
	XMVECTOR T = XMLoadFloat3(&target);
	XMVECTOR U = XMLoadFloat3(&up);

	LookAt(P, T, U);

	mViewDirty = true;
}

XMMATRIX RCamera::GetView()const
{
	//assert(!mViewDirty);
	return XMLoadFloat4x4(&mView);
}

XMMATRIX RCamera::GetProj()const
{
	return XMLoadFloat4x4(&mProj);
}


XMFLOAT4X4 RCamera::GetView4x4f()const
{
	//assert(!mViewDirty);
	return mView;
}

XMFLOAT4X4 RCamera::GetProj4x4f()const
{
	return mProj;
}

void RCamera::Strafe(float d)
{
	// mPosition += d*mRight
	XMVECTOR s = XMVectorReplicate(d);
	XMVECTOR r = XMLoadFloat3(&mRight);
	XMVECTOR p = XMLoadFloat3(&mPosition);
	XMStoreFloat3(&mPosition, XMVectorMultiplyAdd(s, r, p));

	mViewDirty = true;
}

void RCamera::Walk(float d)
{
	// mPosition += d*mLook
	XMVECTOR s = XMVectorReplicate(d);
	XMVECTOR l = XMLoadFloat3(&mLook);
	XMVECTOR p = XMLoadFloat3(&mPosition);
	XMStoreFloat3(&mPosition, XMVectorMultiplyAdd(s, l, p));

	mViewDirty = true;
}

void RCamera::Pitch(float angle)
{
	// Rotate up and look vector about the right vector.

	XMMATRIX R = XMMatrixRotationAxis(XMLoadFloat3(&mRight), angle);

	XMStoreFloat3(&mUp, XMVector3TransformNormal(XMLoadFloat3(&mUp), R));
	XMStoreFloat3(&mLook, XMVector3TransformNormal(XMLoadFloat3(&mLook), R));

	mViewDirty = true;
}

void RCamera::RotateY(float angle)
{
	// Rotate the basis vectors about the world y-axis.

	XMMATRIX R = XMMatrixRotationY(angle);

	XMStoreFloat3(&mRight, XMVector3TransformNormal(XMLoadFloat3(&mRight), R));
	XMStoreFloat3(&mUp, XMVector3TransformNormal(XMLoadFloat3(&mUp), R));
	XMStoreFloat3(&mLook, XMVector3TransformNormal(XMLoadFloat3(&mLook), R));

	mViewDirty = true;
}

void RCamera::UpdateViewMatrix()
{
	if (mViewDirty)
	{
		XMVECTOR R = XMLoadFloat3(&mRight);
		XMVECTOR U = XMLoadFloat3(&mUp);
		XMVECTOR L = XMLoadFloat3(&mLook);
		XMVECTOR P = XMLoadFloat3(&mPosition);

		// Keep camera's axes orthogonal to each other and of unit length.
		L = XMVector3Normalize(L);
		U = XMVector3Normalize(XMVector3Cross(L, R));

		// U, L already ortho-normal, so no need to normalize cross product.
		R = XMVector3Cross(U, L);

		// Fill in the view matrix entries.
		float x = -XMVectorGetX(XMVector3Dot(P, R));
		float x_debug = XMVectorGetX(P) * XMVectorGetX(R) +
						XMVectorGetY(P) * XMVectorGetY(R) +
						XMVectorGetZ(P) * XMVectorGetZ(R);
		float y = -XMVectorGetX(XMVector3Dot(P, U));
		float z = -XMVectorGetX(XMVector3Dot(P, L));

		XMStoreFloat3(&mRight, R);
		XMStoreFloat3(&mUp, U);
		XMStoreFloat3(&mLook, L);

		mView(0, 0) = mRight.x;
		mView(1, 0) = mRight.y;
		mView(2, 0) = mRight.z;
		mView(3, 0) = x;

		mView(0, 1) = mUp.x;
		mView(1, 1) = mUp.y;
		mView(2, 1) = mUp.z;
		mView(3, 1) = y;

		mView(0, 2) = mLook.x;
		mView(1, 2) = mLook.y;
		mView(2, 2) = mLook.z;
		mView(3, 2) = z;

		mView(0, 3) = 0.0f;
		mView(1, 3) = 0.0f;
		mView(2, 3) = 0.0f;
		mView(3, 3) = 1.0f;

		mViewDirty = false;
	}
}



