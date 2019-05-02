

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

	// �ؽ�ó��ǥ�� �״�� ��ȯ�Ѵ�.
	output.uv = input.uv;


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

	//************************ ���� ��ġ*************************
	output.position.w = 1;
	// �ִϸ��̼� ��İ�
	output.position =  mul(input.position, animationMatrix);

	// ������ ��������
	output.position.x *=0.1f * ScaleFactor.x;
	output.position.y *=0.1f * ScaleFactor.y;
	output.position.z *=0.1f * ScaleFactor.z;

	// ���� > �� > �������� ������� ��
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
	//************************ �� ��� *************************
	// �� > ī�޶� ����
	float4 modelToCameraDirection = CameraPosition.xyzw - output.position.xyzw;
	modelToCameraDirection.w = 0;
	modelToCameraDirection = normalize(modelToCameraDirection);

	// ���� �븻���͸� �ִϸ��̼� ��İ� ��������� ���ؼ� DirectX�������Ҷ��� �븻�� �ٲ��ش�.
	input.normal.w = 0;
	input.normal = mul(input.normal, animationMatrix);
	input.normal = mul(input.normal, WorldMatrix);
	input.normal = normalize(input.normal);


	output.diffuse = float4(1.0, 1.0, 1.0, 1.0);//AmbientColor;//float4(0,0,0,0);
	output.specular = float4(0.001, 0.001, 0.001, 1.0);
	
	float4 lightDir = -LightDirection;
	lightDir = normalize(lightDir);
	

	// ���� ���⸦ ���� �����ش�
	// �̴� ���̰��� x�� �� �������� �������� ���翵 ���̰� ���´�
	float lightIntensity = saturate(dot(input.normal, lightDir));


	if(lightIntensity > 0.0f)
	{
		output.diffuse = output.diffuse * lightIntensity;
		// ���翵���̿� normal * 2�� -lightDirection�� ����ϸ� �ݻ纤�͸� ���� �� �ִ�.
		float4 reflection = normalize(2 * lightIntensity * input.normal - lightDir);
		output.specular = 0.001 * output.specular * saturate( dot(reflection, modelToCameraDirection) );
	}

	/*

	if(lightIntensity > 0.0f)
	{
		// ���� diffuse ���
		output.diffuse += (DiffuseColor * lightIntensity);
		output.diffuse = saturate(output.diffuse);

		// ���� scecular���
		float4 reflection = normalize(2 * lightIntensity * input.normal - lightDirection);

		output.specular = pow(saturate(dot(reflection, modelToCameraDirection)), SpecularPower);
		output.specular = saturate(output.specular);
	}
	*/
	

    return output;
}