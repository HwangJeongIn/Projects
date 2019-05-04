
//
// Globals
//

//static vector defaultNormal = {1.0f, 0.0f, 0.0f, 0.0f};
//static vector Black = {1.0f, 1.0f, 1.0f, 1.0f};

//extern matrix WorldViewMatrix;
extern matrix ViewProjectionMatrix;
extern sampler Texture;

//extern vector Color;

//extern vector LightDirection;

//
// Structures
//

struct VS_INPUT
{
    vector position : POSITION;
    float4 diffuse   : COLOR0;
};

struct VS_OUTPUT
{
    vector position : POSITION;
    float4 diffuse  : COLOR0;
    float4 specular   : COLOR1;
};


//
// Main
//

VS_OUTPUT Main(VS_INPUT input)
{
    // zero out each member in output
    VS_OUTPUT output = (VS_OUTPUT)0;


    output.position = mul(input.position, ViewProjectionMatrix);
    output.diffuse = input.diffuse;
	// output.diffuse.w = 0.3;
	  output.specular = input.diffuse;
	 // output.specular.w = 0.3;
    return output;
}





