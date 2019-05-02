

extern float4x4 ViewProjectionMatrix;
extern float4x4 WorldMatrix;
extern float4 CameraPosition;
extern float4 ScaleFactor;
extern sampler Texture;



static float4 LightDirection = {1.0, -1.0, 1.0, 0.0};
static float4 AmbientColor = {0.15, 0.15, 0.15, 1.0};
static float4 DiffuseColor = {1.0, 1.0, 1.0, 1.0};
static float4 SpecularColor = {1.0, 1.0, 1.0, 1.0};
static float SpecularPower = 1.0;



struct VS_INPUT
{
    float4 position : POSITION0;
    vector normal   : NORMAL0;
	float2 uv   : TEXCOORD0;

	float4 aniRow1   : NORMAL1;
	float4 aniRow2   : NORMAL2;
	float4 aniRow3   : NORMAL3;
	float4 aniRow4   : NORMAL4;
};

struct VS_OUTPUT
{
    float4 position : POSITION0;
    //vector normal   : NORMAL0;
	float2 uv       : TEXCOORD0;
	float4 diffuse : COLOR0;
	float4 specular : COLOR1;
};



VS_OUTPUT Main(VS_INPUT input)
{
    // zero out each member in output
    VS_OUTPUT output = (VS_OUTPUT)0;

	// 텍스처좌표는 그대로 반환한다.
	output.uv = input.uv;


	// 애니메이션 행렬 곱해준다.
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

	//************************ 최종 위치*************************
	output.position.w = 1;
	// 애니메이션 행렬곱
	output.position =  mul(input.position, animationMatrix);

	// 스케일 조절가능
	output.position.x *=0.1f * ScaleFactor.x;
	output.position.y *=0.1f * ScaleFactor.y;
	output.position.z *=0.1f * ScaleFactor.z;

	// 월드 > 뷰 > 프로젝션 순서대로 곱
	output.position = mul(output.position, WorldMatrix);
	output.position = mul(output.position, ViewProjectionMatrix);


	/*
	output.specular  = float4(0,0,0,0);
	if(input.normal.x != 0.0f)
	{
		output.diffuse = float4(1,1,1,1);
		output.specular += float4(1,0,0,0);
	}

	if(input.normal.y != 0.0f)
	{
	output.diffuse = float4(1,1,1,1);
		output.specular += float4(0,0,1,0);
	}

	if(input.normal.z != 0.0f)
	{	
	output.diffuse = float4(1,1,1,1);
		output.specular += float4(0,1,0,0);
	}


	return output;
	*/
	//************************ 빛 계산 *************************
	// 모델 > 카메라 벡터
	float4 modelToCameraDirection = CameraPosition.xyzw - output.position.xyzw;
	modelToCameraDirection.w = 0;
	modelToCameraDirection = normalize(modelToCameraDirection);

	// 먼저 노말벡터를 애니메이션 행렬과 월드행렬을 곱해서 DirectX렌더링할때의 노말로 바꿔준다.
	input.normal.w = 0;
	input.normal = mul(input.normal, animationMatrix);
	input.normal = mul(input.normal, WorldMatrix);
	input.normal = normalize(input.normal);


	output.diffuse = float4(1.0, 1.0, 1.0, 1.0);//AmbientColor;//float4(0,0,0,0);
	output.specular = float4(0.001, 0.001, 0.001, 1.0);
	
	float4 lightDir = -LightDirection;
	lightDir = normalize(lightDir);
	

	// 빛의 세기를 먼저 구해준다
	// 이는 사이각이 x인 두 단위벡터 기준으로 정사영 길이가 나온다
	float lightIntensity = saturate(dot(input.normal, lightDir));


	if(lightIntensity > 0.0f)
	{
		output.diffuse = output.diffuse * lightIntensity;
		// 정사영길이에 normal * 2와 -lightDirection을 계산하면 반사벡터를 구할 수 있다.
		float4 reflection = normalize(2 * lightIntensity * input.normal - lightDir);
		output.specular = 0.001 * output.specular * saturate( dot(reflection, modelToCameraDirection) );
	}

	/*

	if(lightIntensity > 0.0f)
	{
		// 최종 diffuse 계산
		output.diffuse += (DiffuseColor * lightIntensity);
		output.diffuse = saturate(output.diffuse);

		// 최종 scecular계산
		float4 reflection = normalize(2 * lightIntensity * input.normal - lightDirection);

		output.specular = pow(saturate(dot(reflection, modelToCameraDirection)), SpecularPower);
		output.specular = saturate(output.specular);
	}
	*/
	

    return output;
}