// chickpea unframework
// sound loader and mixer
// version 0.01
//
// This software is in the public domain. Where that dedication is not
// recognized, you are granted a perpetual, irrevocable license to copy
// and modify it however you want.
//
// People who worked on this file:
//	Ivan Safrin

#ifndef mixer_h
#define mixer_h

#define READ_BUFFER_SIZE 32768
#define MIXER_FRAMES_PER_BUFFER 2048
#define MIXER_AUDIO_FREQ 44100
#define MIXER_CIRCULAR_BUFFER_SIZE 32
#define MIXER_NUM_CHANNELS 2
#define MIXER_MIX_BUFFER_SIZE (POLY_FRAMES_PER_BUFFER*POLY_CIRCULAR_BUFFER_SIZE)


typedef enum {SoundFormatUnsupported, SoundFormat8, SoundFormat16, SoundFormat32} SoundFormat;

typedef struct {
	int16_t *soundBuffer;
	uint32_t numSamples;
	float frequencyAdjust;
	unsigned int numChannels;
	unsigned int playbackOffset;
	unsigned int frequency;
	int playing;
	int looped;
	float volume;
	float pitch;
	void *next;
	uint32_t id;
} MixerSound;

typedef struct {
	MixerSound *sounds;
	float volume;
	uint32_t nextID;
} Mixer;

void mixerInit(Mixer *mixer);
uint32_t mixerLoadSoundFromData(Mixer *mixer, const char *data, int size, int channels, unsigned int freq, SoundFormat format);
uint32_t mixerLoadSoundWAV(Mixer *mixer, const char *fileName);
void mixerMixIntoBuffer(Mixer *mixer, int16_t *buffer, unsigned int numSamples);
void mixerPlaySound(Mixer *mixer, uint32_t soundID, char restart, char looped);

// IMPLEMENTATION


void mixerPlaySound(Mixer *mixer, uint32_t soundID, char restart, char looped) {
	MixerSound *sound = mixer->sounds;
	while(sound) {
		if(sound->id == soundID) {
			sound->playing = 1;
			if(restart) {
				sound->playbackOffset = 0;
			}
			sound->looped = looped;
			return;
		}
		sound = sound->next;
	}
}

void mixerInit(Mixer *mixer) {
	mixer->volume = 1.0f;
	mixer->sounds = 0;
	mixer->nextID = 1;
}

int soundCheck(int val, const char *msg, const char *fileName) {
	if(!val) {
		printf("ERROR: Cannot load %s\n%s\n", fileName, msg);
		return 1;
	}
	return 0;
}


unsigned long readByte32(const unsigned char data[4]) {
#if PLATFORM_BIG_ENDIAN
	return (data[0] << 24) + (data[1] << 16) + (data[2] << 8) + data[3];
#else
	return (data[3] << 24) + (data[2] << 16) + (data[1] << 8) + data[0];
#endif
}

unsigned short readByte16(const unsigned char data[2]) {
#if PLATFORM_BIG_ENDIAN
	return (data[0] << 8) + data[1];
#else
	return (data[1] << 8) + data[0];
#endif	
}

uint32_t mixerLoadSoundFromData(Mixer *mixer, const char *data, int size, int channels, unsigned int freq, SoundFormat format) {

	MixerSound *sound = (MixerSound*) malloc(sizeof(MixerSound));

	sound->next = mixer->sounds;
	mixer->sounds = sound;
	sound->id = mixer->nextID;
	sound->playing = 0;
	sound->volume = 1.0f;
	sound->pitch = 1.0f;

	sound->soundBuffer = (int16_t*) malloc(sizeof(int16_t) * channels * size);
	int16_t *soundBufferPtr = sound->soundBuffer;
	unsigned int dataOffset = 0;
	switch(format) {
		case SoundFormat8:
			sound->numSamples = size / channels;
			break;
		case SoundFormat16:
			sound->numSamples = size / channels / 2;
			break;
		case SoundFormat32:
			sound->numSamples = size / channels / 4;
			break;
		default:
		break;
	}
	
	for(int i=0; i < sound->numSamples; i++){
		for(int c=0; c < channels; c++) {
			switch(format) {
				case SoundFormat8:
					*soundBufferPtr = ((int8_t*)data)[dataOffset];
				break;
				case SoundFormat16:
					*soundBufferPtr = ((int16_t*)data)[dataOffset];
				break;
				case SoundFormat32:
					*soundBufferPtr = ((int32_t*)data)[dataOffset];
				break;
				default:
				break;
			}
			soundBufferPtr++;
			dataOffset++;
		}
	}
	
	sound->numChannels = channels;
	sound->frequency = freq;
	sound->frequencyAdjust = (float)freq/(float)MIXER_AUDIO_FREQ;

	mixer->nextID++;
	return sound->id;
}

