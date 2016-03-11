/****************************************************************************
 *
 *   Copyright (c) 2012-2016 PX4 Development Team. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 * 3. Neither the name PX4 nor the names of its contributors may be
 *    used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 ****************************************************************************/

/**
 * @file mavlink_log.h
 * MAVLink text logging.
 *
 * @author Lorenz Meier <lorenz@px4.io>
 * @author Julian Oes <julian@oes.ch>
 */

#ifndef MAVLINK_LOG
#define MAVLINK_LOG

/**
 * The mavlink log device node; must be opened before messages
 * can be logged.
 */
#define MAVLINK_LOG_DEVICE			"/dev/mavlink"
/**
 * The maximum string length supported.
 */
#define MAVLINK_LOG_MAXLEN			50

#ifdef __cplusplus
extern "C" {
#endif
__EXPORT void mavlink_vasprintf(int severity, const char *fmt, ...);
#ifdef __cplusplus
}
#endif

/*
 * The va_args implementation here is not beautiful, but obviously we run into the same issues
 * the GCC devs saw, and are using their solution:
 *
 * http://gcc.gnu.org/onlinedocs/cpp/Variadic-Macros.html
 */

/**
 * Send a mavlink emergency message.
 *
 * @param _text		The text to log;
 */
#define mavlink_log_emergency(_text, ...)	mavlink_vasprintf(3, _text, ##__VA_ARGS__);

/**
 * Send a mavlink critical message.
 *
 * @param _text		The text to log;
 */
#define mavlink_log_critical(_text, ...)	mavlink_vasprintf(2, _text, ##__VA_ARGS__);

/**
 * Send a mavlink info message.
 *
 * @param _text		The text to log;
 */
#define mavlink_log_info(_text, ...)		mavlink_vasprintf(1, _text, ##__VA_ARGS__);

/**
 * Send a mavlink emergency message and print to console.
 *
 * @param _text		The text to log;
 */
#define mavlink_and_console_log_emergency(_text, ...) \
	do { \
		mavlink_log_emergency(_text, ##__VA_ARGS__); \
		PX4_ERR("telem> "); \
		PX4_ERR(_text, ##__VA_ARGS__); \
	} while(0);

/**
 * Send a mavlink critical message and print to console.
 *
 * @param _text		The text to log;
 */
#define mavlink_and_console_log_critical(_text, ...) \
	do { \
		mavlink_log_critical(_text, ##__VA_ARGS__); \
		PX4_WARN("telem> "); \
		PX4_WARN(_text, ##__VA_ARGS__); \
	} while(0);

/**
 * Send a mavlink emergency message and print to console.
 *
 * @param _text		The text to log;
 */
#define mavlink_and_console_log_info(_text, ...)			\
	do { \
		mavlink_log_info(_text, ##__VA_ARGS__); \
		PX4_INFO("telem> "); \
		PX4_INFO(_text, ##__VA_ARGS__); \
	} while(0);

struct mavlink_logmessage {
	char text[MAVLINK_LOG_MAXLEN + 1];
	unsigned char severity;
};

struct mavlink_logbuffer {
	unsigned int start;
	unsigned int size;
	int count;
	struct mavlink_logmessage *elems;
};

__BEGIN_DECLS
void mavlink_logbuffer_init(struct mavlink_logbuffer *lb, int size);

void mavlink_logbuffer_destroy(struct mavlink_logbuffer *lb);

int mavlink_logbuffer_is_full(struct mavlink_logbuffer *lb);

int mavlink_logbuffer_is_empty(struct mavlink_logbuffer *lb);

void mavlink_logbuffer_write(struct mavlink_logbuffer *lb, const struct mavlink_logmessage *elem);

int mavlink_logbuffer_read(struct mavlink_logbuffer *lb, struct mavlink_logmessage *elem);

void mavlink_logbuffer_vasprintf(struct mavlink_logbuffer *lb, int severity, const char *fmt, ...);

__END_DECLS

#endif
