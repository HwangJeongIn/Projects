
//
// Globals
//

extern float4x4 WorldViewProjectionMatrix;

//
// Structures
//

struct VS_INPUT
{
    vector position : POSITION0;
    vector normal   : NORMAL0;
	float2 uv   : TEXCOORD0;

	//float4 aniRow1   : NORMAL1;
	//float4 aniRow2   : NORMAL2;
	//float4 aniRow3   : NORMAL3;
	//float4 aniRow4   : NORMAL4;
};

struct VS_OUTPUT
{
    vector position : POSITION0;
    //vector normal   : NORMAL0;
	float2 uv       : TEXCOORD0;
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

	//finalPosition =  mul(input.position, animationMatrix);
	// ���� > �� > �������� ������� ���ؼ� ���� ���������� ���ؼ� ������ ������ ����� ����
	// ��İ��� ���ش�.
    // output.position = mul(finalPosition, WorldViewProjectionMatrix);
	output.position = mul(input.position, WorldViewProjectionMatrix);



    return output;
}





