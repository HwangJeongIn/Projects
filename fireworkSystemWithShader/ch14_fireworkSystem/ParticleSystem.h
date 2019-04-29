#ifndef PARTICLESYSTEM_H
#define PARTICLESYSTEM_H

#include "d3dUtility.h"
#include "camera.h"
#include <vector>

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
		// millisec 단위
		ParticleAttribute()
		{
			lifeTime = 0.0f;
			age = 0.0f;
			isAlive = false;
		}

		D3DXVECTOR3 position;
		D3DXVECTOR3 velocity;
		D3DXVECTOR3 acceleration;
		float       lifeTime;     // how long the particle lives for before dying  
		float       age;          // current age of the particle  
		D3DXCOLOR   color;        // current color of the particle   
		D3DXCOLOR   colorFade;    // how the color fades with respect to time
		bool        isAlive;
	};

	//ParticleSystem(IDirect3DDevice9 * device, const D3DXMATRIX & viewMatrix, const char * textureFileName, int numOfParticles, int particleSize, const D3DXVECTOR3 & origin);
	ParticleSystem(IDirect3DDevice9 * device, const char * textureFileName, int numOfParticles, int particleSize, const D3DXVECTOR3 & origin);
	virtual ~ParticleSystem();

	bool init(const char * texFileName);
	void addParticle();
	void removeParticle(int index);



	bool isEmpty() { return currentNumOfParticles == 0; }
	void setAmountFactor(float amountFactor)
	{
		if (amountFactor > 1.0f)
			amountFactor = 1.0f;
		if (amountFactor < 0)
			amountFactor = 0.01f;

		currentMaxNumOfParticles = maxNumOfParticles * amountFactor;

	}

	void resetAllParticles();

	virtual void resetParticle(ParticleAttribute & attribute) = 0;
	virtual void update(float deltaTime) = 0;
	virtual void render() = 0;

	virtual void preRender();
	virtual void postRender();

protected:
	IDirect3DDevice9*       device;

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	D3DXMATRIX projectionMatrix;
	D3DXMATRIX viewMatrix;
	//------------------------------------------

	D3DXVECTOR3             origin;
	d3d::BoundingBox        boundingBox;
	IDirect3DTexture9*      texture;
	IDirect3DVertexBuffer9* vb;
	vector<ParticleAttribute>    particlesAttribute;

	float particleSize;       // size of particles

	int maxNumOfParticles;
	float amountFactor;
	int currentMaxNumOfParticles;
	int currentNumOfParticles;
};

class FireExPlosion : public ParticleSystem
{
private :
	IDirect3DVertexDeclaration9* declaration;
	IDirect3DVertexShader9* fireExplosionShader;
	ID3DXConstantTable* constTable;
	D3DXHANDLE viewProjectionMatrixHandle;

public:
	FireExPlosion(IDirect3DDevice9 * device, const char * textureFileName, int numOfParticles, int particleSize, const D3DXVECTOR3 & origin);
	bool init();


	// update와 render는 무조건 오버라이딩
	virtual void resetParticle(ParticleAttribute & attribute);
	virtual void update(float deltaTime);
	virtual void render();

	virtual void preRender();
	virtual void postRender();
};

#endif // __pSystemH__