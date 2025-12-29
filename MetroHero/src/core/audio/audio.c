#define _CRT_SECURE_NO_WARNINGS
#include "audio.h"
#include "raylib.h"
#include <string.h>
#include <stdio.h>

static Music currentMusic = { 0 };
static char currentMusicPath[256] = { 0 };
static int isMusicLoaded = 0;

void audio_init(void) {
    InitAudioDevice();
    // 볼륨 기본 설정 (0.0 ~ 1.0)
    SetMasterVolume(0.5f);
}

void audio_close(void) {
    if (isMusicLoaded) {
        UnloadMusicStream(currentMusic);
        isMusicLoaded = 0;
    }
    CloseAudioDevice();
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
