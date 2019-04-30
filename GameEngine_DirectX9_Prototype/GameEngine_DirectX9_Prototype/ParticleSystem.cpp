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



const DWORD ParticleSystem::Particle::FVF = D3DFVF_XYZ | D3DFVF_DIFFUSE;


ParticleSystem::ParticleSystem(IDirect3DDevice9 * device, const char * textureFileName, int numOfParticles, int particleSize, const D3DXVECTOR3 & origin, float duration)
	: device(device), vb(nullptr), texture(nullptr), duration(duration),/*declaration(nullptr),*/
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

	// 현재인덱스 파티클을 가장마지막 활성화 파티클과 스왑해준다.
	ParticleSystem::ParticleAttribute attribute = particlesAttribute[index];
	particlesAttribute[index] = particlesAttribute[--currentNumOfParticles];
	particlesAttribute[currentNumOfParticles] = attribute;

}


void ParticleSystem::preRender()
{
	device->SetRenderState(D3DRS_LIGHTING, false);

	// true인 경우 전체 텍스처를 포인트 스프라이트의 텍스처 매핑에 이용
	device->SetRenderState(D3DRS_POINTSPRITEENABLE, true);

	// true인 경우 포인트 크기를 뷰 스페이스 단위로 해석하도록 지정
	// 포인트 스프라이트의 크기는 카메라와의 거리에 따라 적절하게 조절된다.
	device->SetRenderState(D3DRS_POINTSCALEENABLE, true);

	float temp = 5.0f;
	// 포인트 스프라이트의 크기 
	// D3DRS_POINTSCALEENABLE에 따라서 뷰스페이스 내의 크기나 스크린 스페이스 내의 크기로 해석
	device->SetRenderState(D3DRS_POINTSIZE, *((DWORD*)&temp));
	temp = 0.0f;
	// 포인트 스프라이트의 지정할 수 있는 최소크기를 지정
	device->SetRenderState(D3DRS_POINTSIZE_MIN, *((DWORD*)&temp));

	// 세가지 값에 따라서 최종 크기를 지정한다.
	device->SetRenderState(D3DRS_POINTSCALE_A, *((DWORD*)&temp));
	device->SetRenderState(D3DRS_POINTSCALE_B, *((DWORD*)&temp));
	temp = 1.0f;
	device->SetRenderState(D3DRS_POINTSCALE_C, *((DWORD*)&temp));

	// 텍스처의 알파를 이용
	device->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	device->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);

	// 알파블렌딩 사용
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
	: ParticleSystem(device, /*viewMatrix,*/ textureFileName, numOfParticles, particleSize, origin),
	declaration(nullptr), fireExplosionShader(nullptr), constTable(nullptr), viewProjectionMatrixHandle(0)
{
	if (!device) return;
	resetAllParticles();
	init();
}

