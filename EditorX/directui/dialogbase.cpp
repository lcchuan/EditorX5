#include "stdafx.h"
#include "dialogbase.h"

namespace lcc_direct_ui {
	DialogBase::DialogBase() : UIBase()
	{
	}

	DialogBase::~DialogBase() {
		UIBase::~UIBase();
	}
}