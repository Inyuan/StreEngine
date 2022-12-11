#pragma once 
#include <DirectXMath.h>
#pragma comment(lib,"d3dcompiler.lib")
#pragma comment(lib, "D3D12.lib")
#pragma comment(lib, "dxgi.lib")

//Window的API更快
class RCamera
{
public:

	RCamera();
	~RCamera();

	// Get/Set world camera position.
	DirectX::XMVECTOR GetPosition()const;
	DirectX::XMFLOAT3 GetPosition3f()const;
	void SetPosition(float x, float y, float z);
	void SetPosition(const DirectX::XMFLOAT3& v);

	void convert_to_camera_data(void* out_s_camera);

	// Get camera basis vectors.
	DirectX::XMVECTOR GetRight()const;
	DirectX::XMFLOAT3 GetRight3f()const;
	DirectX::XMVECTOR GetUp()const;
	DirectX::XMFLOAT3 GetUp3f()const;
	DirectX::XMVECTOR GetLook()const;
	DirectX::XMFLOAT3 GetLook3f()const;

	// Get frustum properties.
	float GetNearZ()const;
	float GetFarZ()const;
	float GetAspect()const;
	float GetFovY()const;
	float GetFovX()const;

	// Get near and far plane dimensions in view space coordinates.
	float GetNearWindowWidth()const;
	float GetNearWindowHeight()const;
	float GetFarWindowWidth()const;
	float GetFarWindowHeight()const;

	// Set frustum.
	void SetLens(float fovY, float aspect, float zn, float zf);

	// Define camera space via LookAt parameters.
	void LookAt(DirectX::FXMVECTOR pos, DirectX::FXMVECTOR target, DirectX::FXMVECTOR worldUp);
	void LookAt(const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT3& target, const DirectX::XMFLOAT3& up);

	// Get View/Proj matrices.
	DirectX::XMMATRIX GetView()const;
	DirectX::XMMATRIX GetProj()const;

	DirectX::XMFLOAT4X4 GetView4x4f()const;
	DirectX::XMFLOAT4X4 GetProj4x4f()const;

	// Strafe/Walk the camera a distance d.
	void Strafe(float d);
	void Walk(float d);

	// Rotate the camera.
	void Pitch(float angle);
	void RotateY(float angle);

	// After modifying camera position/orientation, call to rebuild the view matrix.
	void UpdateViewMatrix();

	// Camera coordinate system with coordinates relative to world space.
	DirectX::XMFLOAT3 mPosition = { 0.0f, 0.0f, 0.0f };
	DirectX::XMFLOAT3 mRight = { 1.0f, 0.0f, 0.0f };
	DirectX::XMFLOAT3 mUp = { 0.0f, 1.0f, 0.0f };
	DirectX::XMFLOAT3 mLook = { 0.0f, 0.0f, 1.0f };

	// Cache frustum properties.
	float mNearZ = 0.0f;
	float mFarZ = 0.0f;
	float mAspect = 0.0f;
	float mFovY = 0.0f;
	float mNearWindowHeight = 0.0f;
	float mFarWindowHeight = 0.0f;

	bool mViewDirty = true;

	// Cache View/Proj matrices.
	DirectX::XMFLOAT4X4 mView = DirectX::XMFLOAT4X4(
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f);
	DirectX::XMFLOAT4X4 mProj = DirectX::XMFLOAT4X4(
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f);


};

//Window的API更快
class RObject
{
public:
	void UpdateWorldMatrix();

	void convert_to_object_contants_data(void* out_s_object_contants);

	float Transform[3] = { 0,0,0 };
	float Rotation[3] = { 0,0,0 };
	float Scale[3] = { 1.0,1.0,1.0 };
	// Cache View/Proj matrices.
	DirectX::XMFLOAT4X4 WorldMatrix = DirectX::XMFLOAT4X4(
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f);
};