uint32_t mixerLoadSoundWAV(Mixer *mixer, const char *fileName) {
	FILE *f = fopen(fileName, "rb");
	if(!f) {
		printf("Unable to open WAV file: %s\n", fileName);
	}
	
	char magic[5];
	magic[4] = '\0';
	unsigned char data32[4];
	unsigned char data16[2];
	
	int err = 0;
	err += soundCheck(fread(magic,4,1,f) == 1, "LoadWav: Cannot read wav file.", fileName);
	err += soundCheck(!strcmp(magic, "RIFF"), "LoadWav: Wrong wav file format. This file is not a .wav file (no RIFF magic).", fileName);
	fseek(f, 4,SEEK_CUR);
	err += soundCheck(fread(magic,4,1,f) == 1, "LoadWav: Cannot read wav file.", fileName);
	err += soundCheck(!strcmp(magic, "WAVE"), "LoadWav: Wrong wav file format. This file is not a .wav file (no WAVE format).", fileName);
	err += soundCheck(fread(magic,4,1,f) == 1, "LoadWav: Cannot read wav file.", fileName);
	err += soundCheck(!strcmp(magic,"fmt "), "LoadWav: Wrong wav file format. This file is not a .wav file (no 'fmt ' subchunk).", fileName);
	err += soundCheck(fread(data32,4,1,f)   == 1, "LoadWav: Cannot read wav file.", fileName);
	unsigned long subChunk1Size = readByte32(data32);
	err += soundCheck(subChunk1Size >= 16, "Wrong wav file format. This file is not a .wav file ('fmt ' chunk too small, truncated file?).", fileName);
	err += soundCheck(fread(data16,2,1,f) == 1, "LoadWav: Cannot read wav file.", fileName);
	unsigned short audioFormat = readByte16(data16);
	err += soundCheck(audioFormat == 1, "LoadWav: Wrong wav file format. This file is not a .wav file (audio format is not PCM).", fileName);
	err += soundCheck(fread(data16,2,1,f) == 1, "LoadWav: Cannot read wav file ", fileName);;
	unsigned short channels = readByte16(data16);
	err += soundCheck(fread(data32,4,1,f) == 1, "LoadWav: Cannot read wav file ", fileName);
	unsigned long frequency = readByte32(data32);
	fseek(f, 6, SEEK_CUR);
	err += soundCheck(fread(data16,2,1,f) == 1, "LoadWav: Cannot read wav file ", fileName);
	unsigned short bps = readByte16(data16);
	
	SoundFormat format = SoundFormatUnsupported;
	
	switch(bps) {
		case 8:
			format = SoundFormat8;
		break;
		case 16:
			format = SoundFormat16;
		break;
		case 32:
			format = SoundFormat32;
		break;
			
	}
	
	err += soundCheck(fread(magic,4,1,f) == 1, "LoadWav: Cannot read wav file ", fileName);
	err += soundCheck(!strcmp(magic, "data"), "LoadWav: Wrong wav file format. This file is not a .wav file (no data subchunk): ", fileName);
	err += soundCheck(fread(data32,4,1,f) == 1, "LoadWav: Cannot read wav file", fileName);
	unsigned long subChunk2Size = readByte32(data32);
	
	char array[READ_BUFFER_SIZE];
	long bytes;
	int dataSize = 0;
	char *data = malloc(1);
	while (dataSize != subChunk2Size) {
		bytes = fread(array, 1, READ_BUFFER_SIZE, f);
		if (bytes <= 0) {
			break;
		}
		if (dataSize + bytes > subChunk2Size) {
			bytes = subChunk2Size - dataSize;
		}
		data = realloc(data, dataSize+bytes);
		memcpy(data+dataSize, array, bytes);
		dataSize += bytes;
	};
	fclose(f);

	if(err > 0) {
		printf("Errors loading WAV file.\n");
		free(data);
		return 0;
	} else if(format == SoundFormatUnsupported) {
		free(data);
		printf("Unsupported sound format\n");
		return 0;
	} else {
		uint32_t retID = mixerLoadSoundFromData(mixer, data, dataSize, channels, frequency, format);
		free(data);
		return retID;
	}

}

float mixSamples(float A, float B) {
	if (A < 0 && B < 0 ) {
		return  (A + B) - (A * B)/-1.0;
	} else if (A > 0 && B > 0 ) {
		return (A + B) - (A * B)/1.0;
	} else {
		return A + B;
	}
}

float getSampleAsFloat(MixerSound *sound, unsigned int offset, unsigned int channel) {
	float adjustedOffset = ((float)offset) * sound->pitch * sound->frequencyAdjust;
	float ret = (((float)(sound->soundBuffer[((((unsigned int )adjustedOffset)%sound->numSamples)*sound->numChannels)+(channel % sound->numChannels)])/((float)INT16_MAX))) * sound->volume;
	return ret;
}


void mixerMixIntoBuffer(Mixer *mixer, int16_t *buffer, unsigned int numSamples) {
	memset(buffer, 0, sizeof(int16_t) * numSamples * MIXER_NUM_CHANNELS);

	int16_t *bufferPtr = buffer;
	for(int i=0; i < numSamples; i++) {
		
		float mixResults[MIXER_NUM_CHANNELS];
		memset(mixResults, 0, sizeof(float) * MIXER_NUM_CHANNELS);
	
		int mixNum = 0;
		MixerSound *sound = mixer->sounds;
		while(sound) {
			if(sound->playing) {
				for(int c=0; c < MIXER_NUM_CHANNELS; c++) {
					float sampleA = mixResults[c];
					float sampleB = getSampleAsFloat(sound, sound->playbackOffset, c);
					
					if(mixNum == 0) {
						mixResults[c] = sampleB;
					} else {
						mixResults[c] = mixSamples(sampleA, sampleB);
					}
				}
				sound->playbackOffset++;
				float adjustedOffset = ((float)sound->playbackOffset) * sound->pitch * sound->frequencyAdjust;
				
				if((unsigned int)adjustedOffset >= sound->numSamples) {
					sound->playbackOffset = 0;
					if(!sound->looped) {
						sound->playing = 0;
					}
				}
				mixNum++;

			}
			sound = sound->next;
		}
	
		
		for(int c=0; c < MIXER_NUM_CHANNELS; c++) {
			*bufferPtr = (int16_t)(((float)INT16_MAX) * (mixResults[c] * mixer->volume));
			bufferPtr++;
		}
	}
}


#endif

