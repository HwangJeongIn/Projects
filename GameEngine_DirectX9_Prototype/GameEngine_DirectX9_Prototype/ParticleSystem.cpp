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
		D3DPOOL_DEFAULT,//D3DPOOL_MANAGED,//D3DPOOL_DEFAULT, // D3DPOOL_MANAGED can't be used with D3DUSAGE_DYNAMIC 
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
	device->SetRenderState(D3DRS_LIGHTING, false);

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
	ParticleSystem::update(deltaTime);
	//currentDuration += deltaTime;

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

GateEffect::GateEffect(IDirect3DDevice9 * device, const D3DXVECTOR3 & origin)
// ����Ʈ �ֺ��� �����̴� ��ƼŬ������ 2.0 // 500��/2�� ��ƼŬ ���� // �ؽ�ó ������ 
	: ParticleSystem(device, "../Fbx/Textures/flare_alpha.dds", 1000, 10.0f, origin, 100000000), maxGateSize(300), minGateSize(200), changeRateOfSize(50.0f),
	gateParticleSpeed(2.0f), // ����Ʈ�ֺ��� �����̴� ��ƼŬ�� �ſ� ������ �����δ�.
	gateParticleLifeTime(9.0f)// , gateVb(nullptr)
{
	if (!device) return;
	currentGateSize = minGateSize;
	
	//init();

	//resetGate();
	resetAllParticles();
}

//bool GateEffect::init()
//{
//	//HRESULT hr = 0;
//	//// ����Ʈ�� �ϳ��� �����.
//
//	//hr = device->CreateVertexBuffer(
//	//	maxNumOfParticles * sizeof(Particle),
//	//	D3DUSAGE_DYNAMIC | D3DUSAGE_POINTS | D3DUSAGE_WRITEONLY,
//	//	Particle::FVF,
//	//	D3DPOOL_DEFAULT,//D3DPOOL_MANAGED,//D3DPOOL_DEFAULT, // D3DPOOL_MANAGED can't be used with D3DUSAGE_DYNAMIC 
//	//	&gateVb,
//	//	0);
//
//	//hr = device->CreateVertexBuffer(
//	//	1 * sizeof(Particle),
//	//	/*D3DUSAGE_DYNAMIC |*/ D3DUSAGE_POINTS | D3DUSAGE_WRITEONLY,
//	//	Particle::FVF,
//	//	D3DPOOL_MANAGED,//D3DPOOL_DEFAULT, // D3DPOOL_MANAGED can't be used with D3DUSAGE_DYNAMIC 
//	//	&gateVb,
//	//	0);
//
//	//if (FAILED(hr))
//	//{
//	//	::MessageBox(0, "CreateVertexBuffer() - FAILED", "PSystem", 0);
//	//	return false;
//	//}
//	return true;
//}

//void GateEffect::resetGate()
//{
//	gateAttribute.isAlive = true;
//	gateAttribute.velocity = D3DXVECTOR3(0, 0, 0);
//	gateAttribute.position = origin;
//	gateAttribute.lifeTime =0.0f;
//	gateAttribute.age = 0.0f;
//
//	gateAttribute.color = D3DXCOLOR(.5, .5, .5, 1);
//	gateAttribute.colorFade = D3DXCOLOR(1, 1, 1, 1);
//}

//void GateEffect::updateGate(float deltaTime)
//{
//	if (!device) return;
//
//	gateAttribute.position = origin;
//
//	currentGateSize += changeRateOfSize * deltaTime;
//
//	if (currentGateSize < minGateSize)
//	{
//		currentGateSize = minGateSize + 5;
//		changeRateOfSize *= -1;
//	}
//	else if (currentGateSize > maxGateSize)
//	{
//		currentGateSize = maxGateSize - 5;
//		changeRateOfSize *= -1;
//	}
//
//
//
//
//	//// ���̴����� ����� ���� ������Ʈ
//	//ParticleSystem::Particle* v = 0;
//	//gateVb->Lock(
//	//	0,
//	//	1 * sizeof(Particle),
//	//	(void**)&v,
//	//	0/*_vbOffset ? D3DLOCK_NOOVERWRITE : D3DLOCK_DISCARD*/);
//
//	//float gateFactor = currentGateSize / maxGateSize;
//
//	//v->position = gateAttribute.position;
//	//v->color = D3DXCOLOR(gateAttribute.color * (1 - gateFactor) + gateAttribute.colorFade * gateFactor);
//
//
//
//	//gateVb->Unlock();
//
//}

