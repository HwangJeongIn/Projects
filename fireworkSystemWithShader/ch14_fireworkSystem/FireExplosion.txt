
//
// Globals
//

//static vector defaultNormal = {1.0f, 0.0f, 0.0f, 0.0f};
//static vector Black = {1.0f, 1.0f, 1.0f, 1.0f};

//extern matrix WorldViewMatrix;
extern matrix ViewProjectionMatrix;

//extern vector Color;

//extern vector LightDirection;

//
// Structures
//

struct VS_INPUT
{
    vector position : POSITION;
    vector diffuse   : COLOR0;
};

struct VS_OUTPUT
{
    vector position : POSITION;
    vector diffuse  : COLOR0;
    vector specular   : COLOR1;
};


//
// Main
//

VS_OUTPUT Main(VS_INPUT input)
{
    // zero out each member in output
    VS_OUTPUT output = (VS_OUTPUT)0;


    // transform vertex position to homogenous clip space
     output.position = mul(input.position, ViewProjectionMatrix);

    //
    // Transform lights and normals to view space.  Set w
    // components to zero since we're transforming vectors.
    // Assume there are no scalings in the world
    // matrix as well.
    //
    //LightDirection.w = 0.0f;
    //input.normal.w   = 0.0f;

	// 월드 기준으로 변환해준다.
   // LightDirection   = mul(LightDirection, WorldViewMatrix);
    //input.normal     = mul(input.normal, WorldViewMatrix);

    //
    // Compute the 1D texture coordinate for toon rendering.
    //
	// 법선벡터와 방향의 내적값을 구해준다.
 //   float u = dot(LightDirection, input.normal);

    //
    // Clamp to zero if u is negative because u
    // negative implies the angle between the light
    // and normal is greater than 90 degrees.  And
    // if that is true then the surface receives
    // no light.
    //

	// 만약에 사이각이 90도보다 크다면 0으로 지정한다.
  //  if( u < 0.0f )
 //       u = 0.0f;

    // 
    // Set other tex coord to middle.
    //
	
 //   float v = 0.5f;

	// uv좌표를 찍어준다.
	// 여기서 텍스처 자체를 3가지 칼라만 가진 이미지로 해주었다.
	// 그리고 셰이더는 이 이미지의 uv좌표값을 계산해준다. 세로로 3가지 음영이 분리되기 때문에
	// v값은 .5로 고정하고 // 어떤값이든 상관없음
	// u값은 0-1의값(법선벡터와 빛의 방향의 내적) 이므로 값이 범위에 따라 3가지 음영으로 분리된다.
 //   output.uvCoords.x = u;
 //   output.uvCoords.y = v;

    // save color
	// 

	// 기본적인 반사를 계산할때는 이런식으로 계산해준다.
	// 환경광과 내적에 따라서 난반사광을 계산해준다. 이때 내적값은 0~1까지 연속값을 가지게 때문에 부드러운 렌더링이 된다.
	// output.diffuse = (AmbientMtrl * AmbientLightIntensity) + (s * (DiffuseLightIntensity * DiffuseMtrl));

	// 자연스러운 렌더링은 여러가지 계산(위의식)
	// 여기서는 그냥 색은 일정하게 지정해두고 빛과 법선벡터의 사이각에 따라서 UV좌표값만 조정해준다.
	// 이 UV좌표에 따라서 음영이 3가지로 결정된다. // 정확히는 U의 좌표
    output.diffuse = input.diffuse;
	//output.specular = Black;
   // output.normal = defaultNormal;

    return output;
}





