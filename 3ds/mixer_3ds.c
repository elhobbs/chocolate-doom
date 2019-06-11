#include "SDL.h"
#include <3ds.h>
#include <stdio.h>
#include <string.h>

#define MUS_MIX_CHANNEL 23
#define MUS_PLAYBACK_RATE 11025

#define	WAV_BUFFERS				128
#define	WAV_MASK				0x7F
#define	WAV_BUFFER_SIZE			0x0400

static int ndsp_channels = 2;
static int ndsp_samplebits = 16;
static int ndsp_speed = MUS_PLAYBACK_RATE;
static int ndsp_chunksize = 1024;
static int ndsp_size = WAV_BUFFERS * WAV_BUFFER_SIZE;
static void *ndsp_buffer = 0;
static int ndsp_initialized = 0;
static float ndsp_mix[12];

static int ndsp_sent, ndsp_completed;
static ndspWaveBuf ndsp_waveBufs[WAV_BUFFERS];

void (*ndsp_mix_func)(void *udata, Uint8 *stream, int len) = 0;
void *ndsp_mix_arg = 0;


void ndsp_submit(void) {
	if (!ndsp_initialized)
		return;
	//printf("%d %d\n", ndsp_completed, ndsp_sent);
	//fflush(stdout);
	while (1)
	{
		if (ndsp_completed == ndsp_sent) {
			break;
		}
		if (ndsp_waveBufs[ndsp_completed & WAV_MASK].status != NDSP_WBUF_DONE) {
			//printf("comp: %d\n", ndsp_waveBufs[ndsp_completed & WAV_MASK].status);
			break;
		}
		ndsp_completed++;	// this buffer has been played
	}

	//
	// submit new sound blocks
	//
	while ((ndsp_sent - ndsp_completed) < 32) {
		ndspWaveBuf *buffer = ndsp_waveBufs + (ndsp_sent&WAV_MASK);
		if (ndsp_mix_func) {
			ndsp_mix_func(ndsp_mix_arg, buffer->data_adpcm, WAV_BUFFER_SIZE);
			DSP_FlushDataCache(buffer->data_vaddr, WAV_BUFFER_SIZE);
		}
		ndspChnWaveBufAdd(MUS_MIX_CHANNEL, buffer);
		ndsp_sent++;
	}
}

static void ndsp_init() {
	int i;

	ndsp_sent = 0;
	ndsp_completed = 0;

	ndsp_buffer = linearAlloc(ndsp_size);
	if (!ndsp_buffer)
	{
		printf("Sound: Out of memory.\n");
		return;
	}
	memset(ndsp_buffer, 0, ndsp_size);

	ndspChnSetInterp(MUS_MIX_CHANNEL, NDSP_INTERP_NONE);
	ndspChnSetRate(MUS_MIX_CHANNEL, (float)ndsp_speed);
	ndspChnSetFormat(MUS_MIX_CHANNEL, NDSP_FORMAT_STEREO_PCM16);
	memset((void *)ndsp_waveBufs, 0, sizeof(ndspWaveBuf)*WAV_BUFFERS);
	memset((void *)ndsp_mix, 0, sizeof(ndsp_mix));

	for (i = 0; i < WAV_BUFFERS; i++)
	{
		ndsp_waveBufs[i].nsamples = WAV_BUFFER_SIZE / (ndsp_channels * ndsp_samplebits / 8);
		ndsp_waveBufs[i].looping = 0;
		ndsp_waveBufs[i].status = NDSP_WBUF_FREE;
		ndsp_waveBufs[i].data_vaddr = ndsp_buffer + i * WAV_BUFFER_SIZE;
	}

	ndsp_initialized = 1;
}

static void dsp_shutdown() {
	if (!ndsp_initialized) {
		return;
	}
	ndspChnWaveBufClear(MUS_MIX_CHANNEL);
	svcSleepThread(20000);
	linearFree(ndsp_buffer);
	ndsp_initialized = 0;
}


#define NDSP_STACK_SIZE 0x80000

static Thread ndsp_thread;
//static LightLock ndsp_lock;
static volatile int ndsp_thread_quit = 0;

static void ndsp_thread_main(void* arg) {
	int64_t waitns = (1000000000LL * WAV_BUFFER_SIZE * 2) / (ndsp_speed * ndsp_channels * 2);
	//printf("ndsp_thread_main %lld %d %d\n", waitns, ndsp_speed, ndsp_channels);
	svcSleepThread(waitns);
	svcSleepThread(waitns);
	svcSleepThread(waitns);
	svcSleepThread(waitns);
	do {
		//printf(".");
		//fflush(stdout);
		ndsp_submit();
		//LightLock_Unlock(&ndsp_lock);
		svcSleepThread(waitns);
	} while (!ndsp_thread_quit);
}


int Mix_OpenAudio(int frequency, Uint16 format, int nchannels, int chunksize) {

	ndsp_speed = frequency;
	ndsp_channels = nchannels;
	ndsp_chunksize = chunksize;

	//printf("music speed: %d channel: %d chuck: %d\n", ndsp_speed, ndsp_channels, ndsp_chunksize);
	//while (1);

	//LightLock_Init(&ndsp_lock);
	ndsp_init();
	ndsp_thread_quit = 0;
	//ndsp_thread = threadCreate(ndsp_thread_main, 0, NDSP_STACK_SIZE, 0x18, -2, true);

	//printf("Mix_OpenAudio\n");
	//while (1);

	return 0;
}

int Mix_QuerySpec(int *frequency, Uint16 *format, int *channels) {
	*frequency = ndsp_speed;
	*format = NDSP_FORMAT_STEREO_PCM16;
	*channels = ndsp_channels;
	//printf("Mix_QuerySpec\n");

	return 0;
}

void Mix_HookMusic(void(*mix_func)(void *udata, Uint8 *stream, int len), void *arg) {
	ndsp_mix_func = mix_func;
	ndsp_mix_arg = arg;
	//printf("Mix_HookMusic\n");
}

void Mix_CloseAudio(void) {

	//printf("Mix_CloseAudio\n");
	//LightLock_Lock(&ndsp_lock);
	ndsp_thread_quit = 1;
	//printf("waiting to join\n");
	//threadJoin(ndsp_thread, U64_MAX);
	//printf("joined\n");
	//while (1);

	dsp_shutdown();
}
