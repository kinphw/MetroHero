#define _CRT_SECURE_NO_WARNINGS
#include "audio.h"
#include "raylib.h"
#include <string.h>
#include <stdio.h>

static Music currentMusic = { 0 };
static char currentMusicPath[256] = { 0 };
static int isMusicLoaded = 0;

// SFX Support
typedef struct {
    char name[32];
    Sound sound;
} SoundAsset;

#define SFX_COUNT 6
static SoundAsset sfx[SFX_COUNT];
static const char* sfxNames[SFX_COUNT] = {
    "claw_hit",
    "door_creak",
    "explosion",
    "sword_hit",
    "sword_swing",
    "text_blip"
};

void audio_init(void) {
    InitAudioDevice();
    SetMasterVolume(0.5f);

    // Load SFX
    char path[128];
    for (int i = 0; i < SFX_COUNT; i++) {
        snprintf(sfx[i].name, sizeof(sfx[i].name), "%s", sfxNames[i]);
        snprintf(path, sizeof(path), "assets/effect/%s.wav", sfxNames[i]);
        sfx[i].sound = LoadSound(path);
        if (sfx[i].sound.frameCount == 0) {
            printf("[AUDIO] Failed to load SFX: %s\n", path);
        }
    }
}

void audio_close(void) {
    if (isMusicLoaded) {
        UnloadMusicStream(currentMusic);
        isMusicLoaded = 0;
    }
    
    // Unload SFX
    for (int i = 0; i < SFX_COUNT; i++) {
        if (sfx[i].sound.frameCount > 0) {
            UnloadSound(sfx[i].sound);
        }
    }

    CloseAudioDevice();
}

void audio_play_sfx(const char* name) {
    for (int i = 0; i < SFX_COUNT; i++) {
        if (strcmp(sfx[i].name, name) == 0) {
            PlaySound(sfx[i].sound);
            return;
        }
    }
    printf("[AUDIO] SFX not found: %s\n", name);
}

void audio_play_music(const char* filePath) {
    // 1. 이미 같은 파일이 재생 중이라면 무시
    if (isMusicLoaded && strcmp(currentMusicPath, filePath) == 0) {
        return; // Continue playing
    }

    // 2. 기존 음악 정지 및 언로드
    if (isMusicLoaded) {
        StopMusicStream(currentMusic);
        UnloadMusicStream(currentMusic);
        isMusicLoaded = 0;
        memset(currentMusicPath, 0, sizeof(currentMusicPath));
    }

    // 3. 새 음악 로드
    currentMusic = LoadMusicStream(filePath);
    if (currentMusic.stream.buffer == NULL) { 
        printf("[AUDIO] Failed to load music: %s\n", filePath);
    } else {
        printf("[AUDIO] Playing music: %s\n", filePath);
        isMusicLoaded = 1;
        strncpy(currentMusicPath, filePath, sizeof(currentMusicPath) - 1);
        
        currentMusic.looping = 1; // 기본적으로 반복 재생
        PlayMusicStream(currentMusic);
    }
}

void audio_stop_music(void) {
    if (isMusicLoaded) {
        StopMusicStream(currentMusic);
        // Unload까지 할지는 선택. 여기서는 Stop만.
        // 만약 완전히 끌거면 Unload 추천.
        UnloadMusicStream(currentMusic);
        isMusicLoaded = 0;
        memset(currentMusicPath, 0, sizeof(currentMusicPath));
    }
}

void audio_update(void) {
    if (isMusicLoaded) {
        UpdateMusicStream(currentMusic);
    }
}
