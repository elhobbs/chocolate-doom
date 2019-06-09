#include "SDL.h"
#include <3ds.h>
#include <string.h>
#include <malloc.h>
#include <stdio.h>

int SDL_LockSurface(SDL_Surface * surface) {
	return 0;
}
void SDL_UnlockSurface(SDL_Surface * surface) {

}

SDL_Surface* SDL_SetVideoMode(int width, int height, int bitsperpixel, u32 flags) {
	int cb = (bitsperpixel + 7) / 8;
	SDL_Surface *surf = (SDL_Surface*)malloc(sizeof(SDL_Surface));
	//printf("surf %p\n", surf);
	waithere("surf");
	surf->w = width;
	surf->h = height;
	surf->pitch = cb * width;
	surf->pixels = (u8 *)linearAlloc(cb * width * height);
	//printf("surf %p %d %d\n", surf, width, height);
	return surf;
}

void SDL_FreeSurface(SDL_Surface *s) {
	if (s) {
		if (s->pixels) {
			linearFree(s->pixels);
		}
		free(s);
	}
}

SDL_Surface *SDL_CreateRGBSurface(u32 flags, int width, int height, int depth, u32 Rmask, u32 Gmask, u32 Bmask, u32 Amask) {
	int cb = (depth + 7) / 8;
	SDL_Surface *surf = (SDL_Surface*)malloc(sizeof(SDL_Surface));
	surf->w = width;
	surf->h = height;
	surf->pitch = cb * width;
	surf->pixels = (u8 *)linearAlloc(cb * width * height);
	return surf;
}

void SDL_SetColors(SDL_Surface *screenbuffer, SDL_Color *ega_colors, int start, int count) {
	//printf("sb: %p pal: %p start: %d count: %d colors: %p\n", screenbuffer, screenbuffer->palette, start, count, ega_colors);
	waithere("SDL_SetColors");
	memcpy(&screenbuffer->palette[start], ega_colors, count * sizeof(SDL_Color));
}

uint32_t SDL_GetTicks(void) {
	return (uint32_t)osGetTime();
}

int SDL_BlitSurface(SDL_Surface * surf_src, const SDL_Rect * srcrect, SDL_Surface * surf_dst, SDL_Rect * dstrect) {
	u8* bufAdr = gfxGetFramebuffer(GFX_TOP, GFX_LEFT, NULL, NULL);
	u8 *src = surf_src->pixels;
	SDL_Color *pal = surf_src->palette;
	int w, h;

	//printf("surf_src: %d %d %d\n", surf_src->w, surf_src->h, surf_src->pitch);
	//printf("surf_dst: %d %d %d\n", surf_dst->w, surf_dst->h, surf_dst->pitch);
	//printf("dstrect: %d %d %d %d\n", dstrect->x, dstrect->y, dstrect->w, dstrect->h);

	for (w = 0; w < surf_src->w; w++) {
		for (h = 0; h < surf_src->h; h++) {
			u32 srcofs = h * 320 + w;
			u32 dstofs = ((w+dstrect->x)*240 + (239 - h - dstrect->y)) * 3;
			bufAdr[dstofs++] = pal[src[srcofs]].b;
			bufAdr[dstofs++] = pal[src[srcofs]].g;
			bufAdr[dstofs] = pal[src[srcofs]].r;
		}
	}
	return 0;
}

void SDL_UpdateRects(SDL_Surface *screen, int numrects, SDL_Rect *rects) {
	gfxSwapBuffers();
}

int SDL_FillRect(SDL_Surface * dst, const SDL_Rect * rect, u32 color) {
	return 0;
}

int SDL_Flip(SDL_Surface *screen) {
	gfxSwapBuffers();
	return 0;
}


void SDL_GetMouseState(int *x, int *y) {
	touchPosition touch = { 0,0 };

	if (keysDown() & KEY_TOUCH)
	{
		touchRead(&touch);
		*x = touch.px;
		*y = touch.py;
	}
}

void SDL_Delay(u32 ms) {

}

const char *SDL_GetError(void) {
	return 0;
}

int SDL_WaitEvent(SDL_Event * event) {
	return 1;
}

static int _keys_down;
static int _keys_up;
static int _keys_index;
static int _keys[32] = {
	SDLK_LCTRL,
	SDLK_SPACE,
	SDLK_ESCAPE,
	SDLK_RETURN,
	SDLK_RIGHT,
	SDLK_LEFT,
	SDLK_UP,
	SDLK_DOWN,
	SDLK_PERIOD,
	SDLK_COMMA,
	SDLK_RSHIFT,
	'y',
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0
};

void SDL_PumpEvents(void) {
	scanKeys();
	_keys_down = keysDown();
	_keys_up = keysUp();
	_keys_index = 0;
	//printf("keys %08x %08x\n", _keys_down, _keys_up);
}

