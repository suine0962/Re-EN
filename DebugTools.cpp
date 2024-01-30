﻿#include "DebugTools.h"

DebugTools* DebugTools::GetInstance()
{
	static DebugTools instance;
	return &instance;
}

void DebugTools::Initialize()
{
#ifdef _DEBUG


	Grid *grid = new Grid();
	Grid::Initialize();
	//GridCommandをセット
	DebugTools::addCommand(grid);

#endif // _DEBUG

}


void DebugTools::addCommand(ICommand *command)
{
	DebugTools::GetInstance()->commands.push_back(command);
}

void DebugTools::Execute(int buttonIndex)
{
	if (buttonIndex >= 0 && buttonIndex < DebugTools::GetInstance()->commands.size()) 
	{
		DebugTools::GetInstance()->commands[buttonIndex]->execute(DebugTools::GetInstance()->viewProjection_);
	}
	else 
	{
		LogManager::Log("None_Command\n");
	}

}
