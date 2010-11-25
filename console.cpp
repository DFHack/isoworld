#pragma once

#include "allegro5/allegro_native_dialog.h"
#include "console.h"
#include <stdio.h>
#include <stdarg.h>

ALLEGRO_TEXTLOG *textlog = NULL;
void open_log(void)
{
	textlog = al_open_native_text_log("Log", 0);
}

void open_log_monospace(void)
{
	textlog = al_open_native_text_log("Log", ALLEGRO_TEXTLOG_MONOSPACE);
}

void close_log(bool wait_for_user)
{
	if (textlog && wait_for_user) {
		ALLEGRO_EVENT_QUEUE *queue = al_create_event_queue();
		al_register_event_source(queue, al_get_native_text_log_event_source(
			textlog));
		al_wait_for_event(queue, NULL);
		al_destroy_event_queue(queue);
	}

	al_close_native_text_log(textlog);
	textlog = NULL;
}

void log_printf(char const *format, ...)
{
	char str[1024];
	va_list args;
	va_start(args, format);
	vsnprintf(str, sizeof str, format, args);
	va_end(args);
	al_append_native_text_log(textlog, "%s", str);
}

void DisplayErr(const char *format, ...)
{
	ALLEGRO_USTR *buf;
	va_list ap;
	const char *s;

	/* Fast path for common case. */
	if (0 == strcmp(format, "%s")) {
		va_start(ap, format);
		s = va_arg(ap, const char *);
		al_show_native_message_box(al_get_current_display(), "Error", "ERROR", s, NULL, ALLEGRO_MESSAGEBOX_ERROR);
		va_end(ap);
		return;
	}

	va_start(ap, format);
	buf = al_ustr_new("");
	al_ustr_vappendf(buf, format, ap);
	va_end(ap);

	al_show_native_message_box(al_get_current_display(), "Error", "ERROR", al_cstr(buf), NULL, ALLEGRO_MESSAGEBOX_ERROR);

	al_ustr_free(buf);
}