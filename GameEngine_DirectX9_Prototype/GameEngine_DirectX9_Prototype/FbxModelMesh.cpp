#include "FbxModelMesh.h"
#include "FbxModelSkeletonBones.h"


FbxMatrix FbxModelMesh::getGeometryTransformation(FbxNode * node)
{
	FbxMatrix result;
	result.SetIdentity();
	if (node)
	{
		result.SetTRS
		(
			node->GetGeometricTranslation(FbxNode::eSourcePivot),
			node->GetGeometricRotation(FbxNode::eSourcePivot),
			node->GetGeometricScaling(FbxNode::eSourcePivot)
		);
	}
	return result;
}

void FbxModelMesh::processVertices(FbxModelSkeletonBones * skeletonBones)
{
	if (!mesh) return;

	unsigned int polygonCount = mesh->GetPolygonCount();
	unsigned int vertexCounter = 0;
	unsigned int controlPointCount = mesh->GetControlPointsCount();

	vector<WeightOfBones> weightOfBonesArray{ controlPointCount, {} };

	// 스켈레톤 본이 있을때 본들의 가중치 계산 각각의 컨트롤 포인트에 대해서
	if (skeletonBones)
	{
		// 보통 메쉬하나당 디포머 하나 // 내부적으로 클러스터덩어리들을 가지고 있다. // 본에 맞는 피부조직같은 느낌
		unsigned int deformerCount = mesh->GetDeformerCount();
		for (int i = 0; i < deformerCount; ++i)
		{
			FbxDeformer * deformer = mesh->GetDeformer(i);
			// 없을때 컨티뉴
			if (!deformer) continue;

			FbxSkin * skin = nullptr;
			if (deformer->GetDeformerType() == FbxDeformer::eSkin)
			{
				skin = static_cast<FbxSkin*>(deformer);

				if (!skin) continue;
			}


			unsigned int clusterCount = skin->GetClusterCount();

			for (int j = 0; j < clusterCount; ++j)
			{
				FbxCluster * cluster = skin->GetCluster(j);
				if (!cluster) continue;

				// 여기서 링크노드의 본은 이 클러스트와 매칭된다.
				FbxNode * linkNode = cluster->GetLink();
				if (!linkNode) continue;

				string linkNodeName = linkNode->GetName();
				FbxModelSkeletonBone * bone = skeletonBones->findBoneWithBoneName(linkNodeName);
				if (!bone) continue;


				FbxAMatrix clusterTransformMatrix;
				FbxAMatrix clusterLinkTransformMatrix;

				cluster->GetTransformMatrix(clusterTransformMatrix);
				cluster->GetTransformLinkMatrix(clusterLinkTransformMatrix);

				bone->setClusterTransformMatrix(clusterTransformMatrix);
				bone->setClusterTransformLinkMatrix(clusterLinkTransformMatrix);

				int * indices = cluster->GetControlPointIndices();
				double * weights = cluster->GetControlPointWeights();

				for (int k = 0; k < cluster->GetControlPointIndicesCount(); ++k)
					weightOfBonesArray[indices[k]].addWeightOfBone(bone->getBoneIndex(), (float)weights[k]);


			}

		}


	}

	FbxDouble3 position{ 0,0,0 };
	FbxDouble3 normal{ 0,0,0 };
	FbxDouble3 binormal{ 0,0,0 };
	FbxDouble3 tangent{ 0,0,0 };
	FbxDouble2 uv{ 0,0 };

	// 폴리곤
	// 원래는 폴리곤 사이즈가 3/4였는데 converter에 의해서 3개로 모두 변경되었다.
	// directX에서 작업하기 편하다.
	for (unsigned int i = 0; i < polygonCount; ++i)
	{
		//for (unsigned int j = 0; j < mesh->GetPolygonSize(i); ++j)
		//{
		int polygonSize = mesh->GetPolygonSize(i);

		// 현재는 모두 삼각형으로 분리시켰기 때문에 모두 삼각형기준으로 작업된다.
		if (polygonSize == 3)
		{
			for (unsigned int j = 0; j < 3; ++j)
			{
				int controlPointIndex = mesh->GetPolygonVertex(i, j);

				// 본가중치 추가 컨트롤포인트의 인덱스 받아서 처리

				// 각각의 정보를 초기화 시켜준다.
				// 만약에 내부에 정보가 없으면 모두 0으로 들어간다.
				position = { 0,0,0 };
				normal = { 0,0,0 };
				binormal = { 0,0,0 };
				tangent = { 0,0,0 };
				uv = { 0,0 };

				// 내부 레퍼런스 모드에 따라서 버텍스 카운터가 필요할 수도 있고
				// 컨트롤 포인트 인덱스가 필요할 수도 있다.
				position = controlPoints[controlPointIndex];
				readNormal(mesh, controlPointIndex, vertexCounter, normal);
				readTangent(mesh, controlPointIndex, vertexCounter, tangent);
				readBinormal(mesh, controlPointIndex, vertexCounter, binormal);
				readUV(mesh, controlPointIndex, mesh->GetTextureUVIndex(i, j, fbxsdk::FbxLayerElement::eTextureDiffuse)/*vertexCounter*/, uv);

				// DirectX기준으로 UV좌표 맞춰줌
				uv[0] = uv[0];
				uv[1] = 1.0 - uv[1];

				//if (vertexCounter == 0 || vertexCounter ==1)
				//{
				//	Trace::Write("TAG_DEBUG", "u", uv[0]);
				//	Trace::Write("TAG_DEBUG", "v", uv[1]);

				//}

				//vertices.push_back(vertex);
				// 만약에 스켈레톤 본이 없는 경우 크기가 0인 본들의 가중치 객체로 초기화 된다.
				insertVertex(position, normal, uv, binormal, tangent, weightOfBonesArray[controlPointIndex]);

				//디버깅

				//Trace::Write("TAG_DEBUG", "controlPoint Index", controlPointIndex);
				//vector<pair<int, float>> & tempArray = weightOfBonesArray[controlPointIndex].getAllInfo();

				//for (int l = 0; l < tempArray.size(); ++l)
				//{
				//	Trace::Write("TAG_DEBUG", "l", l);
				//	Trace::Write("TAG_DEBUG", "boneIndex", tempArray[l].first);

				//	Trace::Write("TAG_DEBUG", "weight", tempArray[l].second);

				//	//FbxModelSkeletonBone * bone = skeletonBones->findBoneWithBoneIndex(tempArray[l].first);
				//	//if (bone)
				//	//{
				//	//	Trace::Write("TAG_DEBUG", "boneIndex", bone->getBoneIndex());
				//	//	Trace::Write("TAG_DEBUG", "boneName", bone->getBoneName());

				//	//	Trace::Write("TAG_DEBUG", "parentBoneIndex", bone->getParentBoneIndex());
				//	//	Trace::Write("TAG_DEBUG", "parentBoneName", bone->getParentBoneName());
				//	//}
				//}

				//Trace::Write("TAG_DEBUG", "", "");


				++vertexCounter;
			}
		}
		
		globalTransformMatrix = node->EvaluateGlobalTransform(FbxTime(0));
		geometryTransformMatrix = getGeometryTransformation(node);

		//else if (polygonSize == 4)
		//{
		//	// 사각형을 그려줘야 한다.
		//	// 먼저 삼각형하나를 그려주고
		//	for (unsigned int j = 0; j < 3; ++j)
		//	{
		//		int controlPointIndex = mesh->GetPolygonVertex(i, j);
		//
		//		// 각각의 정보를 초기화 시켜준다.
		//		// 만약에 내부에 정보가 없으면 모두 0으로 들어간다.
		//		position = { 0,0,0 };
		//		normal = { 0,0,0 };
		//		binormal = { 0,0,0 };
		//		tangent = { 0,0,0 };
		//		uv = { 0,0 };
		//
		//		// 내부 레퍼런스 모드에 따라서 버텍스 카운터가 필요할 수도 있고
		//		// 컨트롤 포인트 인덱스가 필요할 수도 있다.
		//		position = controlPoints[controlPointIndex];
		//		readNormal(mesh, controlPointIndex, vertexCounter, normal);
		//		readTangent(mesh, controlPointIndex, vertexCounter, tangent);
		//		readBinormal(mesh, controlPointIndex, vertexCounter, binormal);
		//		readUV(mesh, controlPointIndex, mesh->GetTextureUVIndex(i, j), uv);
		//
		//		//vertices.push_back(vertex);
		//		insertVertex(position, normal, uv, binormal, tangent);
		//		++vertexCounter;
		//	}
		//
		//	// 일단 4개기준으로 vertexCounter초기화 해준다
		//	++vertexCounter;
		//
		//	// 나머지 삼각형하나를 그려준다.
		//	for (unsigned int j = 2; j != 1; (++j)%=4)
		//	{
		//		int controlPointIndex = mesh->GetPolygonVertex(i, j);
		//
		//		// 각각의 정보를 초기화 시켜준다.
		//		// 만약에 내부에 정보가 없으면 모두 0으로 들어간다.
		//		position = { 0,0,0 };
		//		normal = { 0,0,0 };
		//		binormal = { 0,0,0 };
		//		tangent = { 0,0,0 };
		//		uv = { 0,0 };
		//
		//		vertexCounter += - 4 + (j);
		//		// 내부 레퍼런스 모드에 따라서 버텍스 카운터가 필요할 수도 있고
		//		// 컨트롤 포인트 인덱스가 필요할 수도 있다.
		//		position = controlPoints[controlPointIndex];
		//		readNormal(mesh, controlPointIndex, vertexCounter, normal);
		//		readTangent(mesh, controlPointIndex, vertexCounter, tangent);
		//		readBinormal(mesh, controlPointIndex, vertexCounter, binormal);
		//		readUV(mesh, controlPointIndex, mesh->GetTextureUVIndex(i, j), uv);
		//		vertexCounter +=  4 - (j);
		//
		//
		//		//vertices.push_back(vertex);
		//		insertVertex(position, normal, uv, binormal, tangent);
		//
		//	}
		//}
		//
		//}


	}
}