int SDL_PollEvent(SDL_Event * ev) {
	if ((_keys_down | _keys_up) && _keys_index < 32) {
		int i;
		for (i = 0; i < 32; i++) {
			if (_keys_up & BIT(i)) {
				SDL_KeyboardEvent *key_ev = (SDL_KeyboardEvent *)ev;
				key_ev->type = SDL_KEYUP;
				key_ev->keysym.sym = _keys[i];
				key_ev->keysym.scancode = (_keys[i] & (~SDLK_SCANCODE_MASK));
				_keys_up &= ~BIT(i);
				return 1;
			}
			if (_keys_down & BIT(i)) {
				SDL_KeyboardEvent *key_ev = (SDL_KeyboardEvent *)ev;
				key_ev->type = SDL_KEYDOWN;
				key_ev->keysym.sym = _keys[i];
				key_ev->keysym.scancode = (_keys[i] & (~SDLK_SCANCODE_MASK));
				_keys_down &= ~BIT(i);
				return 1;
			}
			_keys_index = i;
		}
		_keys_index = 32;
	}
	return 0;
}

static 	ndspWaveBuf *waveBuf;

int Mix_HaltChannel(int channel) {
	if (channel < 0 || channel >= 16) {
		return 0;
	}
	//printf("halt %d\n", which);
	ndspChnWaveBufClear(channel);
	return 0;
}

int Mix_UnregisterAllEffects(int channel) {
	return 0;
}

int Mix_SetPanning(int channel, Uint8 left, Uint8 right) {
	if (channel < 0 || channel >= 16) {
		return 0;
	}
	float mix[12];
	memset(mix, 0, sizeof(mix));
	mix[0] = left/255.0f;
	mix[1] = right/255.0f;
	ndspChnSetMix(channel, mix);
	return 0;
}

__attribute__((inline)) u32 __get_SP(void)
{
	register uint32_t result;

	asm volatile ("MOV %0, SP\n" : "=r" (result));
	return(result);
}

int Mix_PlayChannel(int channel, Mix_Chunk *chunk, int loops) {
	if (channel < 0 || channel >= 16) {
		return 0;
	}
	//printf("channel %d %p\n", channel, chunk);
	//if (!chunk) return;
	//printf("chunk %d %p\n", chunk->alen, chunk->abuf);
	//return 0;
	//printf("play %d\n", channel);

	ndspChnSetInterp(channel, NDSP_INTERP_LINEAR);
	ndspChnSetRate(channel, NDSP_SAMPLE_RATE);
	ndspChnSetFormat(channel, NDSP_FORMAT_STEREO_PCM16);

	// Output at 100% on the first pair of left and right channels.

	float mix[12];
	memset(mix, 0, sizeof(mix));
	mix[0] = 1.0;
	mix[1] = 1.0;
	ndspChnSetMix(channel, mix);
	
	memset(&waveBuf[channel], 0, sizeof(waveBuf));
	waveBuf[channel].data_vaddr = chunk->abuf;
	waveBuf[channel].nsamples = chunk->alen/4;
	//u32 sp1 = __get_SP();
	//printf("sp1 %08x\n", sp1);
	ndspChnWaveBufAdd(channel, &waveBuf[channel]);
	//u32 sp2 = __get_SP();
	//printf("sp2 %08x\n", sp2);

	return 0;
}

int Mix_AllocateChannels(int numchans) {
	waveBuf = (ndspWaveBuf *)linearAlloc(numchans * sizeof(ndspWaveBuf));
	return numchans;
}

void SDL_PauseAudio(int pause_on) {
}

int Mix_Playing(int channel) {
	if (channel < 0 || channel >= 16) {
		return 0;
	}
	//printf("playing %d %d\n", which, waveBuf[which].status);
	return (waveBuf[channel].status == NDSP_WBUF_PLAYING || waveBuf[channel].status == NDSP_WBUF_QUEUED) ? 1 : 0;
}

SDL_mutex* SDL_CreateMutex() {
	SDL_mutex* mutex = malloc(sizeof(*mutex));
	LightLock_Init(mutex);
	return mutex;
}

void SDL_LockMutex(SDL_mutex *mutex) {
	LightLock_Lock(mutex);
}

void SDL_UnlockMutex(SDL_mutex *mutex) {
	LightLock_Unlock(mutex);
}

void SDL_DestroyMutex(SDL_mutex *mutex) {
	if (mutex) {
		free(mutex);
	}
}

/*SDL_cond *SDL_CreateCond() {
	SDL_cond* cond = malloc(sizeof(*cond));
	LightEvent_Init(cond, RESET_STICKY);
	return cond;
}

void SDL_CondSignal(SDL_cond *cond) {
	LightEvent_Signal(cond);
}

void SDL_CondWait(SDL_cond *cond, SDL_mutex *mutex) {
	LightEvent_Wait(cond);
}

void SDL_DestroyCond(SDL_cond *cond) {
	if (cond) {
		free(cond);
	}
}*/

