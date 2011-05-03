/*
 *  SoundKit.h
 *  fps_demo
 *
 *  Created by Charles J. Cliffe on 24/02/09.
 *  Copyright 2009 Cubic Productions. All rights reserved.
 *
 */

#pragma once

#include <OpenAL/al.h>
#include <OpenAL/alc.h>

#include <iostream>
#include <fstream>
using namespace std;


#define NUM_BUFFERS 3
#define BUFFER_SIZE 4096

struct WaveFmtSubChunk
{
	char         id [4];
	int          size          : 32;
	int          format        : 16;
	unsigned int channels      : 16;
	unsigned int sampleRate    : 32;
	unsigned int byteRate      : 32;
	unsigned int blockAlign    : 16;
	unsigned int bitsPerSample : 16;
};


// The wave data chunk contains the actual sound data.
struct WaveDataSubChunk
{
	char id [4];
	int  size : 32;
	char data [];
};
	
class SoundSample
{
private:
	ALuint sndBuffer;
	ALuint sndSource;
	bool bufferOwner;
	
public:
	SoundSample(): sndBuffer(0), sndSource(0), bufferOwner(false)
	{
	}

	SoundSample(const SoundSample& sample_in): sndBuffer(0), sndSource(0), bufferOwner(false)
	{
		sndBuffer = sample_in.sndBuffer;
		postLoad();
	}
	
	
	~SoundSample()
	{
		if (sndSource)
		{
			alDeleteSources(1, &sndSource);
		}
		
		if (sndBuffer)
		{
			alDeleteBuffers(1, &sndBuffer);		
		}
	}
	
	
	void bind(const SoundSample &sample_in)
	{
		sndBuffer = sample_in.sndBuffer;
		alSourcei (sndSource, AL_BUFFER, sndBuffer);
	}
	
	void postLoad()
	{
		alGenSources(1, &sndSource);

		alSourcei (sndSource, AL_BUFFER, sndBuffer);
		setGain(1.0);
		setPitch(1.0);
		setLoop(false);

		//	alSourcefv(sndSource, AL_POSITION, SourcePos);
		//	alSourcefv(sndSource, AL_VELOCITY, SourceVel);
	}
	
	
	void setGain(float gainVal)
	{
		alSourcef(sndSource, AL_GAIN, gainVal);
	}
	
	void setPitch(float pitchVal)
	{
		alSourcef(sndSource, AL_PITCH, pitchVal);
	}
	
	void setLoop(bool loopVal)
	{
		alSourcei (sndSource, AL_LOOPING, loopVal);
	}
	
	void play()
	{
		alSourcePlay(sndSource);	
	}
	
	bool isPlaying()
	{
		ALint state;
		alGetSourcei(sndSource, AL_SOURCE_STATE, &state);
		
		return (state == AL_PLAYING);
	}
	
	void stop()
	{
		alSourceStop(sndSource);
	}

