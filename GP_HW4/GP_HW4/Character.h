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
} FOOD; // ���� ����ü

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
	};	// ���� ����

	enum TYPE
	{
		TYPE_FISH = 0,
		TYPE_SHARK,
	};	// ĳ������ ����

	STATE_ID state;
	TYPE type;

	D2D_POINT_2F pos;		// ���� ��ġ
	D2D_POINT_2F destPos;	// ��ǥ ��ġ
	BOOL exist;				// �����ϰ� ������ true
	int fishNumber;			// ������� 3���� Ÿ�� ǥ��
	int health;				// ������ ü��
	float velocity;			// �ӵ�
	BOOL isLeft;			// �¿��Ī�� ���� ����
	int targetID;			// ��ǥ�� ���� ����� ������ ID�� ����
	int ID;					// ������ ID(������)
	clock_t timeCheck1, timeCheck2;	// �ð� �����

public:
	Character(TYPE _type, STATE_ID _state, D2D_POINT_2F _pos, 
		BOOL _exist, int _fishNumber, float _velocity, BOOL _isLeft, int _ID); // ����� ������
	Character(TYPE _type, STATE_ID _state, BOOL _exist, 
		int _health, float _velocity, BOOL isLeft); // ��� ������
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