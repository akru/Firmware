#include <nuttx/config.h>

extern "C" __EXPORT int main(int argc, const char *argv[]);

#include <fcntl.h>
#include <unistd.h>

#include <cstring>
#include <cstdio>

#include <drivers/drv_airleash_kbd.h>
#include <drivers/drv_hrt.h>
#include <systemlib/systemlib.h>

#include "kbd_reader.hpp"

namespace {

static bool daemon_should_run = false;
static bool daemon_running = false;

using KbdButtonState = ButtonState<
	pressed_mask_t, KBD_SCAN_BUFFER_N_ITEMS,
	hrt_abstime, KBD_SCAN_INTERVAL_usec
>;

void
update_buttons(KbdButtonState & s, hrt_abstime now, int f_kbd)
{
	pressed_mask_t masks[KBD_SCAN_BUFFER_N_ITEMS];
	read(f_kbd, masks, sizeof(masks));
	// It should always be full.
	s.update(now, masks);
}

static int
daemon(int argc, char *argv[])
{
	int f_kbd_masks = open(KBD_DEVICE_PATH, O_RDONLY);
	if (f_kbd_masks == -1) {
		perror("airleash open(" KBD_DEVICE_PATH ")");
		return 1;
	}

	KbdButtonState btn;

	daemon_running = true;
	fprintf(stderr, "%s has started.\n", argv[0]);

	while (daemon_should_run)
	{
		usleep(KBD_SCAN_INTERVAL_usec);
		hrt_abstime now = hrt_absolute_time();
		update_buttons(btn, now, f_kbd_masks);

		if (btn.actual_button)
		{
			if (btn.time_released)
				printf("%04x mask was pressed for %u usec.\n",
					btn.actual_button,
					btn.time_released - btn.time_pressed);
			else
				printf("%04x mask is pressed for %u usec.\n",
					btn.actual_button,
					now - btn.time_pressed);
		}
	}

	close(f_kbd_masks);
	daemon_running = false;

	fprintf(stderr, "%s has stopped.\n", argv[0]);
	return 0;
}

static inline bool
streq(const char a[], const char b[]) { return std::strcmp(a, b) == 0; }

static void
usage(const char name[])
{ std::fprintf(stderr, "Usage: %s start|stop|status\n\n", name); }

} // end of anonymous namespace

int
leash_main(int argc, const char *argv[])
{
	if (argc != 2)
	{
		usage(argv[0]);
		return 1;
	}

	if (streq(argv[1], "start"))
	{
		if (daemon_running)
		{
			fprintf(stderr, "%s is already running.\n", argv[0]);
			return 1;
		}
		daemon_should_run = true;
		task_spawn_cmd(argv[0],
				SCHED_DEFAULT,
				SCHED_PRIORITY_DEFAULT,
				CONFIG_TASK_SPAWN_DEFAULT_STACKSIZE,
				daemon,
				argv);
	}
	else if (streq(argv[1], "status"))
	{
		if (daemon_running) { printf("%s is running.\n", argv[0]); }
		else { printf("%s is NOT running.\n", argv[0]); }
	}
	else if (streq(argv[1], "stop"))
	{
		if (not daemon_running)
		{
			fprintf(stderr, "%s is NOT running.\n", argv[0]);
			return 1;
		}
		daemon_should_run = false;
	}
	else
	{
		usage(argv[0]);
		return 1;
	}
}