void FbxModelMesh::readNormal(FbxMesh * mesh, int controlPointIndex, int vertexCounter, FbxDouble3 & outNormal)
{
	if (!mesh) return;
	if (mesh->GetElementNormalCount() < 1)
	{
		return;
	}

	FbxGeometryElementNormal* vertexNormal = mesh->GetElementNormal(0);
	switch (vertexNormal->GetMappingMode())
	{
	case FbxGeometryElement::eByControlPoint:
		switch (vertexNormal->GetReferenceMode())
		{
		case FbxGeometryElement::eDirect:
		{
			outNormal[0] = static_cast<float>(vertexNormal->GetDirectArray().GetAt(controlPointIndex).mData[0]);
			outNormal[1] = static_cast<float>(vertexNormal->GetDirectArray().GetAt(controlPointIndex).mData[1]);
			outNormal[2] = static_cast<float>(vertexNormal->GetDirectArray().GetAt(controlPointIndex).mData[2]);
		}
		break;

		case FbxGeometryElement::eIndexToDirect:
		{
			int index = vertexNormal->GetIndexArray().GetAt(controlPointIndex);
			outNormal[0] = static_cast<float>(vertexNormal->GetDirectArray().GetAt(index).mData[0]);
			outNormal[1] = static_cast<float>(vertexNormal->GetDirectArray().GetAt(index).mData[1]);
			outNormal[2] = static_cast<float>(vertexNormal->GetDirectArray().GetAt(index).mData[2]);
		}
		break;

		default:
			return;
		}
		break;

	case FbxGeometryElement::eByPolygonVertex:
		switch (vertexNormal->GetReferenceMode())
		{
		case FbxGeometryElement::eDirect:
		{
			outNormal[0] = static_cast<float>(vertexNormal->GetDirectArray().GetAt(vertexCounter).mData[0]);
			outNormal[1] = static_cast<float>(vertexNormal->GetDirectArray().GetAt(vertexCounter).mData[1]);
			outNormal[2] = static_cast<float>(vertexNormal->GetDirectArray().GetAt(vertexCounter).mData[2]);
		}
		break;

		case FbxGeometryElement::eIndexToDirect:
		{
			int index = vertexNormal->GetIndexArray().GetAt(vertexCounter);
			outNormal[0] = static_cast<float>(vertexNormal->GetDirectArray().GetAt(index).mData[0]);
			outNormal[1] = static_cast<float>(vertexNormal->GetDirectArray().GetAt(index).mData[1]);
			outNormal[2] = static_cast<float>(vertexNormal->GetDirectArray().GetAt(index).mData[2]);
		}
		break;

		default:
			return;
		}
		break;
	}
}

