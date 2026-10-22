/* Minimal WebAssembly bridge for mGBA (64MiB-capable celio fork). MPL-2.0 (same as mGBA). */
#include <mgba/core/core.h>
#include <mgba/core/config.h>
#include <mgba/core/serialize.h>
#include <mgba-util/audio-buffer.h>
#include <mgba-util/vfs.h>
#include <stdlib.h>
#include <string.h>

#define EXPORT(name) __attribute__((export_name(#name)))

static struct mCore* core;
static uint32_t* video;
static void* rom;
static size_t romSize;
static int16_t audio[8192 * 2];
static void* blob;        /* last exported state / save */
static size_t blobSize;

EXPORT(wbuf_alloc) void* wbuf_alloc(size_t n) { return malloc(n); }
EXPORT(wbuf_free) void wbuf_free(void* p) { free(p); }

EXPORT(emu_load) int emu_load(void* data, size_t size) {
	if (core) { core->deinit(core); core = NULL; }
	free(rom); rom = data; romSize = size;
	struct VFile* vf = VFileFromMemory(rom, romSize);
	if (!vf) return 0;
	core = mCoreFindVF(vf);
	if (!core) { vf->close(vf); return 0; }
	core->init(core);
	mCoreInitConfig(core, NULL);
	struct mCoreOptions opts = { .useBios = false, .skipBios = true, .volume = 0x100, .sampleRate = 32768 };
	mCoreConfigLoadDefaults(&core->config, &opts);
	mCoreLoadConfig(core);
	unsigned w, h; core->baseVideoSize(core, &w, &h);
	if (!video) video = malloc(256 * 256 * 4);
	core->setVideoBuffer(core, (mColor*) video, 256);
	core->setAudioBufferSize(core, 4096);
	if (!core->loadROM(core, vf)) { core->deinit(core); core = NULL; return 0; }
	core->reset(core);
	return 1;
}
EXPORT(emu_reset) void emu_reset(void) { if (core) core->reset(core); }
EXPORT(emu_frame) void emu_frame(int keys) { if (!core) return; core->setKeys(core, keys); core->runFrame(core); }
EXPORT(emu_video) void* emu_video(void) { return video; }
EXPORT(emu_audio_rate) int emu_audio_rate(void) { return core ? (int) core->audioSampleRate(core) : 32768; }
/* returns number of stereo frames copied into emu_audio_buf() */
EXPORT(emu_audio_read) int emu_audio_read(void) {
	if (!core) return 0;
	struct mAudioBuffer* ab = core->getAudioBuffer(core);
	size_t n = mAudioBufferAvailable(ab);
	if (n > 8192) n = 8192;
	return (int) mAudioBufferRead(ab, audio, n);
}
EXPORT(emu_audio_buf) void* emu_audio_buf(void) { return audio; }
EXPORT(emu_blob) void* emu_blob(void) { return blob; }

/* in-game save (SRAM / flash) */
EXPORT(emu_save_export) size_t emu_save_export(void) {
	free(blob); blob = NULL; blobSize = 0;
	if (!core) return 0;
	void* sram = NULL;
	size_t n = core->savedataClone(core, &sram);
	blob = sram; blobSize = n;
	return n;
}
EXPORT(emu_save_import) int emu_save_import(void* data, size_t size) {
	if (!core) return 0;
	return core->savedataRestore(core, data, size, false);
}

/* quick state (includes save RAM + RTC, like the celio web build) */
EXPORT(emu_state_export) size_t emu_state_export(void) {
	free(blob); blob = NULL; blobSize = 0;
	if (!core) return 0;
	struct VFile* vf = VFileMemChunk(NULL, 0);
	if (!vf) return 0;
	if (!mCoreSaveStateNamed(core, vf, SAVESTATE_SAVEDATA | SAVESTATE_RTC)) { vf->close(vf); return 0; }
	ssize_t size = vf->size(vf);
	if (size <= 0) { vf->close(vf); return 0; }
	blob = malloc(size);
	vf->seek(vf, 0, SEEK_SET);
	if (vf->read(vf, blob, size) != size) { vf->close(vf); free(blob); blob = NULL; return 0; }
	vf->close(vf);
	blobSize = size;
	return size;
}
EXPORT(emu_state_import) int emu_state_import(void* data, size_t size) {
	if (!core) return 0;
	struct VFile* vf = VFileFromConstMemory(data, size);
	if (!vf) return 0;
	int ok = mCoreLoadStateNamed(core, vf, SAVESTATE_SAVEDATA | SAVESTATE_RTC);
	vf->close(vf);
	return ok;
}
int main(void) { return 0; }
