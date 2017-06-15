#include "Character.h"

Character::Character(TYPE _type, STATE_ID _state, D2D_POINT_2F _pos,
					 BOOL _exist, int _fishNumber, float _velocity, BOOL _isLeft, int _ID)
{	// 물고기 생성자
	type = _type;
	state = _state;
	pos = _pos;
	exist = _exist;
	fishNumber = _fishNumber;
	velocity = _velocity;
	isLeft = _isLeft;
	targetID = -1;
	ID = _ID;
}

Character::Character(TYPE _type, STATE_ID _state, BOOL _exist,
					 int _health, float _velocity, BOOL _isLeft)
{	// 상어 생성자
	type = _type;
	state = _state;
	exist = _exist;
	health = _health;
	velocity = _velocity;
	isLeft = _isLeft;
	targetID = -1;
}

Character::Character(TYPE _type, D2D1_POINT_2F _pos, int _fishNumber)
{	// 마우스 따라다니는 물고기 생성자
	type = _type;
	fishNumber = _fishNumber;
	pos = _pos;
}

Character::~Character()
{
}

void Character::CharacterAction(D2D_SIZE_F rtSize, std::queue <FOOD*> food, Character shark)
{
	if (state == STATE_STAND)
	{
		// 대기상태이면 새로운 목적지 좌표를 정한다.
		velocity = 15.0f;
		NewRandomPos(rtSize);

		while (!food.empty())
		{
			if (isVisible(food.front(), 130.0f))
			{	// 먹이가 사거리 130 안에 들어오면 해당 먹이 추적 시작
				targetID = food.front()->ID;
				setDestPos(food.front()->pos);
				break;
			}
			food.pop();
		}

		if (shark.exist && isVisible(&shark, DETECTION_RANGE))
		{	// 탐지범위 내에 상어가 있으면 도망감. 먹이 추적보다 우선순위가 높음.
			state = STATE_RUNAWAY;
		}

		state = STATE_MOVE;
	}

	else if (state == STATE_MOVE)
	{
		velocity = 15.0f;

		while (!food.empty())
		{
			if (isVisible(food.front(), 100.0f))
			{	// 음식이 사거리 100 안에 들어오면 추적 모드로 변경
				setDestPos(food.front()->pos);
				state = STATE_FOLLOW;
				break;
			}
			food.pop();
		}

		moveToDestPos();

		if (shark.exist && isVisible(&shark, DETECTION_RANGE))
		{	// 탐지범위 내에 상어가 있으면 도망감.
			state = STATE_RUNAWAY;
		}
	}

	else if (state == STATE_FOLLOW)
	{
		velocity = 20.0f;
		BOOL trace = false;

		while (!food.empty())
		{
			if (isVisible(food.front(), 100.0f) && targetID == food.front()->ID)
			{	// 추적 시작
				setDestPos(food.front()->pos);
				trace = true;
				break;
			}
			food.pop();
		}

		moveToDestPos();

		if (trace)
		{
			float distance = sqrt(pow((food.front()->pos.x - pos.x), 2) + pow((food.front()->pos.y - pos.y), 2));
			// 두 점 사이의 거리공식 사용
			if (distance < 30.0f)
			{
				food.front()->exist = false;
			}
		}
		else
		{	// 추적 대상이 없으면 다시 stand 모드
			targetID = -1;
			state = STATE_STAND;
		}

		if (shark.exist && isVisible(&shark, DETECTION_RANGE))
		{	// 탐지범위 내에 상어가 있으면 도망감.
			state = STATE_RUNAWAY;
		}
	}
	else if (state == STATE_RUNAWAY)
	{
		velocity = 30.0f;
		D2D1_POINT_2F dest;

		if (shark.exist && isVisible(&shark, DETECTION_RANGE + 50.0f))
		{	// 도망 상태 진입 시 상어 탐지 범위가 넓어짐
			state = STATE_RUNAWAY;
		}
		else
		{
			state = STATE_STAND;
		}

		float a = (shark.pos.y - pos.y) / (shark.pos.x - pos.x);
		float b = pos.y - a * pos.x;	// y=ax+b의 식 구하기

		if (pos.x > shark.pos.x)
		{
			dest.x = rtSize.width;
			dest.y = a * dest.x + b;
		}
		else
		{
			dest.x = 0;
			dest.y = a * dest.x + b;
		}

		moveToDestPos();
	}
}

