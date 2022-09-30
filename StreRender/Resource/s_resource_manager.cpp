#include "stre_render.h"
#include "cg_object.h"
#include "Core/Memory/s_memory.h"
#include "Core/File_Manager/s_fbx.h"
#include "cg_resource_factory.h"
////////////////////////////////////////////////////////////////////
//create object

s_memory_allocater_register resource_allocater("resource_allocater");

s_object* s_resource_manager::create_object(s_resource* in_resource)
{
	auto allocater = memory_allocater_group["resource_allocater"];
	
	return allocater->allocate<cg_object>((cg_resource*)in_resource);
}
s_static_object* s_resource_manager::create_static_object(s_resource* in_resource)
{
	auto allocater = memory_allocater_group["resource_allocater"];

	return allocater->allocate<cg_static_object,cg_resource>((cg_resource*)in_resource);

}
s_dynamic_object* s_resource_manager::create_dynamic_object(s_resource* in_resource)
{
	auto allocater = memory_allocater_group["resource_allocater"];

	return allocater->allocate<cg_dynamic_object>((cg_resource*)in_resource);

}
s_camera* s_resource_manager::create_camera(s_resource* in_resource)
{
	auto allocater = memory_allocater_group["resource_allocater"];

	return allocater->allocate<cg_camera>((cg_resource*)in_resource);

}
s_light* s_resource_manager::create_light(s_resource* in_resource)
{
	auto allocater = memory_allocater_group["resource_allocater"];

	return allocater->allocate<cg_light>((cg_resource*)in_resource);

}
s_sence* s_resource_manager::create_sence(s_resource* in_resource)
{
	auto allocater = memory_allocater_group["resource_allocater"];

	return allocater->allocate<cg_sence>((cg_resource*)in_resource);

}
s_material* s_resource_manager::create_material(s_resource* in_resource)
{
	auto allocater = memory_allocater_group["resource_allocater"];

	return allocater->allocate<cg_material>((cg_resource*)in_resource);

}
s_texture* s_resource_manager::create_texture(s_resource* in_resource)
{
	auto allocater = memory_allocater_group["resource_allocater"];

	return allocater->allocate<cg_texture>((cg_resource*)in_resource);

}

/***
************************************************************
*
* Init Resource
*
************************************************************
*/
//��Դ�Ĺ�������cg_resource_factory
// 
//cpu��Դ��������
//1.����reource_factory����Ĭ�ϵ�resource
//2.������resource ������Ӧ��������
//3.�������͵Ĺ��캯���й��� GPU layout

s_resource* s_resource_manager::create_object_resource()
{

}

s_resource* s_resource_manager::create_camera_resource()
{

}

s_resource* s_resource_manager::create_light_resource()
{

}

s_resource* s_resource_manager::create_sence_resource()
{

}

s_resource* s_resource_manager::create_material_resource()
{

}

s_resource* s_resource_manager::create_texture_resource()
{

}



////////////////////////////////////////////////////////////////////
//read resource

/***
************************************************************
*
* Auxiliary Function
*
************************************************************
*/

std::wstring StringToWString(const std::string& str) {
    int num = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, NULL, 0);
    wchar_t* wide = new wchar_t[num];
    MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, wide, num);
    std::wstring w_str(wide);
    delete[] wide;
    return w_str;
}

std::string WstringToString(std::wstring wstr)
{
    std::string result;
    int len = WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), wstr.size(), NULL, 0, NULL, NULL);
    char* buffer = new char[len + 1];
    WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), wstr.size(), buffer, len, NULL, NULL);
    buffer[len] = '\0';
    result.append(buffer);
    delete[] buffer;
    return result;
}


/***
************************************************************
*
* Read Resource
*
************************************************************
*/

//���պ�׺�������ͣ�������ȡ
s_resource* s_resource_manager::load_local_resource( wchar_t* in_path)
{

}