	void rewind()
	{
		alSourceRewind(sndSource);
	}
	
	
	void pause()
	{
		alSourceRewind(sndSource);
	}
	
	
	bool loadWav(char *wavFn)
	{
		int mFileOffset = 0;
		
		ifstream waveFile (wavFn);
		
		// Get the size of the file
		waveFile.seekg (0, ios::end); // Move the get pointer to the end of the file
		int fileSize = waveFile.tellg (); // Get the offset of the get pointer
		
		// Move the get pointer back to the beginning of the file
		waveFile.seekg (mFileOffset);
		
		// Allocate memory for the data
		unsigned char *fileData = new unsigned char [fileSize];
		
		// Read the data into memory
		waveFile.read ((char *)fileData, fileSize);
		
		
		// Check that it is a RIFF file
		if (strncmp ((char *)fileData, "RIFF", 4))
		{
			//raiseError ("Load Audio File", String ("Not a RIFF File : ") + pPathName);
			delete [] fileData;
			return false;
		}
		
		// Check that it is a WAVE format of the RIFF file.
		if (strncmp ((char *)(fileData + 8), "WAVE", 4))
		{
			//raiseError ("Load Audio File", String ("Not a WAVE File : ") + pPathName);
			delete [] fileData;
			return false;
		}
		
		
		// The wave format chunk contains information about the format
		//   of the sound information in the file.
		
		
		WaveFmtSubChunk *fmtChunk = (WaveFmtSubChunk *)(fileData + 12);
		
		if (strncmp (fmtChunk->id, "fmt ", 4))
		{
			//raiseError ("Load Audio File", String ("Format sub chunk not found in file : ") + pPathName);
			delete [] fileData;
			return false;
		}
		
		if (fmtChunk->format != 1)
		{
			//raiseError ("Load Audio File", String ("Unsupported compression found in file : ") + pPathName);
			delete [] fileData;
			return false;
		}
		
		WaveDataSubChunk *dataChunk = (WaveDataSubChunk *)(fileData + fmtChunk->size + 20); // 20 = (12 to fmtChunk) + (8 for fmt id and size members)
		
		for (int i=0;i<10;i++)
			if (!strncmp (dataChunk->id, "data", 4))
				break;
			else
				dataChunk = (WaveDataSubChunk *)(((char *)dataChunk) + dataChunk->size + 8);
		
		if (strncmp (dataChunk->id, "data", 4))
		{
			//raiseError ("Load Audio File", String ("Data sub chunk not found in file : ") + pPathName);
			delete [] fileData;
			return false;
		}
		
		
		ALenum format; // From <al.h>
		
		alGenBuffers (1, &sndBuffer);
		
		if (fmtChunk->channels == 1)
		{
			if (fmtChunk->bitsPerSample == 8)
				format = AL_FORMAT_MONO8; // 8 bit single channel
			else if (fmtChunk->bitsPerSample == 16)
				format = AL_FORMAT_MONO16; // 16 bit single channel
			else
			{
				//raiseError ("Load Audio File", String ("Unsupported bits per pixel in file : ") + pPathName);
				delete [] fileData;
				return false;
			}
		}
		else if (fmtChunk->channels == 2)
		{
			if (fmtChunk->bitsPerSample == 8)
				format = AL_FORMAT_STEREO8; // 8 bit dual channel
			else if (fmtChunk->bitsPerSample == 16)
				format = AL_FORMAT_STEREO16; // 16 bit dual channel
			else
			{
				//raiseError ("Load Audio File", String ("Unsupported bits per pixel in file : ") + pPathName);
				delete [] fileData;
				return false;
			}
		}
		else
		{
			//raiseError ("Load Audio File", String ("Unsupported number of channels in file : ") + pPathName);
			delete [] fileData;
			return false;
		}
		
		// Load the data into an OpenAL buffer.
		alBufferData (sndBuffer, format, dataChunk->data, dataChunk->size, fmtChunk->sampleRate);
		
		// Delete the memory for the file data
		delete [] fileData;
		
		printf("Wav [%s] loaded OK.\n",wavFn);
		
		postLoad();
		
		
		bufferOwner = true;
		
		// Check if there was an OpenAL error.
		return alGetError () == AL_NO_ERROR;
		
	}
	
	
	
};

class SoundKit
{
private:	
	static ALuint source, buffers[NUM_BUFFERS];	
	static ALCdevice *audio_dev;
	static ALCcontext *audio_ctx;
	
public:
	static bool init()
	{
		alGetError();
		const ALchar *pDeviceList = alcGetString(NULL, ALC_DEFAULT_DEVICE_SPECIFIER);
		if (pDeviceList) {
			printf("Available Audio Devices are:\n");
			while (*pDeviceList) {
				printf(" - %s\n", pDeviceList);
				pDeviceList += strlen(pDeviceList) + 1;
			}
		}
		
		audio_dev = alcOpenDevice(NULL);
		
		if (alGetError() != AL_NO_ERROR)
		{
			return 0;
		}
		
		
		audio_ctx = alcCreateContext(audio_dev, NULL);
		
		alcMakeContextCurrent(audio_ctx);
		
		if (alGetError() != AL_NO_ERROR)
		{
			return 0;
		}
		
		alGenBuffers(NUM_BUFFERS, buffers);
		//	alGenSources(1, &source);
		//		
		
		if(alGetError() != AL_NO_ERROR)
		{
			return 0;
		}	
		
		return 1;		
	}
	
	
	static void shutdown()
	{
		alcMakeContextCurrent(NULL);
		alcDestroyContext(audio_ctx);
		alcCloseDevice(audio_dev);
		alDeleteBuffers(NUM_BUFFERS, buffers);
	}
};

ALuint SoundKit::source, SoundKit::buffers[NUM_BUFFERS];	
ALCdevice *SoundKit::audio_dev;
ALCcontext *SoundKit::audio_ctx;



	
