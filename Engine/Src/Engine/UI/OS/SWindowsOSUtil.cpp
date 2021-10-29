#include "Engine\pch.h"
#include "SWindowsOSUtil.h"
#include <Windows.h>
#include <WinUser.h>


void SWindowsOSUtil::ShowMessage(DStringView Message, DStringView Title)
{
	MessageBox(
		NULL,
		DStrings::DStringToSTDWideStr(Message).c_str(),
		DStrings::DStringToSTDWideStr(Title).c_str(),
		0
	);
}
