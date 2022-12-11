#ifdef  DLL_GRAPHICS_API
#else
#define DLL_GRAPHICS_API _declspec(dllexport)
#endif

#include "resource_factory.h"
#include "Core/Memory/s_memory.h"
#include "Core/File_Manager/s_fbx.h"

/***
************************************************************
*
* Create & Load Function
*
************************************************************
*/

cpu_mesh* mesh_manager::create_resource()
{
    auto instance = new cpu_mesh();
    generate_unique_identifier<cpu_mesh>(instance->uid);
    return instance;
}

void mesh_manager::dx_allocate_gpu_resource(cpu_mesh* in_cpu_data)
{
    typedef gpu_shader_resource::SHADER_RESOURCE_TYPE GPU_SR_TYPE;

    if(in_cpu_data->vertex_ptr)
    custom_manager<cpu_vertex>().dx_allocate_gpu_resource(
        in_cpu_data->vertex_ptr,
        GPU_SR_TYPE::SHADER_RESOURCE_TYPE_CUSTOM_BUFFER_GROUP);

    if (in_cpu_data->index_ptr)
    custom_manager<cpu_index>().dx_allocate_gpu_resource(
        in_cpu_data->index_ptr,
        GPU_SR_TYPE::SHADER_RESOURCE_TYPE_CUSTOM_BUFFER_GROUP);

    if (in_cpu_data->material_ptr)
    custom_manager<cpu_material>().dx_allocate_gpu_resource(
        in_cpu_data->material_ptr,
        GPU_SR_TYPE::SHADER_RESOURCE_TYPE_CUSTOM_BUFFER_FOLLOW_MESH);

    if (in_cpu_data->object_constant_ptr)
    custom_manager<cpu_object_constant>().dx_allocate_gpu_resource(
        in_cpu_data->object_constant_ptr,
        GPU_SR_TYPE::SHADER_RESOURCE_TYPE_CUSTOM_BUFFER_FOLLOW_MESH);

    //custom_manager<cpu_texture>::dx_allocate_gpu_resource(
    //    in_cpu_data->texture_ptr,
    //    GPU_SR_TYPE::SHADER_RESOURCE_TYPE_TEXTURE_GROUP);
}

cpu_mesh* mesh_manager::load_resource(const wchar_t* in_path)
{
    return nullptr;
}


/***
************************************************************
*
* Update Function
*
************************************************************
*/

void mesh_manager::update_gpu(cpu_mesh* in_cpu_data)
{
    if (in_cpu_data->vertex_ptr)
        custom_manager<cpu_vertex>().update_gpu(in_cpu_data->vertex_ptr);

    if (in_cpu_data->index_ptr)
        custom_manager<cpu_index>().update_gpu(in_cpu_data->index_ptr);

    if (in_cpu_data->material_ptr)
        custom_manager<cpu_material>().update_gpu(in_cpu_data->material_ptr);

    if (in_cpu_data->object_constant_ptr)
        custom_manager<cpu_object_constant>().update_gpu(in_cpu_data->object_constant_ptr);

}

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
    int len = WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), (int)wstr.size(), NULL, 0, NULL, NULL);
    char* buffer = new char[len + 1];
    WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), (int)wstr.size(), buffer, len, NULL, NULL);
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

//按照后缀区分类型，进而读取

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

//动画支持函数
//void ReadNodeRecursive(
//    RSkeleton_Model* IOSkeletonmodel,
//    Fbx* IFbx,
//    FbxNode* pNode/*,FbxAMatrix& pParentGlobalPosition*/)
//{
//    FbxTakeInfo* IAnimInfo = IFbx->PAnimInfo;
//
//    //记录每帧主体位置偏移
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
//    //    //保存物体变换矩阵
//    //    ConvertFBXMatrix(lGlobalOffPosition, IOSkeletonmodel->GlobalOffPosition[i]);
//    //}
//
//    if (pNode->GetNodeAttribute())
//    {
//        ReadNode(IOSkeletonmodel, IFbx, pNode);
//    }
//
//    //暂时没必要遍历scene所有的node
//    //const int lChildCount = pNode->GetChildCount();
//    //for (int lChildIndex = 0; lChildIndex < lChildCount; ++lChildIndex)
//    //{
//    //    ReadNodeRecursive(IOSkeletonmodel, IFbx, pNode->GetChild(lChildIndex) /*lGlobalPosition*/);
//    //}
//}
//
////1.需要关节位置 Skeleton
////2.需要皮肤对关节的矩阵位置 Cluster
////3.需要关节变化数据  Animation Stack
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
//    //检查有无动画
//    const bool lHasVertexCache = IFbxMesh->GetDeformerCount(FbxDeformer::eVertexCache) &&
//        (static_cast<FbxVertexCacheDeformer*>(IFbxMesh->GetDeformer(0, FbxDeformer::eVertexCache)))->Active.Get();
//    const bool lHasShape = IFbxMesh->GetShapeCount() > 0;
//    const bool lHasSkin = IFbxMesh->GetDeformerCount(FbxDeformer::eSkin) > 0;
//    const bool lHasDeformation = lHasVertexCache || lHasShape || lHasSkin;
//
//    //有动画
//    if (lHasDeformation)
//    {
//        if (lHasVertexCache)
//        {//顶点缓存（帧动画）//？？？没返回结果
//            //ReadVertexCacheData(IFbxMesh);
//        }
//        else
//        {//计算顶点位置
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

