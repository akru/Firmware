#include "BlockSegwayController.hpp"

void BlockSegwayController::update()
{
	// wait for a sensor update, check for exit condition every 100 ms
	if (px4_poll(&_attPoll, 1, 100) < 0) { return; } // poll error
    
	uint64_t newTimeStamp = hrt_absolute_time();
	float dt = (newTimeStamp - _timeStamp) / 1.0e6f;
	_timeStamp = newTimeStamp;

	// check for sane values of dt
	// to prevent large control responses
	if (dt > 1.0f || dt < 0) { return; }

	// set dt for all child blocks
	setDt(dt);

	// check for new updates
	if (_param_update.updated()) { updateParams(); }

	// get new information from subscriptions
    updateSubscriptions();

    if (fabs(_att.get().pitch) < 0.1) {
        rgbled_set_color(RGBLED_COLOR_GREEN);
    } else if (_att.get().pitch > 0) {
        rgbled_set_color(RGBLED_COLOR_BLUE);
    } else {
        rgbled_set_color(RGBLED_COLOR_PURPLE);
    }

	// compute speed command
	float spdCmd = - th2v.update(_att.get().pitch)
                   - q2v.update(_att.get().pitchspeed);
	_actuators.get().control[0] = spdCmd;
	_actuators.get().control[1] = -spdCmd;
	// update all publications
	updatePublications();
}
