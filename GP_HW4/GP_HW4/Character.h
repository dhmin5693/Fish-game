#pragma once
#include "main.h"

#define DEFAULT_VELOCITY_FISH	15.0f
#define DEFAULT_VELOCITY_SHARK	30.0f
#define DEFAULT_ID_FISH			10000
#define DEFAULT_ID_FOOD			0
#define DETECTION_RANGE			200.0f

typedef struct
{
	D2D_POINT_2F pos;
	BOOL exist;
	float velocity;
	int ID;
} FOOD; // 먹이 구조체

class Character
{
public:
	enum STATE_ID
	{
		STATE_UNDEFINED = 0,
		STATE_STAND,
		STATE_MOVE,
		STATE_FOLLOW,
		STATE_RUNAWAY,
	};	// 상태 정의

	enum TYPE
	{
		TYPE_FISH = 0,
		TYPE_SHARK,
	};	// 캐릭터의 종류

	STATE_ID state;
	TYPE type;

	D2D_POINT_2F pos;		// 현재 위치
	D2D_POINT_2F destPos;	// 목표 위치
	BOOL exist;				// 존재하고 있으면 true
	int fishNumber;			// 물고기의 3가지 타입 표시
	int health;				// 괴물의 체력
	float velocity;			// 속도
	BOOL isLeft;			// 좌우대칭을 위한 변수
	int targetID;			// 목표로 잡을 대상의 고유값 ID를 저장
	int ID;					// 고유값 ID(물고기용)
	clock_t timeCheck1, timeCheck2;	// 시간 제어용

public:
	Character(TYPE _type, STATE_ID _state, D2D_POINT_2F _pos, 
		BOOL _exist, int _fishNumber, float _velocity, BOOL _isLeft, int _ID); // 물고기 생성자
	Character(TYPE _type, STATE_ID _state, BOOL _exist, 
		int _health, float _velocity, BOOL isLeft); // 상어 생성자
	Character(TYPE _type, D2D1_POINT_2F _pos, int _fishNumber);
	~Character();

	void CharacterAction(D2D_SIZE_F rtSize, std::queue <FOOD*> food, Character _shark);
	void CharacterAction(D2D_SIZE_F rtSize, std::queue <Character*> fish, D2D_POINT_2F p);
	void NewRandomPos(D2D_SIZE_F rtSize);
	BOOL isVisible(FOOD *food, float sight);
	BOOL isVisible(Character *fish, float sight);
	void setDestPos(D2D1_POINT_2F _destPos);
	void moveToDestPos();
};