void FbxModelMesh::readTangent(FbxMesh * mesh, int controlPointIndex, int vertexCounter, FbxDouble3 & outTangent)
{
	if (!mesh) return;
	if (mesh->GetElementTangentCount() < 1)
	{
		return;
	}

	FbxGeometryElementTangent* vertexTangent = mesh->GetElementTangent(0);

	switch (vertexTangent->GetMappingMode())
	{
	case FbxGeometryElement::eByControlPoint:
		switch (vertexTangent->GetReferenceMode())
		{
		case FbxGeometryElement::eDirect:
		{
			outTangent[0] = static_cast<float>(vertexTangent->GetDirectArray().GetAt(controlPointIndex).mData[0]);
			outTangent[1] = static_cast<float>(vertexTangent->GetDirectArray().GetAt(controlPointIndex).mData[1]);
			outTangent[2] = static_cast<float>(vertexTangent->GetDirectArray().GetAt(controlPointIndex).mData[2]);
		}
		break;

		case FbxGeometryElement::eIndexToDirect:
		{
			int index = vertexTangent->GetIndexArray().GetAt(controlPointIndex);
			outTangent[0] = static_cast<float>(vertexTangent->GetDirectArray().GetAt(index).mData[0]);
			outTangent[1] = static_cast<float>(vertexTangent->GetDirectArray().GetAt(index).mData[1]);
			outTangent[2] = static_cast<float>(vertexTangent->GetDirectArray().GetAt(index).mData[2]);
		}
		break;

		default:
			return;
		}
		break;

	case FbxGeometryElement::eByPolygonVertex:
		switch (vertexTangent->GetReferenceMode())
		{
		case FbxGeometryElement::eDirect:
		{
			outTangent[0] = static_cast<float>(vertexTangent->GetDirectArray().GetAt(vertexCounter).mData[0]);
			outTangent[1] = static_cast<float>(vertexTangent->GetDirectArray().GetAt(vertexCounter).mData[1]);
			outTangent[2] = static_cast<float>(vertexTangent->GetDirectArray().GetAt(vertexCounter).mData[2]);
		}
		break;

		case FbxGeometryElement::eIndexToDirect:
		{
			int index = vertexTangent->GetIndexArray().GetAt(vertexCounter);
			outTangent[0] = static_cast<float>(vertexTangent->GetDirectArray().GetAt(index).mData[0]);
			outTangent[1] = static_cast<float>(vertexTangent->GetDirectArray().GetAt(index).mData[1]);
			outTangent[2] = static_cast<float>(vertexTangent->GetDirectArray().GetAt(index).mData[2]);
		}
		break;

		default:
			return;
		}
		break;
	}
}

