#include "Engine\pch.h"
#include "SNativeMessage.h"

#include "OS\SWindowsOSUtil.h"


void SNativeMessage::ShowMessage(DStringView Message, DStringView Title)
{
	SWindowsOSUtil::ShowMessage(Message, Title);
}