//fbx解包

cpu_mesh* mesh_manager::load_fbx(const wchar_t* in_path)
{
    bool has_animation = false;

    //路径
    std::wstring IDictionary(in_path);
    //FBX解析
    s_fbx myFbx;
    std::string Fbxpath = GetFbxFile(IDictionary);
    if (Fbxpath.empty()) return nullptr;
    myFbx.ReadFbx(Fbxpath.c_str(), has_animation);

    cpu_mesh* out_resource = create_resource();

    out_resource->path = in_path;


    //获取fbx数据信息
    using GeoElement = FbxGeometryElement;
    using LayerElement = FbxLayerElement;
    const int polygonCount = myFbx.pMesh->GetPolygonCount();
    auto controlPoints = myFbx.pMesh->GetControlPoints();
    const int controlPointCount = myFbx.pMesh->GetControlPointsCount();
    int PolygonType = myFbx.pMesh->GetPolygonSize(0);
    fbxsdk::FbxLayerElementArrayTemplate<int>* pMaterialIndices = &myFbx.pMesh->GetElementMaterial()->GetIndexArray();
    int material_size = 2;
    int mat_count = 0;
    //???不知道怎么获得mesh的材质数量，只能遍历一遍
    
    for (int polygon = 0; polygon < polygonCount; polygon++)
    {
        //由于相同材质的点群可能被错开，只能当作分别的材质处理
        auto current_mat_index = pMaterialIndices->GetAt(polygon);
        if (current_mat_index != mat_count)
        {
            material_size++;
            mat_count = current_mat_index;
        }
    }

    
    
    //为资源分配空间
    {

        out_resource->vertex_ptr = custom_manager<cpu_vertex>().create_resource(controlPointCount,true);
        out_resource->index_ptr = custom_manager<cpu_index>().create_resource((int)polygonCount * PolygonType, true);
        out_resource->material_ptr = custom_manager<cpu_material>().create_resource(material_size, true);
        out_resource->object_constant_ptr = custom_manager<cpu_object_constant>().create_resource(material_size, true);
        out_resource->mesh_element_index_count.assign(material_size,0);
        //贴图得自己更新了
        //out_resource->texture_ptr.assign();



    }

    auto vertices = out_resource->vertex_ptr->get_data();
    auto indices = out_resource->index_ptr->get_data();
    auto& indeices_offset = out_resource->mesh_element_index_count;
    auto object_constant = out_resource->object_constant_ptr->get_data();
    s_float3& vMin = object_constant->object_bound_box.min_position;
    s_float3& vMax = object_constant->object_bound_box.max_position;

    //初始化object_constants
    for (int i = 0; i < material_size; i++)
    {
        object_constant[i] = s_object_constant();
        object_constant[i].material_index = i;
    }

    //读取数据
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
    //需要区分 controlpoint 和 vertice
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
    int frontCount = 0;
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

            //??? 没算切线
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

                indeices_offset[materialCount] = IndeicesCount - frontCount;
                frontCount = IndeicesCount;
                materialCount++;
            }
            IndeicesCount++;
        }
    }
    materialCount++;
    indeices_offset[materialCount] = IndeicesCount - frontCount;

    //std::vector<s_vertex> debug_see(vertices, vertices+ controlPointCount);

    //包围盒需要再建？？？
    //XMStoreFloat3(&bounds.Center, 0.5f * (vMin + vMax));
    //XMStoreFloat3(&bounds.Extents, 0.5f * (vMax - vMin));

    //？？？动画读取 还没写
    //if (has_animation)
    //{
    //    auto* Skeletonmodel = (RSkeleton_Model*)IOObjectmodel;
    //    GetAnimationData(Skeletonmodel, &myFbx);
    //    //多做一个动画顶点缓存
    //    Skeletonmodel->Verticescurrent.assign(controlPointCount, GVertex());
    //    const UINT vbByteSize = (UINT)Objectmodel->CPUMeshdata.Vertices.size() * sizeof(GVertex);
    //    void* StaVectorptr = Objectmodel->CPUMeshdata.Vertices.data();
    //    void* SkeVectorptr = Skeletonmodel->Verticescurrent.data();
    //    CopyMemory(SkeVectorptr, StaVectorptr, vbByteSize);
    //}

    return out_resource;
}
