#ifndef PARTICLESYSTEM_H
#define PARTICLESYSTEM_H

#include <vector>
#include "Utility.h"

using namespace std;

class ParticleSystem
{
public:
	struct Particle
	{
		D3DXVECTOR3 position;
		D3DCOLOR    color;
		static const DWORD FVF;
	};

	struct ParticleAttribute
	{
		// millisec ´ÜÀ§
		ParticleAttribute()
		{
			lifeTime = 0.0f;
			age = 0.0f;
			isAlive = false;
		}

		D3DXVECTOR3 position;
		D3DXVECTOR3 velocity;
		D3DXVECTOR3 acceleration;
		float       lifeTime;     
		float       age;          
		D3DXCOLOR   color;        
		D3DXCOLOR   colorFade;    
		bool        isAlive;
	};

	ParticleSystem(IDirect3DDevice9 * device, const char * textureFileName, int numOfParticles, int particleSize, const D3DXVECTOR3 & origin, float duration = 1.5f);
	virtual ~ParticleSystem();

	bool init(const char * texFileName);
	void addParticle();
	void removeParticle(int index);
	static float getRandomFloat(float value1, float value2)
	{
		if (value1 >= value2)
		{
			float temp = value1;
			value1 = value2;
			value2 = temp;
		}

		float factor = (rand() % 10000) * 0.0001f;

		return (factor * (value2 - value1)) + value1;
	}


	bool isEmpty() { return currentNumOfParticles == 0; }

	void setAmountFactor(float amountFactor)
	{
		if (amountFactor > 1.0f)
			amountFactor = 1.0f;
		if (amountFactor < 0)
			amountFactor = 0.01f;

		currentMaxNumOfParticles = maxNumOfParticles * amountFactor;

	}
	void setDuration(float duration)
	{
		if (duration <= 0)
			duration = 0.5f;
		this->duration = duration;
	}
	float getDuration() const { return duration; }

	void resetAllParticles();

	virtual void resetParticle(ParticleAttribute & attribute) = 0;
	virtual void update(float deltaTime) = 0;
	virtual void render() = 0;

	virtual void preRender();
	virtual void postRender();

protected:
	IDirect3DDevice9*       device;
	D3DXMATRIX projectionMatrix;
	D3DXMATRIX viewMatrix;

	D3DXVECTOR3             origin;
	IDirect3DTexture9*      texture;
	IDirect3DVertexBuffer9* vb;
	vector<ParticleAttribute> particlesAttribute;

	float duration;
	float particleSize;       // size of particles

	int maxNumOfParticles;
	float amountFactor;
	int currentMaxNumOfParticles;
	int currentNumOfParticles;
};

class FireExplosion : public ParticleSystem
{
private :
	IDirect3DVertexDeclaration9* declaration;
	IDirect3DVertexShader9* fireExplosionShader;
	ID3DXConstantTable* constTable;
	D3DXHANDLE viewProjectionMatrixHandle;

public:
	FireExplosion(IDirect3DDevice9 * device, const char * textureFileName, int numOfParticles, int particleSize, const D3DXVECTOR3 & origin);
	bool init();
	virtual ~FireExplosion()
	{
		if (declaration)
			declaration->Release();

		if (constTable)
			constTable->Release();

		if (fireExplosionShader)
			fireExplosionShader->Release();
	}
	virtual void resetParticle(ParticleAttribute & attribute);
	virtual void update(float deltaTime);
	virtual void render();

	virtual void preRender();
	virtual void postRender();

};

#endif 