std::string GetFbxFile(std::wstring DirPath)
{
    HANDLE hFind;
    WIN32_FIND_DATA data;
    std::string res;
    std::wstring SearchName = DirPath + L"*.fbx";
    int i = 0;
    hFind = FindFirstFile(SearchName.c_str(), &data);
    if (hFind != INVALID_HANDLE_VALUE) {

        res = WstringToString(DirPath + data.cFileName);
        FindClose(hFind);
    }
    return res;
}

//����֧�ֺ���
//void ReadNodeRecursive(
//    RSkeleton_Model* IOSkeletonmodel,
//    Fbx* IFbx,
//    FbxNode* pNode/*,FbxAMatrix& pParentGlobalPosition*/)
//{
//    FbxTakeInfo* IAnimInfo = IFbx->PAnimInfo;
//
//    //��¼ÿ֡����λ��ƫ��
//    FbxAMatrix lGeometryOffset = GetGeometry(pNode);
//
//    FbxTime start = IAnimInfo->mLocalTimeSpan.GetStart();
//    FbxTime end = IAnimInfo->mLocalTimeSpan.GetStop();
//    FbxLongLong animationlength = end.GetFrameCount(FbxTime::eFrames30) - start.GetFrameCount(FbxTime::eFrames30) + 1;
//    //IOSkeletonmodel->GlobalOffPosition.assign(animationlength, DirectX::XMMATRIX());
//
//    int hour;
//    int minute;
//    int second;
//    int frame;
//    int field;
//    int residual;
//
//    end.GetTime(hour, minute, second, frame, field, residual);
//
//    IOSkeletonmodel->Animationlength = hour * 3600 + minute * 60 + second + (float)frame / 60.0;
//
//    IOSkeletonmodel->Framecount = 30;
//
//    //for (FbxLongLong i = start.GetFrameCount(FbxTime::eFrames30); i <= end.GetFrameCount(FbxTime::eFrames30); ++i)
//    //{
//    //    //each second's tranformMatrix of Joint
//    //    FbxTime time;
//    //    time.SetFrame(i, FbxTime::eFrames30);
//    //    FbxAMatrix lGlobalPosition = pNode->EvaluateGlobalTransform(time);
//    //    FbxAMatrix lGlobalOffPosition = lGlobalPosition * lGeometryOffset;
//    //    lGlobalOffPosition = lGlobalOffPosition.Inverse();
//    //    //��������任����
//    //    ConvertFBXMatrix(lGlobalOffPosition, IOSkeletonmodel->GlobalOffPosition[i]);
//    //}
//
//    if (pNode->GetNodeAttribute())
//    {
//        ReadNode(IOSkeletonmodel, IFbx, pNode);
//    }
//
//    //��ʱû��Ҫ����scene���е�node
//    //const int lChildCount = pNode->GetChildCount();
//    //for (int lChildIndex = 0; lChildIndex < lChildCount; ++lChildIndex)
//    //{
//    //    ReadNodeRecursive(IOSkeletonmodel, IFbx, pNode->GetChild(lChildIndex) /*lGlobalPosition*/);
//    //}
//}
//
////1.��Ҫ�ؽ�λ�� Skeleton
////2.��ҪƤ���Թؽڵľ���λ�� Cluster
////3.��Ҫ�ؽڱ仯����  Animation Stack
//bool GetAnimationData(s_resource* in_out_resource, s_fbx* IFbx)
//{
//
//    FbxMesh* IFbxMesh = IFbx->pMesh;
//    FbxAnimStack* IAnimStack = IFbx->PAnimStack;
//    FbxTakeInfo* IAnimInfo = IFbx->PAnimInfo;
//    auto& IOSkeletongroupIndexmap = IFbx->pSkeletongroupIndexmap;
//    auto& IParentIndexmap = IFbx->SkeletonParentmap;
//
//    const int lVertexCount = IFbxMesh->GetControlPointsCount();
//
//    // No vertex to draw.
//    if (lVertexCount == 0)
//    {
//        return false;
//    }
//
//    //������޶���
//    const bool lHasVertexCache = IFbxMesh->GetDeformerCount(FbxDeformer::eVertexCache) &&
//        (static_cast<FbxVertexCacheDeformer*>(IFbxMesh->GetDeformer(0, FbxDeformer::eVertexCache)))->Active.Get();
//    const bool lHasShape = IFbxMesh->GetShapeCount() > 0;
//    const bool lHasSkin = IFbxMesh->GetDeformerCount(FbxDeformer::eSkin) > 0;
//    const bool lHasDeformation = lHasVertexCache || lHasShape || lHasSkin;
//
//    //�ж���
//    if (lHasDeformation)
//    {
//        if (lHasVertexCache)
//        {//���㻺�棨֡������//������û���ؽ��
//            //ReadVertexCacheData(IFbxMesh);
//        }
//        else
//        {//���㶥��λ��
//
//            IOSkeletonmodel->Jointsgroup.assign(IFbx->pSkeletongroup.size(), RSkeleton_Model::Joint());
//
//            ReadNodeRecursive(IOSkeletonmodel, IFbx, IFbx->pMeshNode);
//        }
//
//        return true;
//    }
//
//    return false;
//
//}



