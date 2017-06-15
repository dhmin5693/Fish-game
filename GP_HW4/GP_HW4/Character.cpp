#include "Character.h"

Character::Character(TYPE _type, STATE_ID _state, D2D_POINT_2F _pos,
					 BOOL _exist, int _fishNumber, float _velocity, BOOL _isLeft, int _ID)
{	// ����� ������
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
{	// ��� ������
	type = _type;
	state = _state;
	exist = _exist;
	health = _health;
	velocity = _velocity;
	isLeft = _isLeft;
	targetID = -1;
}

Character::Character(TYPE _type, D2D1_POINT_2F _pos, int _fishNumber)
{	// ���콺 ����ٴϴ� ����� ������
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
		// �������̸� ���ο� ������ ��ǥ�� ���Ѵ�.
		velocity = 15.0f;
		NewRandomPos(rtSize);

		while (!food.empty())
		{
			if (isVisible(food.front(), 130.0f))
			{	// ���̰� ��Ÿ� 130 �ȿ� ������ �ش� ���� ���� ����
				targetID = food.front()->ID;
				setDestPos(food.front()->pos);
				break;
			}
			food.pop();
		}

		if (shark.exist && isVisible(&shark, DETECTION_RANGE))
		{	// Ž������ ���� �� ������ ������. ���� �������� �켱������ ����.
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
			{	// ������ ��Ÿ� 100 �ȿ� ������ ���� ���� ����
				setDestPos(food.front()->pos);
				state = STATE_FOLLOW;
				break;
			}
			food.pop();
		}

		moveToDestPos();

		if (shark.exist && isVisible(&shark, DETECTION_RANGE))
		{	// Ž������ ���� �� ������ ������.
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
			{	// ���� ����
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
			// �� �� ������ �Ÿ����� ���
			if (distance < 30.0f)
			{
				food.front()->exist = false;
			}
		}
		else
		{	// ���� ����� ������ �ٽ� stand ���
			targetID = -1;
			state = STATE_STAND;
		}

		if (shark.exist && isVisible(&shark, DETECTION_RANGE))
		{	// Ž������ ���� �� ������ ������.
			state = STATE_RUNAWAY;
		}
	}
	else if (state == STATE_RUNAWAY)
	{
		velocity = 30.0f;
		D2D1_POINT_2F dest;

		if (shark.exist && isVisible(&shark, DETECTION_RANGE + 50.0f))
		{	// ���� ���� ���� �� ��� Ž�� ������ �о���
			state = STATE_RUNAWAY;
		}
		else
		{
			state = STATE_STAND;
		}

		float a = (shark.pos.y - pos.y) / (shark.pos.x - pos.x);
		float b = pos.y - a * pos.x;	// y=ax+b�� �� ���ϱ�

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
		// �������̸� ���ο� ������ ��ǥ�� ���Ѵ�.
		NewRandomPos(rtSize);
		velocity = 25.0f;

		while (!fish.empty())
		{
			if (isVisible(fish.front(), 250.0f))
			{	// ����Ⱑ ��Ÿ� 250 �ȿ� ������ ���󰡱� ����
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
			{	// ����Ⱑ ��Ÿ� 250 �ȿ� ������ ���� ���·� ����
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
			{	// ���� ����
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
			// �� �� ������ �Ÿ����� ���
			if (distance < 120.0f)
			{	// ���� �Ÿ���ŭ �����ϸ� ����
				fish.front()->exist = false;
			}
		}
		else
		{	// ���� ����� ������ �ٽ� stand ���
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
		{	// ���� �ð����� ����� ���� ���� ����
			state = STATE_RUNAWAY;
		}
		else
		{
			state = STATE_STAND;
			timeCheck1 = clock();
		}

		float a = (pos.y - p.y) / (pos.x - p.x);
		float b = pos.y - a * pos.x;	// y=ax+b�� �� ���ϱ�

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
	// �� �� ������ �Ÿ����� ���
	
	if (distance < sight)
		return true;

	else
		return false;
}

BOOL Character::isVisible(Character *fish, float sight)
{
	float distance = sqrt(pow((fish->pos.x - pos.x), 2) + pow((fish->pos.y - pos.y), 2));
	// �� �� ������ �Ÿ����� ���

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
		pow((pos.y - destPos.y), 2)); // ���� ��ǥ�� ��ǥ ��ǥ ������ �Ÿ�

	if (distance >= velocity && distance > 17.0f)
	{
		// ������ <-> ������ ������ �Ÿ��� ������ �̻��̸� �۵�.
		// �ӵ��� ���� �Ÿ��� ���� �� * x(�Ǵ� y)�� ��ǥ�� �̵��� �� ������ ���
		float ratio = velocity / distance;
		float moveX = abs(destPos.x - pos.x); // �� x�� ������ ���밪
		float moveY = abs(destPos.y - pos.y); // �� y�� ������ ���밪

		// ������ ���� �ʹ� ������ �������� ����. ���� �Ų����� ������.
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