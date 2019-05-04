//////////////////////////////////////////////////////////////////////////////////////////////////
// 
// File: pSystem.cpp
// 
// Author: Frank Luna (C) All Rights Reserved
//
// System: AMD Athlon 1800+ XP, 512 DDR, Geforce 3, Windows XP, MSVC++ 7.0 
//
// Desc: Represents a general particle system.
//          
//////////////////////////////////////////////////////////////////////////////////////////////////

#include "ParticleSystem.h"
#include "ctime"
#include "ShaderContainer.h"



const DWORD ParticleSystem::Particle::FVF = D3DFVF_XYZ | D3DFVF_DIFFUSE;


ParticleSystem::ParticleSystem(IDirect3DDevice9 * device, const char * textureFileName, int numOfParticles, int particleSize, const D3DXVECTOR3 & origin, float duration)
	: device(device), vb(nullptr), texture(nullptr), duration(duration), currentDuration(0.0f),/*declaration(nullptr),*/
	maxNumOfParticles(numOfParticles), currentNumOfParticles(0), particleSize(particleSize), origin(origin), amountFactor(0.5f)
{
	if (!device) return;

	srand((unsigned int)time(0));
	this->device->GetTransform(D3DTS_PROJECTION /*D3DTRANSFORMSTATETYPE*/, &projectionMatrix);
	this->device->GetTransform(D3DTS_VIEW, &viewMatrix);
	//this->viewMatrix = viewMatrix;
	this->particlesAttribute.resize(maxNumOfParticles);
	this->currentMaxNumOfParticles = maxNumOfParticles * amountFactor;


	init(textureFileName);
}


ParticleSystem::~ParticleSystem()
{
	if(vb)
		vb->Release();
	if (texture)
		texture->Release();

}

bool ParticleSystem::init(const char * texFileName)
{
	HRESULT hr = 0;

	hr = device->CreateVertexBuffer(
		maxNumOfParticles * sizeof(Particle),
		D3DUSAGE_DYNAMIC | D3DUSAGE_POINTS | D3DUSAGE_WRITEONLY,
		Particle::FVF,
		D3DPOOL_DEFAULT, // D3DPOOL_MANAGED can't be used with D3DUSAGE_DYNAMIC 
		&vb,
		0);

	if (FAILED(hr))
	{
		::MessageBox(0, "CreateVertexBuffer() - FAILED", "PSystem", 0);
		return false;
	}

	hr = D3DXCreateTextureFromFile(
		device,
		texFileName,
		&texture);

	if (FAILED(hr))
	{
		::MessageBox(0, "D3DXCreateTextureFromFile() - FAILED", "PSystem", 0);
		return false;
	}

	return true;
}

void ParticleSystem::resetAllParticles()
{
	for (int i = 0; i < currentMaxNumOfParticles; ++i)
	{
		resetParticle(particlesAttribute[i]);
	}
}



void ParticleSystem::addParticle()
{
	if (currentMaxNumOfParticles <= currentNumOfParticles) return;

	//ParticleSystem::ParticleAttribute particleAttribute;
	//resetParticle(particleAttribute);
	//particlesAttribute[currentNumOfParticles++] = particleAttribute;
	resetParticle(particlesAttribute[currentNumOfParticles]);
	currentNumOfParticles++;
}

void ParticleSystem::removeParticle(int index)
{
	if (currentNumOfParticles <= 0 || index < 0 || index >=currentNumOfParticles) return;

	// �����ε��� ��ƼŬ�� ���帶���� Ȱ��ȭ ��ƼŬ�� �������ش�.
	ParticleSystem::ParticleAttribute attribute = particlesAttribute[index];
	particlesAttribute[index] = particlesAttribute[--currentNumOfParticles];
	particlesAttribute[currentNumOfParticles] = attribute;

}


