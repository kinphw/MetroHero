#ifndef AUDIO_SYSTEM_H
#define AUDIO_SYSTEM_H

// 오디오 시스템 초기화 및 종류
void audio_init(void);
void audio_close(void);

// BGM 재생 (경로 기반)
// 이미 같은 곡이 재생 중이면 끊지 않고 계속 재생.
void audio_play_music(const char* filePath);

// BGM 정지
void audio_stop_music(void);

// 매 프레임 호출 필요 (스트리밍 버퍼 갱신)
void audio_update(void);

#endif
