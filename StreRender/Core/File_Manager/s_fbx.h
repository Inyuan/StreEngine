#pragma once
#include <fbxsdk.h>
#include <cstdint>
#include <vector>
#include <map>
#include "base_type.h"

class s_fbx
{
private:
	FbxManager* lSdkManager;
	FbxScene* lScene;
	void PrintNode(FbxNode* pNode);
	FbxString GetAttributeTypeName(FbxNodeAttribute::EType type);
	void PrintAttribute(FbxNodeAttribute* pAttribute);
	void ProcessNode(FbxNode* pNode);
	bool ProcessMesh(FbxNode* pNode);
	bool ProcessCamera();
	bool ProcessLight(FbxNode* pNode);

	void ProcessSkeletonHeirarchy(FbxNode* rootnode);
	void ProcessSkeletonHeirarchyre(FbxNode* node, int depth, int index, int parentindex);


public:

	FbxMesh* pMesh;
	FbxNode* pMeshNode;
	FbxPose* pPose;
	FbxAnimStack* PAnimStack;
	FbxTakeInfo* PAnimInfo;
	std::vector<int> SkeletonParentmap;
	std::vector<FbxSkeleton*> pSkeletongroup;
	std::map<unsigned long,size_t> pSkeletongroupIndexmap;
	

	bool ReadFbx(const char* lFilename, bool IhasAnime);
	void ReadVertex(int ctrlPointIndex, s_float3* pVertex);
	void ReadColor(int ctrlPointIndex, int vertexCounter, s_float4* pColor);
	bool ReadUV(int ctrlPointIndex, int textureUVIndex, int uvLayer, s_float2* pUV);
	void ReadNormal(int ctrlPointIndex, int vertexCounter, s_float3* pNormal);
	void ReadTangent(int ctrlPointIndex, int vertecCounter, s_float3* pTangent);

	void GetAnimationStack();

	void PrintFbx();
	
};