void Character::CharacterAction(D2D_SIZE_F rtSize, std::queue <Character*> fish, D2D_POINT_2F p)
{
	if (state == STATE_STAND)
	{
		// 대기상태이면 새로운 목적지 좌표를 정한다.
		NewRandomPos(rtSize);
		velocity = 25.0f;

		while (!fish.empty())
		{
			if (isVisible(fish.front(), 250.0f))
			{	// 물고기가 사거리 250 안에 들어오면 따라가기 시작
				targetID = fish.front()->ID;
				setDestPos(fish.front()->pos);
				break;
			}
			fish.pop();
		}
		state = STATE_MOVE;
	}

	else if (state == STATE_MOVE)
	{
		while (!fish.empty())
		{
			if (isVisible(fish.front(), 250.0f))
			{	// 물고기가 사거리 250 안에 들어오면 추적 상태로 변경
				setDestPos(fish.front()->pos);
				state = STATE_FOLLOW;
				break;
			}
			fish.pop();
		}
		moveToDestPos();
	}

	else if (state == STATE_FOLLOW)
	{
		BOOL trace = false;

		velocity = 40.0f;

		while (!fish.empty())
		{
			if (isVisible(fish.front(), 180.0f) && targetID == fish.front()->ID)
			{	// 추적 시작
				setDestPos(fish.front()->pos);
				trace = true;
				break;
			}
			fish.pop();
		}

		moveToDestPos();

		if (trace)
		{
			float distance = sqrt(pow((fish.front()->pos.x - pos.x), 2) + pow((fish.front()->pos.y - pos.y), 2));
			// 두 점 사이의 거리공식 사용
			if (distance < 120.0f)
			{	// 일정 거리만큼 접근하면 섭식
				fish.front()->exist = false;
			}
		}
		else
		{	// 추적 대상이 없으면 다시 stand 모드
			targetID = -1;
			state = STATE_STAND;
		}
	}
	else if (state == STATE_RUNAWAY)
	{
		velocity = 60.0f;
		D2D1_POINT_2F dest;
		timeCheck2 = clock();

		if (timeCheck2 - timeCheck1 < 300)
		{	// 일정 시간동안 상어의 도망 상태 유지
			state = STATE_RUNAWAY;
		}
		else
		{
			state = STATE_STAND;
			timeCheck1 = clock();
		}

		float a = (pos.y - p.y) / (pos.x - p.x);
		float b = pos.y - a * pos.x;	// y=ax+b의 식 구하기

		if (pos.x > p.x)
		{
			dest.x = rtSize.width;
			dest.y = a * dest.x + b;
		}
		else
		{
			dest.x = 0;
			dest.y = a * dest.x + b;
		}

		moveToDestPos();
	}
}

void Character::NewRandomPos(D2D_SIZE_F rtSize)
{
	int randomPosX = rand() % (int)(rtSize.width - 20) + 10;
	int randomPosY = rand() % (int)(rtSize.height - 150) + 150;

	destPos.x = (float)randomPosX;
	destPos.y = (float)randomPosY;
}

BOOL Character::isVisible(FOOD *food, float sight)
{
	float distance = sqrt(pow((food->pos.x - pos.x), 2) + pow((food->pos.y - pos.y), 2));
	// 두 점 사이의 거리공식 사용
	
	if (distance < sight)
		return true;

	else
		return false;
}

BOOL Character::isVisible(Character *fish, float sight)
{
	float distance = sqrt(pow((fish->pos.x - pos.x), 2) + pow((fish->pos.y - pos.y), 2));
	// 두 점 사이의 거리공식 사용

	if (distance < sight)
		return true;

	else
		return false;
}

void Character::setDestPos(D2D1_POINT_2F _destPos)
{
	destPos = _destPos;
}

void Character::moveToDestPos()
{
	float dt = 0.1f;

	float distance = sqrt(
		pow((pos.x - destPos.x), 2) +
		pow((pos.y - destPos.y), 2)); // 현재 좌표와 목표 좌표 사이의 거리

	if (distance >= velocity && distance > 17.0f)
	{
		// 현재점 <-> 목적지 사이의 거리가 일정값 이상이면 작동.
		// 속도를 남은 거리로 나눈 값 * x(또는 y)를 좌표값 이동의 곱 비율로 사용
		float ratio = velocity / distance;
		float moveX = abs(destPos.x - pos.x); // 두 x점 사이의 절대값
		float moveY = abs(destPos.y - pos.y); // 두 y점 사이의 절대값

		// 움직일 값이 너무 적으면 동작하지 않음. 더욱 매끄럽게 움직임.
		if (moveX > 12.0f)
		{
			if (pos.x > destPos.x)
			{
				pos.x -= moveX * ratio * dt;
				isLeft = true;
			}
			else
			{
				pos.x += moveX * ratio * dt;
				isLeft = false;
			}
		}

		if (moveY > 12.0f)
		{
			if (pos.y > destPos.y)
				pos.y -= moveY * ratio * dt;
			else
				pos.y += moveY * ratio * dt;
		}
	}

	else
		state = STATE_STAND;
}