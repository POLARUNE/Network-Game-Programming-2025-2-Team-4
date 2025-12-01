#pragma once

#include "CHARACTER.h"

class Canada : public Character
{
public:
	Canada(int playerNum);
	~Canada();

	virtual void Move(int dir, int playerNum) override;
	virtual void Jump(int dir) override;
};