#include "FbxInfo.h"

void FbxInfo::readNormal(FbxMesh * mesh, int controlPointIndex, int vertexCounter, FbxDouble3 & outNormal)
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

void FbxInfo::readTangent(FbxMesh * mesh, int controlPointIndex, int vertexCounter, FbxDouble3 & outTangent)
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

void FbxInfo::readBinormal(FbxMesh * mesh, int controlPointIndex, int vertexCounter, FbxDouble3 & outbinormal)
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

void FbxInfo::readUV(FbxMesh * mesh, int controlPointIndex, int vertexCounter, FbxDouble2 & outUV)
{
	if (!mesh) return;
	if (mesh->GetElementUVCount() < 1)
	{
		return;
	}

	FbxGeometryElementUV * vertexUV = mesh->GetElementUV(0);

	switch (vertexUV->GetMappingMode())
	{
	case FbxGeometryElement::eByControlPoint:
		switch (vertexUV->GetReferenceMode())
		{
		case FbxGeometryElement::eDirect:
		{
			outUV[0] = static_cast<float>(vertexUV->GetDirectArray().GetAt(controlPointIndex).mData[0]);
			outUV[1] = static_cast<float>(vertexUV->GetDirectArray().GetAt(controlPointIndex).mData[1]);
			//outUV[2] = static_cast<float>(vertexUV->GetDirectArray().GetAt(controlPointIndex).mData[2]);
		}
		break;

		case FbxGeometryElement::eIndexToDirect:
		{
			int index = vertexUV->GetIndexArray().GetAt(controlPointIndex);
			outUV[0] = static_cast<float>(vertexUV->GetDirectArray().GetAt(index).mData[0]);
			outUV[1] = static_cast<float>(vertexUV->GetDirectArray().GetAt(index).mData[1]);
			//outUV[2] = static_cast<float>(vertexUV->GetDirectArray().GetAt(index).mData[2]);
		}
		break;

		default:
			return;
		}
		break;

	case FbxGeometryElement::eByPolygonVertex:
		switch (vertexUV->GetReferenceMode())
		{
		case FbxGeometryElement::eDirect:
		{
			outUV[0] = static_cast<float>(vertexUV->GetDirectArray().GetAt(vertexCounter).mData[0]);
			outUV[1] = static_cast<float>(vertexUV->GetDirectArray().GetAt(vertexCounter).mData[1]);
			//outUV[2] = static_cast<float>(vertexUV->GetDirectArray().GetAt(vertexCounter).mData[2]);
		}
		break;

		case FbxGeometryElement::eIndexToDirect:
		{
			int index = vertexUV->GetIndexArray().GetAt(vertexCounter);
			outUV[0] = static_cast<float>(vertexUV->GetDirectArray().GetAt(index).mData[0]);
			outUV[1] = static_cast<float>(vertexUV->GetDirectArray().GetAt(index).mData[1]);
			//outUV[2] = static_cast<float>(vertexUV->GetDirectArray().GetAt(index).mData[2]);
		}
		break;

		default:
			return;
		}
		break;
	}
}



