#pragma once

#include "CHARACTER.h"

class Korea : public Character
{
public:
	Korea(int playerNum);
	~Korea();

	virtual void Move(int dir, int playerNum) override;
	virtual void Jump(int dir) override;
};