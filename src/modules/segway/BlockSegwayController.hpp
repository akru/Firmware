#pragma once

#include <px4_posix.h>
#include <controllib/uorb/blocks.hpp>
#include <commander/commander_helper.h>

using namespace control;

class BlockSegwayController : public control::BlockUorbEnabledAutopilot
{
public:
	BlockSegwayController() :
		BlockUorbEnabledAutopilot(NULL, "SEG"),
		th2v(this, "TH2V"),
		q2v(this, "Q2V"),
		_attPoll(),
		_timeStamp(0)
	{
		_attPoll.fd = _att.getHandle();
		_attPoll.events = POLLIN;

        led_init();
        rgbled_set_mode(RGBLED_MODE_ON);
	}
	void update();
private:
	BlockPI th2v;
	BlockP q2v;
	px4_pollfd_struct_t _attPoll;
	uint64_t _timeStamp;
};

