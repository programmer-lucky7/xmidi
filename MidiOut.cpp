#include "MidiOut.h"

MidiOut::MidiOut(const char *name) {
	snd_seq_open(&hmo, "default", SND_SEQ_OPEN_OUTPUT, 0);
	snd_seq_set_client_name(hmo, name);
}

MidiOut::~MidiOut() {
	if (fOpened) {
		Close();
	}
}

int MidiOut::Open() {
	snd_seq_port_subscribe_t *subs;
	snd_seq_addr_t sender, dest;

	nPort = snd_seq_create_simple_port(hmo, "midi:out",
		SND_SEQ_PORT_CAP_READ | SND_SEQ_PORT_CAP_SUBS_READ,
		SND_SEQ_PORT_TYPE_APPLICATION);

	sender.client = 129;
	sender.port = nPort;
	dest.client = 128;
	dest.port = 0;
	snd_seq_port_subscribe_alloca(&subs);
	snd_seq_port_subscribe_set_sender(subs, &sender);
	snd_seq_port_subscribe_set_dest(subs, &dest);
	snd_seq_subscribe_port(hmo, subs);

	snd_seq_ev_clear(&event);
	snd_seq_ev_set_source(&event, nPort);
	snd_seq_ev_set_subs(&event);
	snd_seq_ev_set_direct(&event);
	event.type = SND_SEQ_EVENT_OSS;

	return nPort;
}

int MidiOut::Close() {
	int r = -1;
	if (fOpened) {
		r = snd_seq_close(hmo);
		fOpened = 0;
	}
	return r;
}

int MidiOut::ShortMsg(unsigned long msg) {
	event.type = SND_SEQ_EVENT_OSS;
	event.data.raw32.d[0] = msg;
	printf("ShortMsg: 0x%08X\n", msg);
	snd_seq_event_output(hmo, &event);
	snd_seq_drain_output(hmo);
}

int MidiOut::NoteOn(unsigned char channel, unsigned char note, unsigned char velocity) {
	puts("MidiOut::NoteOn()");
	event.type = SND_SEQ_EVENT_NOTEON;
	event.data.note.channel = channel;
	event.data.note.note = note;
	event.data.note.velocity = velocity;
	snd_seq_event_output_direct(hmo, &event);
}

int MidiOut::NoteOff(unsigned char channel, unsigned char note, unsigned char velocity) {
	puts("MidiOut::NoteOff()");
	event.type = SND_SEQ_EVENT_NOTEOFF;
	event.data.note.channel = channel;
	event.data.note.note = note;
	event.data.note.velocity = velocity;
	snd_seq_event_output_direct(hmo, &event);
}

int MidiOut::SetPatch(unsigned char channel, unsigned char patch) {
	event.type = SND_SEQ_EVENT_PGMCHANGE;
	event.data.control.channel = channel;
	event.data.control.param = 0; //patch;
	event.data.control.value = patch;
	snd_seq_event_output_direct(hmo, &event);
}

int MidiOut::SustainPress(unsigned char channel) {
	event.type = SND_SEQ_EVENT_CONTROLLER;
	event.data.control.channel = channel;
	event.data.control.param = 0x40;
	event.data.control.value = 0x7F;
	snd_seq_event_output_direct(hmo, &event);
}

int MidiOut::SustainRelease(unsigned char channel) {
	event.type = SND_SEQ_EVENT_CONTROLLER;
	event.data.control.channel = channel;
	event.data.control.param = 0x40;
	event.data.control.value = 0x00;
	snd_seq_event_output_direct(hmo, &event);
}

int MidiOut::Reset() {
	puts("MidiOut::Reset()");
	event.type = SND_SEQ_EVENT_RESET;
	snd_seq_event_output_direct(hmo, &event);
}

int MidiOut::AllNoteOff(unsigned char channel) {
	puts("MidiOut::AllNoteOff()");
	event.type = SND_SEQ_EVENT_CONTROLLER;
	event.data.control.channel = channel;
	event.data.control.param = 0x7B;
	event.data.control.value = 0x00;
	snd_seq_event_output_direct(hmo, &event);
}

int MidiOut::SetPitch(unsigned char channel, unsigned char pitch) {
	event.type = SND_SEQ_EVENT_PITCHBEND;
	event.data.control.channel = channel;
	event.data.control.param = 0x00;
	event.data.control.value = 0;
	snd_seq_event_output_direct(hmo, &event);
}

int MidiOut::SetExpression(unsigned char channel, unsigned char expression) {
	event.type = SND_SEQ_EVENT_CONTROLLER;
	event.data.control.channel = channel;
	event.data.control.param = 0x0B;
	event.data.control.value = expression;
	snd_seq_event_output_direct(hmo, &event);
}