void GateEffect::resetParticle(ParticleAttribute & attribute)
{
	attribute.isAlive = true;

	attribute.position
		= D3DXVECTOR3
		(
			getRandomFloat(-maxGateSize/10, maxGateSize/10),
			getRandomFloat(-maxGateSize/10, maxGateSize/10),
			getRandomFloat(-maxGateSize/10, maxGateSize/10)
		);

	attribute.position += origin;

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

	attribute.velocity *= gateParticleSpeed;
	attribute.lifeTime = getRandomFloat(gateParticleLifeTime/2.0f, gateParticleLifeTime);
	attribute.age = 0.0f;

	float temp = getRandomFloat(0.5f, 0.80f);
	attribute.color = D3DXCOLOR(
		getRandomFloat(9.0f, 1.00f),
		getRandomFloat(5.0f, 1.00f),
		getRandomFloat(0.0f, 1.00f),
		1.0f);

	temp = getRandomFloat(0.0f, .10f);
	attribute.colorFade = D3DXCOLOR(
		temp,//getRandomFloat(0.0f, .20f),
		temp,//getRandomFloat(0.0f, .20f),
		temp,//getRandomFloat(0.0f, .20f),
		0.3f);



}

void GateEffect::update(float deltaTime)
{
	if (!device) return;
	// ���� ����

	ParticleSystem::update(deltaTime);
	//currentDuration += deltaTime;
	//updateGate(deltaTime);

	while (currentNumOfParticles < currentMaxNumOfParticles)
	{
		addParticle();
	}

	for (int i = 0; i < currentNumOfParticles; ++i)
	{
		// ����ִ� ��ƼŬ�� ���ؼ�
		particlesAttribute[i].position += particlesAttribute[i].velocity * deltaTime;
		particlesAttribute[i].age += deltaTime;

		if (particlesAttribute[i].age > particlesAttribute[i].lifeTime)
		{
			particlesAttribute[i].isAlive = false;

			removeParticle(i);
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

void GateEffect::render()
{

	if (!device) return;

	// ���̴� ���� ����
	preRender();

	//if (texture)
	//	device->SetTexture(0, texture);

	//device->SetFVF(ParticleSystem::Particle::FVF);
	device->SetStreamSource(0, vb, 0, sizeof(ParticleSystem::Particle));


	// ���� ī�޶� ���� �ֽ�ȭ
	device->GetTransform(D3DTS_VIEW, &viewMatrix);
	device->GetTransform(D3DTS_PROJECTION, &projectionMatrix);
	// viewProjection �ʱ�ȭ
	ShaderContainer::SetFireExplosionShaderParameters(texture, viewMatrix * projectionMatrix);

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
/*
{
	// ���̴� ���� ����
	preRender();

	if (texture)
		device->SetTexture(0, texture);

	// ����Ʈ���� �׸���
	//device->SetRenderState(D3DRS_POINTSIZE, *((DWORD*)&currentGateSize));
	//device->SetStreamSource(0, gateVb, 0, sizeof(ParticleSystem::Particle));

	//// ���� ī�޶� ���� �ֽ�ȭ
	//device->GetTransform(D3DTS_VIEW, &viewMatrix);
	//device->GetTransform(D3DTS_PROJECTION, &projectionMatrix);


	//// viewProjection �ʱ�ȭ
	//ShaderContainer::SetFireExplosionShaderParameters(texture, viewMatrix * projectionMatrix);

	//// ���̴��� �������ش�.
	//ShaderContainer::SetFireExplosionShader();


	//device->DrawPrimitive(
	//	D3DPT_POINTLIST,
	//	0,
	//	1);


	//device->SetFVF(ParticleSystem::Particle::FVF);
	// �ֺ� ��ƼŬ�� �׸���.
	//device->SetRenderState(D3DRS_POINTSIZE, *((DWORD*)&particleSize));
	device->SetStreamSource(0, vb, 0, sizeof(ParticleSystem::Particle));

	// viewProjection �ʱ�ȭ
	ShaderContainer::SetFireExplosionShaderParameters(texture, viewMatrix * projectionMatrix);

	// ���̴��� �������ش�.
	ShaderContainer::SetFireExplosionShader();

	device->DrawPrimitive(
		D3DPT_POINTLIST,
		0,
		currentNumOfParticles);

	postRender();
}
*/
void GateEffect::preRender()
{
	ParticleSystem::preRender();

	device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
	device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

	device->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE);
	device->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);

	device->SetRenderState(D3DRS_ZWRITEENABLE, false);
}

void GateEffect::postRender()
{
	ParticleSystem::postRender();


	device->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	device->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
	device->SetRenderState(D3DRS_ZWRITEENABLE, true);
	device->SetVertexShader(NULL);
	device->SetVertexDeclaration(NULL);
}