//fbx���
s_resource* s_resource_manager::load_local_fbx(wchar_t* in_path)
{
    bool has_animation = false;

    auto allocater = memory_allocater_group["resource_allocater"];

    s_resource* out_resource = allocater->allocate<cg_resource>();

    //·��
    std::wstring IDictionary(in_path);

    //FBX����
    s_fbx myFbx;
    std::string Fbxpath = GetFbxFile(IDictionary);
    if (Fbxpath.empty()) return false;
    myFbx.ReadFbx(Fbxpath.c_str(), has_animation);

    //��ȡfbx������Ϣ
    using GeoElement = FbxGeometryElement;
    using LayerElement = FbxLayerElement;
    const int polygonCount = myFbx.pMesh->GetPolygonCount();
    auto controlPoints = myFbx.pMesh->GetControlPoints();
    const int controlPointCount = myFbx.pMesh->GetControlPointsCount();
    int PolygonType = myFbx.pMesh->GetPolygonSize(0);
    fbxsdk::FbxLayerElementArrayTemplate<int>* pMaterialIndices = &myFbx.pMesh->GetElementMaterial()->GetIndexArray();
    const int material_size = myFbx.pMesh->GetElementMaterialCount();
    
    
    //Ϊ��Դ����ռ�
    cg_static_object::cpu_static_mesh_data* mesh_data;
    {
        cg_resource_factory mesh_factory;
        mesh_data = mesh_factory.allocate_static_mesh((cg_resource*)out_resource, controlPointCount, (int)polygonCount * PolygonType, material_size);
        //����Ĭ�ϲ���
        for (int i = 0; i < material_size; i++)
        {
            s_resource* mat_resource = allocater->allocate<cg_resource>();
            
            auto mat_data = mesh_factory.allocate_material((cg_resource*)mat_resource);
            
            //??? ����Ĭ�ϲ���������
            //...
            //mat_data->textrue

            //����object
            mesh_data->material_group_ptr[i] = (cg_material*)create_material(mat_resource);
        }
    
    }
    auto vertices = mesh_data->vertex_group_ptr;
    auto indices = mesh_data->index_group_ptr;
    auto indeices_offset = mesh_data->material_index_offset_group_ptr;
    auto& object_constant = mesh_data->object_constant_data;
    auto& vMin = object_constant.object_bound_box.min_position;
    auto& vMax = object_constant.object_bound_box.max_position;



    //��ȡ����
    bool HasNormal = myFbx.pMesh->GetElementNormalCount() > 0;
    bool HasUV = myFbx.pMesh->GetElementUVCount() > 0;
    bool NormalAllByControlPoint = true;
    bool UVAllByControlPoint = true;
    const FbxGeometryElementNormal* lNormalElement = NULL;
    const FbxGeometryElementUV* lUVElement = NULL;
    FbxGeometryElement::EMappingMode lNormalMappingMode = FbxGeometryElement::eNone;
    FbxGeometryElement::EMappingMode lUVMappingMode = FbxGeometryElement::eNone;

    FbxVector4 lCurrentNormal;
    FbxVector2 lCurrentUV;
    //��Ҫ���� controlpoint �� vertice
    if (HasNormal)
    {
        lNormalMappingMode = myFbx.pMesh->GetElementNormal(0)->GetMappingMode();
        lNormalElement = myFbx.pMesh->GetElementNormal(0);
        HasNormal = lNormalMappingMode != FbxGeometryElement::eNone;
        NormalAllByControlPoint = HasNormal && (lNormalMappingMode == FbxGeometryElement::eByControlPoint);
    }
    if (HasUV)
    {
        lUVMappingMode = myFbx.pMesh->GetElementUV(0)->GetMappingMode();
        lUVElement = myFbx.pMesh->GetElementUV(0);
        HasUV = lUVMappingMode != FbxGeometryElement::eNone;
        UVAllByControlPoint = HasUV && (lUVMappingMode == FbxGeometryElement::eByControlPoint);
    }


    for (int i = 0; i < controlPointCount; i++)
    {
        vertices[i].position = s_float3(
            controlPoints[i].mData[0],
            controlPoints[i].mData[1],
            controlPoints[i].mData[2]
        );

        // Save the normal.
        if (NormalAllByControlPoint)
        {
            int lNormalIndex = i;
            if (lNormalElement->GetReferenceMode() == FbxLayerElement::eIndexToDirect)
            {
                lNormalIndex = lNormalElement->GetIndexArray().GetAt(i);
            }
            lCurrentNormal = lNormalElement->GetDirectArray().GetAt(lNormalIndex);
            vertices[i].normal = s_float3(
                static_cast<float>(lCurrentNormal.mData[0]),
                static_cast<float>(lCurrentNormal.mData[1]),
                static_cast<float>(lCurrentNormal.mData[2])
            );
        }

        // Save the UV.
        if (UVAllByControlPoint)
        {
            int lUVIndex = i;
            if (lUVElement->GetReferenceMode() == FbxLayerElement::eIndexToDirect)
            {
                lUVIndex = lUVElement->GetIndexArray().GetAt(i);
            }
            lCurrentUV = lUVElement->GetDirectArray().GetAt(lUVIndex);
            vertices[i].texC = s_float2(
                static_cast<float>(lCurrentUV.mData[0]),
                static_cast<float>(lCurrentUV.mData[1])
            );
        }
    }
    int current_material_id = 0;
    int IndeicesCount = 0;
    int materialCount = 0;
    for (int polygon = 0; polygon < polygonCount; polygon++) //each polygon
    {
        const int polyVertCount = myFbx.pMesh->GetPolygonSize(polygon);
        for (int polyVert = 0; polyVert < polyVertCount; polyVert++) //each Index
        {
            const int cpIndex = myFbx.pMesh->GetPolygonVertex(polygon, polyVert);

            //indices.push_back(cpIndex);
            indices[IndeicesCount] = cpIndex;

            if (!UVAllByControlPoint)
            {
                const int uvElementsCount = myFbx.pMesh->GetElementUVCount();
                for (int uvElement = 0; uvElement < uvElementsCount; uvElement++)
                {
                    const FbxGeometryElementUV* geometryElementUV = myFbx.pMesh->GetElementUV(uvElement);
                    const auto mapMode = geometryElementUV->GetMappingMode();
                    const auto refMode = geometryElementUV->GetReferenceMode();
                    int directIndex = -1;
                    if (GeoElement::eByControlPoint == mapMode)
                    {
                        if (GeoElement::eDirect == refMode) directIndex = cpIndex;
                        else if (GeoElement::eIndexToDirect == refMode)
                            directIndex = geometryElementUV->GetIndexArray().GetAt(cpIndex);
                    }
                    else if (
                        GeoElement::eByPolygonVertex == mapMode
                        && (GeoElement::eDirect == refMode || FbxGeometryElement::eIndexToDirect == refMode)
                        )
                    {
                        directIndex = myFbx.pMesh->GetTextureUVIndex(polygon, polyVert);
                    }
                    if (directIndex == -1) continue;
                    FbxVector2 uv = geometryElementUV->GetDirectArray().GetAt(directIndex);
                    //UV
                    vertices[cpIndex].texC = s_float2(
                        static_cast<float>(uv.mData[0]),
                        static_cast<float>(uv.mData[1])
                    );
                }
            }
            if (!NormalAllByControlPoint)
            {
                const int normalElementCount = myFbx.pMesh->GetElementNormalCount();
                for (int normalElement = 0; normalElement < normalElementCount; normalElement++)
                {
                    const FbxGeometryElementNormal* geometryElementNormal = myFbx.pMesh->GetElementNormal(normalElement);
                    const LayerElement::EMappingMode mapMode = geometryElementNormal->GetMappingMode();
                    const LayerElement::EReferenceMode refMode = geometryElementNormal->GetReferenceMode();
                    int directIndex = -1;
                    if (GeoElement::eByPolygonVertex == mapMode || GeoElement::eByControlPoint == mapMode)
                    {
                        if (GeoElement::eDirect == refMode) directIndex = IndeicesCount;
                        else if (GeoElement::eIndexToDirect == refMode)
                            directIndex = geometryElementNormal->GetIndexArray().GetAt(IndeicesCount);
                    }
                    if (directIndex == -1) continue;
                    FbxVector4 norm = geometryElementNormal->GetDirectArray().GetAt(directIndex);
                    //NORMAL
                    vertices[cpIndex].normal = s_float3(
                        static_cast<float>(norm.mData[0]),
                        static_cast<float>(norm.mData[1]),
                        static_cast<float>(norm.mData[2])
                    );
                }
            }

            //??? û������
            //XMVECTOR P = XMLoadFloat3(&vertices[cpIndex].Position);
            //XMVECTOR N = XMLoadFloat3(&vertices[cpIndex].Normal);
            //XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
            //if (fabsf(XMVectorGetX(XMVector3Dot(N, up))) < 1.0f - 0.001f)
            //{
            //    XMVECTOR T = XMVector3Normalize(XMVector3Cross(up, N));
            //    XMStoreFloat3(&vertices[cpIndex].TangentU, T);
            //}
            //else
            //{
            //    up = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
            //    XMVECTOR T = XMVector3Normalize(XMVector3Cross(N, up));
            //    XMStoreFloat3(&vertices[cpIndex].TangentU, T);
            //}

            
            vMin.x = min(vMin.x, vertices[cpIndex].position.x);
            vMin.y = min(vMin.x, vertices[cpIndex].position.y);
            vMin.z = min(vMin.x, vertices[cpIndex].position.z);

            vMax.x = max(vMax.x, vertices[cpIndex].position.x);
            vMax.y = max(vMax.y, vertices[cpIndex].position.y);
            vMax.z = max(vMax.z, vertices[cpIndex].position.z);


            if (pMaterialIndices->GetAt(polygon) != current_material_id)
            {
                current_material_id = pMaterialIndices->GetAt(polygon);

                indeices_offset[materialCount] = IndeicesCount;
                materialCount++;
            }
            IndeicesCount++;
        }
    }
    materialCount++;
    indeices_offset[materialCount] = IndeicesCount;

    //��Χ����Ҫ�ٽ�������
    //XMStoreFloat3(&bounds.Center, 0.5f * (vMin + vMax));
    //XMStoreFloat3(&bounds.Extents, 0.5f * (vMax - vMin));

    //������������ȡ ��ûд
    //if (has_animation)
    //{
    //    auto* Skeletonmodel = (RSkeleton_Model*)IOObjectmodel;
    //    GetAnimationData(Skeletonmodel, &myFbx);
    //    //����һ���������㻺��
    //    Skeletonmodel->Verticescurrent.assign(controlPointCount, GVertex());
    //    const UINT vbByteSize = (UINT)Objectmodel->CPUMeshdata.Vertices.size() * sizeof(GVertex);
    //    void* StaVectorptr = Objectmodel->CPUMeshdata.Vertices.data();
    //    void* SkeVectorptr = Skeletonmodel->Verticescurrent.data();
    //    CopyMemory(SkeVectorptr, StaVectorptr, vbByteSize);
    //}

    return out_resource;
}



////////////////////////////////////////////////////////////////////
