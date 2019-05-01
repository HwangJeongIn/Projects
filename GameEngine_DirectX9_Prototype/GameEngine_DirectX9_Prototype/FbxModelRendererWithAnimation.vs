
//
// Globals
//

extern float4x4 WorldViewProjectionMatrix;
extern vector ScaleFactor;
//
// Structures
//

struct VS_INPUT
{
    vector position : POSITION0;
    vector normal   : NORMAL0;
	float2 uv   : TEXCOORD0;

	float4 aniRow1   : NORMAL1;
	float4 aniRow2   : NORMAL2;
	float4 aniRow3   : NORMAL3;
	float4 aniRow4   : NORMAL4;
};

struct VS_OUTPUT
{
    vector position : POSITION0;
    //vector normal   : NORMAL0;
	float2 uv       : TEXCOORD0;
	//vector diffuse : COLOR0;
};


//
// Main
//

VS_OUTPUT Main(VS_INPUT input)
{
    // zero out each member in output
    VS_OUTPUT output = (VS_OUTPUT)0;

	// ��ְ� �ؽ�ó��ǥ�� �״�� ��ȯ�Ѵ�.
	//output.normal = input.normal;
	output.uv = input.uv;

	//vector finalPosition;

	// �ִϸ��̼� ��� �����ش�.
	//float4x4 animationMatrix;
	//animationMatrix[0] = input.aniRow1;
	//animationMatrix[1] = input.aniRow2;
	//animationMatrix[2] = input.aniRow3;
	//animationMatrix[3] = input.aniRow4;
	float4x4 animationMatrix = 
	{
		input.aniRow1.x, input.aniRow1.y, input.aniRow1.z, input.aniRow1.w,
		input.aniRow2.x, input.aniRow2.y, input.aniRow2.z, input.aniRow2.w,
		input.aniRow3.x, input.aniRow3.y, input.aniRow3.z, input.aniRow3.w,
		input.aniRow4.x, input.aniRow4.y, input.aniRow4.z, input.aniRow4.w
	
	};

	// ���� �ִϸ��̼� ��ĺ��� ���ؼ� ���Ѵ�.
	output.position =  mul(input.position, animationMatrix);

	// ������ ��������
	output.position.x *=0.1f * ScaleFactor.x;
	output.position.y *=0.1f * ScaleFactor.y;
	output.position.z *=0.1f * ScaleFactor.z;

	// ���� > �� > �������� ������� ���ؼ� ���� ���������� ���ؼ� ������ ������ ����� ����
	// ��İ��� ���ش�.
	output.position = mul(output.position, WorldViewProjectionMatrix);



	//output.diffuse = vector(1,0,0,1);

    return output;
}