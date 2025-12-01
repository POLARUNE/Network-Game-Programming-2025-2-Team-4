#pragma once

#include "CHARACTER.h"

class Brazil : public Character
{
public:
	Brazil(int playerNum);
	~Brazil();

	virtual void Move(int dir, int playerNum) override;
	virtual void Jump(int dir) override;
};