void ParticleSystem::preRender()
{
	device->SetRenderState(D3DRS_LIGHTING, true);

	// true�� ��� ��ü �ؽ�ó�� ����Ʈ ��������Ʈ�� �ؽ�ó ���ο� �̿�
	device->SetRenderState(D3DRS_POINTSPRITEENABLE, true);

	// true�� ��� ����Ʈ ũ�⸦ �� �����̽� ������ �ؼ��ϵ��� ����
	// ����Ʈ ��������Ʈ�� ũ��� ī�޶���� �Ÿ��� ���� �����ϰ� �����ȴ�.
	device->SetRenderState(D3DRS_POINTSCALEENABLE, true);


	// ����Ʈ ��������Ʈ�� ũ�� 
	// D3DRS_POINTSCALEENABLE�� ���� �佺���̽� ���� ũ�⳪ ��ũ�� �����̽� ���� ũ��� �ؼ�
	device->SetRenderState(D3DRS_POINTSIZE, *((DWORD*)&particleSize));

	float temp = 0.0f;
	// ����Ʈ ��������Ʈ�� ������ �� �ִ� �ּ�ũ�⸦ ����
	device->SetRenderState(D3DRS_POINTSIZE_MIN, *((DWORD*)&temp));

	// ������ ���� ���� ���� ũ�⸦ �����Ѵ�.
	device->SetRenderState(D3DRS_POINTSCALE_A, *((DWORD*)&temp));
	device->SetRenderState(D3DRS_POINTSCALE_B, *((DWORD*)&temp));
	temp = 1.0f;
	device->SetRenderState(D3DRS_POINTSCALE_C, *((DWORD*)&temp));

	// �ؽ�ó�� ���ĸ� �̿�
	device->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	device->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);

	// ���ĺ��� ���
	device->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
	device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
}

void ParticleSystem::postRender()
{

	device->SetRenderState(D3DRS_LIGHTING, true);
	device->SetRenderState(D3DRS_POINTSPRITEENABLE, false);
	device->SetRenderState(D3DRS_POINTSCALEENABLE, false);
	device->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
}



FireExplosion::FireExplosion(IDirect3DDevice9 * device, /*const D3DXMATRIX & viewMatrix,*/ const char * textureFileName, int numOfParticles, int particleSize, const D3DXVECTOR3 & origin)
	: ParticleSystem(device, /*viewMatrix,*/ textureFileName, numOfParticles, particleSize, origin), preExplosionFactor(0.1f)
	// shaderFileName("FireExplosion.vs"), viewProjectionMatrixName("ViewProjectionMatrix")
	//declaration(nullptr), fireExplosionShader(nullptr), constTable(nullptr), viewProjectionMatrixHandle(0)
{
	if (!device) return;
	resetAllParticles();
	//init();
}

bool FireExplosion::init()
{
	//HRESULT hr = 0;

	//D3DVERTEXELEMENT9 decl[] =
	//{
	//	/*
	//	���߿� ���̴� �ڵ��� ����ü
	//	struct VS_INPUT
	//	{
	//	vector positi : POSITION // POSITION0
	//	: NORMAL0
	//	: NORMAL1
	//	: NORMAL2
	//	}
	//	�� ���εȴ�.

	//	*/
	//	// offsets in bytes
	//	{ 0,  0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
	//	{ 0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR,   0 },
	//	//{ 0, 24, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL,   1 },
	//	//{ 0, 36, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL,   2 },
	//	D3DDECL_END()
	//};


	//hr = device->CreateVertexDeclaration(decl, &declaration);
	//if (FAILED(hr))
	//{
	//	::MessageBox(0, "CreateVertexDeclaration() - FAILED", 0, 0);
	//	return false;
	//}
	//

	//// ���̴� ������ / �ڵ� �޾ƿ���
	////------------------------------------------------------------------
	//ID3DXBuffer* compiledCode = 0;
	//ID3DXBuffer* errorBuffer = 0;

	//hr = D3DXCompileShaderFromFile(
	//	"FireExplosion.vs",
	//	0,
	//	0,
	//	"Main", // entry point function name
	//	"vs_1_1",

	//	D3DXSHADER_ENABLE_BACKWARDS_COMPATIBILITY | D3DXSHADER_DEBUG,
	//	&compiledCode,
	//	&errorBuffer,
	//	&constTable);

	//// output any error messages
	//if (errorBuffer)
	//{
	//	::MessageBox(0, (char*)errorBuffer->GetBufferPointer(), 0, 0);
	//	errorBuffer->Release();
	//}

	//if (FAILED(hr))
	//{
	//	::MessageBox(0, "D3DXCompileShaderFromFile() - FAILED", 0, 0);
	//	return false;
	//}

	//hr = device->CreateVertexShader(
	//	(DWORD*)compiledCode->GetBufferPointer(),
	//	&fireExplosionShader);

	//if (FAILED(hr))
	//{
	//	::MessageBox(0, "CreateVertexShader - FAILED", 0, 0);
	//	return false;
	//}

	//if (compiledCode)
	//	compiledCode->Release();

	//viewProjectionMatrixHandle = constTable->GetConstantByName(0, "ViewProjectionMatrix");

	return true;
}

