
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

	// 노멀과 텍스처좌표는 그대로 반환한다.
	//output.normal = input.normal;
	output.uv = input.uv;

	//vector finalPosition;

	// 애니메이션 행렬 곱해준다.
	//float4x4 animationMatrix;
	//animationMatrix[0] = input.aniRow1;
	//animationMatrix[1] = input.aniRow2;
	//animationMatrix[2] = input.aniRow3;
	//animationMatrix[3] = input.aniRow4;

	//finalPosition =  mul(input.position, animationMatrix);
	// 월드 > 뷰 > 프로젝션 순서대로 곱해서 들어온 전역변수를 통해서 마지막 포지션 계산을 위한
	// 행렬곱을 해준다.
    // output.position = mul(finalPosition, WorldViewProjectionMatrix);
	output.position = mul(input.position, WorldViewProjectionMatrix);



    return output;
}