bool FireExplosion::init()
{
	HRESULT hr = 0;

	D3DVERTEXELEMENT9 decl[] =
	{
		/*
		나중에 셰이더 코드의 구조체
		struct VS_INPUT
		{
		vector positi : POSITION // POSITION0
		: NORMAL0
		: NORMAL1
		: NORMAL2
		}
		과 매핑된다.

		*/
		// offsets in bytes
		{ 0,  0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
		{ 0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR,   0 },
		//{ 0, 24, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL,   1 },
		//{ 0, 36, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL,   2 },
		D3DDECL_END()
	};


	hr = device->CreateVertexDeclaration(decl, &declaration);
	if (FAILED(hr))
	{
		::MessageBox(0, "CreateVertexDeclaration() - FAILED", 0, 0);
		return false;
	}
	

	// 쉐이더 컴파일 / 핸들 받아오기
	//------------------------------------------------------------------
	ID3DXBuffer* compiledCode = 0;
	ID3DXBuffer* errorBuffer = 0;

	hr = D3DXCompileShaderFromFile(
		"FireExplosion.vs",
		0,
		0,
		"Main", // entry point function name
		"vs_1_1",

		D3DXSHADER_ENABLE_BACKWARDS_COMPATIBILITY | D3DXSHADER_DEBUG,
		&compiledCode,
		&errorBuffer,
		&constTable);

	// output any error messages
	if (errorBuffer)
	{
		::MessageBox(0, (char*)errorBuffer->GetBufferPointer(), 0, 0);
		errorBuffer->Release();
	}

	if (FAILED(hr))
	{
		::MessageBox(0, "D3DXCompileShaderFromFile() - FAILED", 0, 0);
		return false;
	}

	hr = device->CreateVertexShader(
		(DWORD*)compiledCode->GetBufferPointer(),
		&fireExplosionShader);

	if (FAILED(hr))
	{
		::MessageBox(0, "CreateVertexShader - FAILED", 0, 0);
		return false;
	}

	if (compiledCode)
		compiledCode->Release();

	viewProjectionMatrixHandle = constTable->GetConstantByName(0, "ViewProjectionMatrix");

	return true;
}

void FireExplosion::resetParticle(ParticleAttribute & attribute)
{

	attribute.isAlive  = true;
	attribute.position = origin;

	//D3DXVECTOR3 min = D3DXVECTOR3(-1.0f, -1.0f, -1.0f);
	//D3DXVECTOR3 max = D3DXVECTOR3( 1.0f,  1.0f,  1.0f);

	// 여러가지 방향이 나온다
	attribute.velocity
		= D3DXVECTOR3
		(
			getRandomFloat(-1, 1),
			getRandomFloat(-1, 1),
			getRandomFloat(-1, 1)
		);


	// normalize to make spherical
	// 정규화한다 모두 일정한 속력으로 움직여야 해서
	D3DXVec3Normalize(
		&attribute.velocity,
		&attribute.velocity);

	attribute.velocity *= getRandomFloat(1, 20);

	attribute.color = D3DXCOLOR(
		getRandomFloat(0.8f, 1.0f),
		getRandomFloat(0.0f, .3f),
		getRandomFloat(0.0f, .3f),
		1.0f);

	attribute.colorFade = D3DXCOLOR(
		getRandomFloat(0.0f, .30f),
		getRandomFloat(0.0f, .30f),
		getRandomFloat(0.8f, 1.0f),
		.0f);

	// 수명 2초동안 유지하도록 설정
	attribute.age      = 0.0f;
	attribute.lifeTime = getRandomFloat(0.1f, 1.5f);// lives for 2 seconds
}

void FireExplosion::update(float deltaTime)
{
	if (!device) return;
	// 추후 수정

	while (currentNumOfParticles < currentMaxNumOfParticles)
	{
		addParticle();
	}

	for (int i = 0; i < currentNumOfParticles; ++i)
	{
		// 살아있는 파티클에 대해서
		// 속도 최신화
		particlesAttribute[i].position += particlesAttribute[i].velocity * deltaTime;//(deltaTime /1000.0f);

		// 시간 최신화
		particlesAttribute[i].age += deltaTime;

		if (particlesAttribute[i].age > particlesAttribute[i].lifeTime)
		{
			particlesAttribute[i].isAlive = false;

			removeParticle(i);
			//addParticle();
		}

	}

	// 쉐이더에서 사용할 버퍼 업데이트
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

		// 보간해서 넣어준다.
		lifeFactor = particlesAttribute[i].age / particlesAttribute[i].lifeTime;
		v[i].color = particlesAttribute[i].color * (1 - lifeFactor) + particlesAttribute[i].colorFade * lifeFactor;
	}

	vb->Unlock();
}

void FireExplosion::render()
{
	if (!device) return;

	// 쉐이더 지정 포함
	preRender();

	if (texture)
		device->SetTexture(0, texture);

	device->SetFVF(ParticleSystem::Particle::FVF);
	device->SetStreamSource(0, vb, 0, sizeof(ParticleSystem::Particle));


	// 먼저 카메라 부터 최신화
	this->device->GetTransform(D3DTS_VIEW, &viewMatrix);
	// viewProjection 초기화
	constTable->SetMatrix(
		device,
		viewProjectionMatrixHandle,
		&D3DXMATRIX(viewMatrix * projectionMatrix));

	device->DrawPrimitive(
		D3DPT_POINTLIST,
		0,
		currentNumOfParticles);

	postRender();
}

void FireExplosion::preRender()
{
	ParticleSystem::preRender();

	// 다른 블렌드 인수와 Z버퍼쓰기를 막기위해 오버라이드하였다.
	device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
    device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

	// read, but don't write particles to z-buffer
	//device->SetRenderState(D3DRS_ZWRITEENABLE, false);
	device->SetVertexShader(fireExplosionShader);
	device->SetVertexDeclaration(declaration);
}

void FireExplosion::postRender()
{
	ParticleSystem::postRender();

	//device->SetRenderState(D3DRS_ZWRITEENABLE, true);
	device->SetVertexShader(NULL);
	device->SetVertexDeclaration(NULL);
}
