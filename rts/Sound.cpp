// Sound.cpp: implementation of the CSound class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "Sound.h"

#include <math.h>
#include "InfoConsole.h"
#include "RegHandler.h"
#include "Camera.h"
#include "WorldObject.h"
#include "InfoConsole.h"
#include "FileHandler.h"
//#include "mmgr.h"

CSound* sound;

CSound::CSound()
{
	noSound = false;
	alutInit(NULL,0);
	alGetError();
}

CSound::~CSound()
{
	LoadedFiles.clear();
	for (std::vector<ALuint>::iterator it = Buffers.begin(); it != Buffers.end(); ++it)
		alDeleteBuffers(1,&(*it));
	Buffers.clear();
	if (noSound)
		return;
	alutExit();
}

void CSound::PlaySound(int id, float volume)
{
	if (noSound || !camera)
		return;
	PlaySound(id,camera->pos,volume);
}

void CSound::PlaySound(int id,CWorldObject* p,float volume)
{
	if (noSound)
		return;
	PlaySound(id,p->pos,volume);
}

void CSound::PlaySound(int id,const float3& p,float volume)
{
	if (noSound)
		return;
	ALuint source;
	ALfloat SourcePos[] = {p.x,p.y,p.z};
	ALfloat SourceVel[] = {0.0,0.0,0.0};
	alGenSources(1,&source);
	alSourcei(source, AL_BUFFER, id);
	alSourcef(source, AL_PITCH, 1.0 );
	alSourcef(source, AL_GAIN, volume );
	alSourcefv(source, AL_POSITION, SourcePos);
	alSourcefv(source, AL_VELOCITY, SourceVel);
	alSourcei(source, AL_LOOPING, false);
	alSourcePlay(source);
	alDeleteSources(1,&source);
}

void CSound::Update()
{
	if (noSound)
		return;
	UpdateListener();
}

void CSound::UpdateListener()
{
	if (noSound || !camera)
		return;
	ALfloat ListenerPos[] = {camera->pos.x,camera->pos.y,camera->pos.z};
	ALfloat ListenerVel[] = {0.0,0.0,0.0};
	ALfloat ListenerOri[] = {camera->pos.x+camera->forward.x,camera->pos.y+camera->forward.y,camera->pos.z+camera->forward.z,camera->up.x,camera->up.y,camera->up.z};
	alListenerfv(AL_POSITION,ListenerPos);
	alListenerfv(AL_VELOCITY,ListenerVel);
	alListenerfv(AL_ORIENTATION,ListenerOri);
}

ALuint CSound::LoadALBuffer(string path)
{
	if (noSound)
		return 0;
	Uint8 *buf;
	ALuint buffer;
	alGenBuffers(1,&buffer);
	CFileHandler file("Sounds/"+path);
	if(file.FileExists()){
		buf = new Uint8[file.FileSize()];
		file.Read(buf, file.FileSize());
	} else {
		MessageBox(0,"Couldnt open wav file",path,0);
		return 0;
	}
	alBufferData(buffer,AL_FORMAT_STEREO16,buf,file.FileSize(),22050);
	delete[] buf;
	return buffer;
}

ALuint CSound::GetWaveId(string path)
{
	if (noSound)
		return 0;
	int count = 0;
	ALuint buffer;
	for (vector<string>::iterator it = LoadedFiles.begin(); it != LoadedFiles.end(); ++it,count++) {
		if (*it == path)
			return Buffers[count];
	}
	buffer = LoadALBuffer(path);
	Buffers.push_back(buffer);
	LoadedFiles.push_back(path);
	return buffer;
}