void FireExplosion::resetParticle(ParticleAttribute & attribute)
{

	attribute.isAlive  = true;


	//D3DXVECTOR3 min = D3DXVECTOR3(-1.0f, -1.0f, -1.0f);
	//D3DXVECTOR3 max = D3DXVECTOR3( 1.0f,  1.0f,  1.0f);

	float durationFactor = currentDuration / duration;

	if (durationFactor <=preExplosionFactor)
	{
		attribute.position 
			= D3DXVECTOR3
			(
				getRandomFloat(-1, 1),
				getRandomFloat(-1, 1),
				getRandomFloat(-1, 1)
			);
		D3DXVec3Normalize(
			&attribute.position,
			&attribute.position);


		attribute.position *= getRandomFloat(0.1, 20);

		attribute.position += origin;

		// �������� ������ ���´�
		attribute.velocity = origin - attribute.position;

		D3DXVec3Normalize(
			&attribute.velocity,
			&attribute.velocity);

		attribute.velocity *= getRandomFloat(50, 100);
		attribute.lifeTime = preExplosionFactor * duration;//getRandomFloat(0.5 * duration, 0.5 * duration);// lives for 2 seconds
	}
	else
	{

		attribute.position = origin;
		// �������� ������ ���´�
		attribute.velocity
			= D3DXVECTOR3
			(
				getRandomFloat(-1, 1),
				getRandomFloat(-1, 1),
				getRandomFloat(-1, 1)
			);


		// normalize to make spherical
		// ����ȭ�Ѵ� ��� ������ �ӷ����� �������� �ؼ�
		D3DXVec3Normalize(
			&attribute.velocity,
			&attribute.velocity);

		attribute.velocity *= getRandomFloat(50, 150);
		attribute.lifeTime = (1- preExplosionFactor + 0.01f) * duration;//getRandomFloat(0.1f, 3.0f);// lives for 2 seconds

	}



	attribute.color = D3DXCOLOR(
		getRandomFloat(0.8f, 1.0f),
		getRandomFloat(0.0f, .3f),
		getRandomFloat(0.0f, .3f),
		.10f);

	attribute.colorFade = D3DXCOLOR(
		getRandomFloat(0.0f, .30f),
		getRandomFloat(0.0f, .30f),
		getRandomFloat(0.8f, 1.0f),
		.0f);

	// ���� 2�ʵ��� �����ϵ��� ����
	attribute.age      = 0.0f;

}

void FireExplosion::update(float deltaTime)
{
	if (!device) return;
	// ���� ����
	currentDuration += deltaTime;

	while (currentNumOfParticles < currentMaxNumOfParticles)
	{
		addParticle();
	}

	bool preExplosion = (currentDuration / duration) < preExplosionFactor;

	for (int i = 0; i < currentNumOfParticles; ++i)
	{
		// ����ִ� ��ƼŬ�� ���ؼ�
		// �ӵ� �ֽ�ȭ
		if (preExplosion)
		{
			D3DXVECTOR3 distanceVector3 = origin - particlesAttribute[i].position;
			float distance = D3DXVec3Length(&distanceVector3);
			if(distance > 3.0f)
				particlesAttribute[i].position += particlesAttribute[i].velocity * deltaTime;//(deltaTime /1000.0f);
		}
		else
			particlesAttribute[i].position += particlesAttribute[i].velocity * deltaTime;//(deltaTime /1000.0f);

		// �ð� �ֽ�ȭ
		particlesAttribute[i].age += deltaTime;

		if (particlesAttribute[i].age > particlesAttribute[i].lifeTime)
		{
			particlesAttribute[i].isAlive = false;

			removeParticle(i);
			//addParticle();
		}

	}

	// ���̴����� ����� ���� ������Ʈ
	ParticleSystem::Particle* v = 0;
	vb->Lock(
		0,
		currentNumOfParticles * sizeof(Particle),
		(void**)&v,
		0/*_vbOffset ? D3DLOCK_NOOVERWRITE : D3DLOCK_DISCARD*/);

	float lifeFactor = 0.0f;
	for (int i = 0; i < currentNumOfParticles; ++i)
	{
		v[i].position = particlesAttribute[i].position;

		// �����ؼ� �־��ش�.
		lifeFactor = particlesAttribute[i].age / particlesAttribute[i].lifeTime;
		v[i].color = D3DXCOLOR(particlesAttribute[i].color * (1 - lifeFactor) + particlesAttribute[i].colorFade * lifeFactor);

	}

	vb->Unlock();
}