void FbxModelMesh::readBinormal(FbxMesh * mesh, int controlPointIndex, int vertexCounter, FbxDouble3 & outbinormal)
{
	if (!mesh) return;
	if (mesh->GetElementBinormalCount() < 1)
	{
		return;
	}

	FbxGeometryElementBinormal* vertexBinormal = mesh->GetElementBinormal(0);

	switch (vertexBinormal->GetMappingMode())
	{
	case FbxGeometryElement::eByControlPoint:
		switch (vertexBinormal->GetReferenceMode())
		{
		case FbxGeometryElement::eDirect:
		{
			outbinormal[0] = static_cast<float>(vertexBinormal->GetDirectArray().GetAt(controlPointIndex).mData[0]);
			outbinormal[1] = static_cast<float>(vertexBinormal->GetDirectArray().GetAt(controlPointIndex).mData[1]);
			outbinormal[2] = static_cast<float>(vertexBinormal->GetDirectArray().GetAt(controlPointIndex).mData[2]);
		}
		break;

		case FbxGeometryElement::eIndexToDirect:
		{
			int index = vertexBinormal->GetIndexArray().GetAt(controlPointIndex);
			outbinormal[0] = static_cast<float>(vertexBinormal->GetDirectArray().GetAt(index).mData[0]);
			outbinormal[1] = static_cast<float>(vertexBinormal->GetDirectArray().GetAt(index).mData[1]);
			outbinormal[2] = static_cast<float>(vertexBinormal->GetDirectArray().GetAt(index).mData[2]);
		}
		break;

		default:
			return;
		}
		break;

	case FbxGeometryElement::eByPolygonVertex:
		switch (vertexBinormal->GetReferenceMode())
		{
		case FbxGeometryElement::eDirect:
		{
			outbinormal[0] = static_cast<float>(vertexBinormal->GetDirectArray().GetAt(vertexCounter).mData[0]);
			outbinormal[1] = static_cast<float>(vertexBinormal->GetDirectArray().GetAt(vertexCounter).mData[1]);
			outbinormal[2] = static_cast<float>(vertexBinormal->GetDirectArray().GetAt(vertexCounter).mData[2]);
		}
		break;

		case FbxGeometryElement::eIndexToDirect:
		{
			int index = vertexBinormal->GetIndexArray().GetAt(vertexCounter);
			outbinormal[0] = static_cast<float>(vertexBinormal->GetDirectArray().GetAt(index).mData[0]);
			outbinormal[1] = static_cast<float>(vertexBinormal->GetDirectArray().GetAt(index).mData[1]);
			outbinormal[2] = static_cast<float>(vertexBinormal->GetDirectArray().GetAt(index).mData[2]);
		}
		break;

		default:
			return;
		}
		break;
	}
}