//自己写一个 但利用window的APi计算应该更快
//class simple_camera
//{
//public:
//
//	/// <summary>
//	/// 构建左手透视投影矩阵 ???
//	/// </summary>
//	/// <param name="in_fov"></param>
//	/// <param name="in_aspect"></param>
//	/// <param name="in_near"></param>
//	/// <param name="in_far"></param>
//	/// <param name="out_mret"></param>
//	static void perspective_fov(float in_fov, float in_aspect, float in_near, float in_far, s_float4x4& out_mret)
//	{
//		float D2R = 3.1415926 / 180.0;
//		float yScale = 1.0 / tan(D2R * in_fov / 2);
//		float xScale = yScale / in_aspect;
//		float nearmfar = in_near - in_far;
//		float m[] = {
//			xScale, 0, 0, 0,
//			0, yScale, 0, 0,
//			0, 0, (in_far + in_near) / nearmfar, -1,
//			0, 0, 2 * in_far * in_near / nearmfar, 0
//		};
//		memcpy(&out_mret, m, sizeof(float) * 16);
//	}
//
//	static void normalize(s_float3& in_out_vector)
//	{
//		float len = sqrt(
//			in_out_vector.x * in_out_vector.x +
//			in_out_vector.y * in_out_vector.y +
//			in_out_vector.z * in_out_vector.z);
//
//		if (len != 0.)
//		{
//			in_out_vector.x /= len;
//			in_out_vector.y /= len;
//			in_out_vector.z /= len;
//		}
//	}
//
//	static s_float3 cross(s_float3 vect_A, s_float3 vect_B)
//	{
//		s_float3 cross_P;
//		cross_P.x = vect_A.y * vect_B.z - vect_A.z * vect_B.y;
//		cross_P.y = vect_A.z * vect_B.x - vect_A.x * vect_B.z;
//		cross_P.z = vect_A.x * vect_B.y - vect_A.y * vect_B.x;
//		return cross_P;
//	}
//
//	static float dot(s_float3 vect_A, s_float3 vect_B)
//	{
//		return vect_A.x * vect_B.x + vect_A.y * vect_B.y + vect_A.z * vect_B.z;
//	}
//
//	/// <summary>
//	/// 设置透镜
//	/// </summary>
//	/// <param name="fovY">弧度单位的视野角</param>
//	/// <param name="aspect">纵横比</param>
//	/// <param name="zn">近裁距离</param>
//	/// <param name="zf">远裁距离</param>
//	void set_lens(float in_fov, float in_aspect, float in_near, float in_far)
//	{
//		// cache properties
//		fov_y = in_fov; // 弧度单位的视野角
//		aspect = in_aspect;//纵横比
//		near_z = in_near;//近裁距离
//		far_z = in_far;//远裁距离
//
//		near_window_height = 2.0f * near_z * tanf(0.5f * fov_y);
//		far_window_height = 2.0f * far_z * tanf(0.5f * fov_y);
//
//		perspective_fov(fov_y, aspect, near_z, far_z, project_matrix);
//
//	}
//
//	/// <summary>
//	/// 转换到相机坐标系的矩阵
//	/// </summary>
//	void update_view()
//	{
//		s_float3 nom_u = up_axi;
//		s_float3 nom_l = look_axi;
//
//		normalize(nom_l);
//		nom_u = cross(nom_l, right_axi);
//		normalize(nom_u);
//		s_float3 nom_r = cross(nom_u, nom_l);
//
//		//变换（旋转）后的坐标系和世界坐标系不一样，
//		//要把相机的世界位置映射到变换（旋转）后的坐标系中，
//		//才能放进摄像机局部变换矩阵
//		float x = -dot(world_position, nom_r);
//		float y = -dot(world_position, nom_u);
//		float z = -dot(world_position, nom_l);
//
//		right_axi = nom_r;
//		up_axi = nom_u;
//		look_axi = nom_l;
//
//		view_matrix._11 = right_axi.x;
//		view_matrix._21 = right_axi.y;
//		view_matrix._31 = right_axi.z;
//		view_matrix._41 = x;
//
//		view_matrix._12 = up_axi.x;
//		view_matrix._22 = up_axi.y;
//		view_matrix._32 = up_axi.z;
//		view_matrix._42 = y;
//
//		view_matrix._13 = look_axi.x;
//		view_matrix._23 = look_axi.y;
//		view_matrix._33 = look_axi.z;
//		view_matrix._43 = z;
//
//		view_matrix._14 = 0.0f;
//		view_matrix._24 = 0.0f;
//		view_matrix._34 = 0.0f;
//		view_matrix._44 = 1.0f;
//	}
//
//	// Camera coordinate system with coordinates relative to world space.
//	s_float3 world_position = { 0.0f, 0.0f, 0.0f };
//	s_float3 right_axi = { 1.0f, 0.0f, 0.0f };
//	s_float3 up_axi = { 0.0f, 1.0f, 0.0f };
//	s_float3 look_axi = { 0.0f, 0.0f, 1.0f };
//
//	// Cache frustum properties.
//	float near_z = 0.0f;
//	float far_z = 0.0f;
//	float aspect = 0.0f;
//	float fov_y = 0.0f;
//	float near_window_height = 0.0f;
//	float far_window_height = 0.0f;
//	// Cache View/Proj matrices.
//	s_float4x4 view_matrix = s_float4x4::identity4x4();
//	s_float4x4 project_matrix = s_float4x4::identity4x4();
//};