void FireExplosion::render()
{
	if (!device) return;

	// ���̴� ���� ����
	preRender();

	//if (texture)
	//	device->SetTexture(0, texture);

	//D3DMATERIAL9 mtrl;
	//// Set the RGBA for diffuse reflection.
	//mtrl.Diffuse.r = 1.0f;
	//mtrl.Diffuse.g = 0.0f;
	//mtrl.Diffuse.b = 0.0f;
	//mtrl.Diffuse.a = 1.0f;
	//// Set the RGBA for ambient reflection.
	//mtrl.Ambient.r = 1.0f;
	//mtrl.Ambient.g = 0.0f;
	//mtrl.Ambient.b = 0.0f;
	//mtrl.Ambient.a = 1.0f;
	//// Set the color and sharpness of specular highlights.
	//mtrl.Specular.r = 1.0f;
	//mtrl.Specular.g = 1.0f;
	//mtrl.Specular.b = 1.0f;
	//mtrl.Specular.a = 1.0f;
	//mtrl.Power = 500.0f;
	//// Set the RGBA for emissive color.
	//mtrl.Emissive.r = 0.0f;
	//mtrl.Emissive.g = 0.0f;
	//mtrl.Emissive.b = 0.0f;
	//mtrl.Emissive.a = 0.0f;
	//device->SetMaterial(&mtrl);


	//device->SetFVF(ParticleSystem::Particle::FVF);
	device->SetStreamSource(0, vb, 0, sizeof(ParticleSystem::Particle));


	// ���� ī�޶� ���� �ֽ�ȭ
	device->GetTransform(D3DTS_VIEW, &viewMatrix);
	device->GetTransform(D3DTS_PROJECTION, &projectionMatrix);
	// viewProjection �ʱ�ȭ
	ShaderContainer::SetFireExplosionShaderParameters(texture,viewMatrix * projectionMatrix);

	// ���̴��� �������ش�.
	ShaderContainer::SetFireExplosionShader();
	//constTable->SetMatrix(
	//	device,
	//	viewProjectionMatrixHandle,
	//	&D3DXMATRIX(viewMatrix * projectionMatrix));


	device->DrawPrimitive(
		D3DPT_POINTLIST,
		0,
		currentNumOfParticles);

	postRender();
}

void FireExplosion::preRender()
{
	ParticleSystem::preRender();

	//device->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE);
	// �ٸ� ���� �μ��� Z���۾��⸦ �������� �������̵��Ͽ���.
	//device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCCOLOR);
    //device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	// read, but don't write particles to z-buffer
	device->SetRenderState(D3DRS_ZWRITEENABLE, false);
	//device->SetVertexShader(fireExplosionShader);
	//device->SetVertexDeclaration(declaration);
}

void FireExplosion::postRender()
{
	ParticleSystem::postRender();

	//device->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	device->SetRenderState(D3DRS_ZWRITEENABLE, true);
	device->SetVertexShader(NULL);
	device->SetVertexDeclaration(NULL);
}

GateEffect::GateEffect(IDirect3DDevice9 * device, const char * textureFileName, int numOfParticles, int particleSize, const D3DXVECTOR3 & origin)
	: ParticleSystem(device, /*viewMatrix,*/ textureFileName, numOfParticles, particleSize, origin), preExplosionFactor(0.1f)
{
}

void GateEffect::resetParticle(ParticleAttribute & attribute)
{
}

void GateEffect::update(float deltaTime)
{
}

void GateEffect::render()
{

}

void GateEffect::preRender()
{
	ParticleSystem::preRender();

	device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
	device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

	device->SetRenderState(D3DRS_ZWRITEENABLE, false);
}

void GateEffect::postRender()
{
	ParticleSystem::postRender();

	device->SetRenderState(D3DRS_ZWRITEENABLE, true);
	device->SetVertexShader(NULL);
	device->SetVertexDeclaration(NULL);
}