void FbxModelMesh::readUV(FbxMesh * mesh, int controlPointIndex, int vertexCounter, FbxDouble2 & outUV)
{
	if (!mesh) return;
	if (mesh->GetElementUVCount() < 1)
	{
		return;
	}

	
	FbxGeometryElementUV * vertexUV = mesh->GetElementUV(0); // mesh->GetLayer(0)->GetUVs();

	switch (vertexUV->GetMappingMode())
	{
	case FbxGeometryElement::eByControlPoint:
		switch (vertexUV->GetReferenceMode())
		{
			case FbxGeometryElement::eDirect:
			{
				outUV[0] = static_cast<float>(vertexUV->GetDirectArray().GetAt(controlPointIndex).mData[0]);
				outUV[1] = static_cast<float>(vertexUV->GetDirectArray().GetAt(controlPointIndex).mData[1]);
			}
			break;

			case FbxGeometryElement::eIndexToDirect:
			{
				int index = vertexUV->GetIndexArray().GetAt(controlPointIndex);
				outUV[0] = static_cast<float>(vertexUV->GetDirectArray().GetAt(index).mData[0]);
				outUV[1] = static_cast<float>(vertexUV->GetDirectArray().GetAt(index).mData[1]);
			}
			break;

		}
		break;

	case FbxGeometryElement::eByPolygonVertex:
		switch (vertexUV->GetReferenceMode())
		{
			case FbxGeometryElement::eDirect:
			{
				outUV[0] = static_cast<float>(vertexUV->GetDirectArray().GetAt(vertexCounter).mData[0]);
				outUV[1] = static_cast<float>(vertexUV->GetDirectArray().GetAt(vertexCounter).mData[1]);
			}
			break;

			case FbxGeometryElement::eIndexToDirect:
			{
				outUV[0] = static_cast<float>(vertexUV->GetDirectArray().GetAt(vertexCounter).mData[0]);
				outUV[1] = static_cast<float>(vertexUV->GetDirectArray().GetAt(vertexCounter).mData[1]);
				//int index = vertexUV->GetIndexArray().GetAt(vertexCounter);
				//outUV[0] = static_cast<float>(vertexUV->GetDirectArray().GetAt(index).mData[0]);
				//outUV[1] = static_cast<float>(vertexUV->GetDirectArray().GetAt(index).mData[1]);
			}
			break;

		}
		break;
	}
	//// DirectX기준으로 UV좌표 맞춰줌
	////if (outUV[0] == 0.0f && outUV[1] == 0.0f) 
	//{
	//	outUV[0] = outUV[0];
	//	outUV[1] = 1.0 - outUV[1];
	//}
}



