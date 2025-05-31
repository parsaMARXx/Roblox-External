#pragma once
#include <Windows.h>
#include <iostream>


namespace Offsets {
	inline uintptr_t FakeDataModelPointer = 0x66EA5E8;
	inline uintptr_t FakeDataModelToRealDataModel = 0x1b8;

	inline uintptr_t Children = 0x80;
	inline uintptr_t Name = 0x78;
	inline uintptr_t Parent = 0x50;

	inline uintptr_t ChildrenEnd = 0x8;
	inline uintptr_t ObjectValue = 0xD8;

	inline uintptr_t LocalPlayer = 0x128;
	inline uintptr_t JumpPower = 0x1B8;
	inline uintptr_t WalkSpeed = 0x1D8;
	inline uintptr_t WalkSpeedCheck = 0x3B0;
	inline uintptr_t Health = 0x19C;

}

namespace Globals {
	inline uintptr_t datamodel;
	inline BYTE* Base;
	inline DWORD Pid;



	inline uintptr_t PlayerPtr = 0x0;
	inline uintptr_t WorkspacePtr = 0x0;
	inline uintptr_t HumanoidPtr = 0x0;
	inline uintptr_t CameraPtr = 0x0;
}