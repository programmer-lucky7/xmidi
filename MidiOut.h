#ifndef __MIDIOUT_H_
#define __MIDIOUT_H_

#include <alsa/asoundlib.h>

class MidiOut {
	snd_seq_t *hmo;
	int nPort;
	int fOpened;
	snd_seq_event_t event;
public:
	MidiOut(const char *name);
	~MidiOut();
	int Open();
	int Close();
	int Reset();
	int ShortMsg(unsigned long msg);
	int NoteOn(unsigned char channel, unsigned char note, unsigned char velocity);
	int NoteOff(unsigned char channel, unsigned char note, unsigned char velocity);
	int SetPatch(unsigned char channel, unsigned char patch);
	int SustainPress(unsigned char channel);
	int SustainRelease(unsigned char channel);
	int AllNoteOff(unsigned char channel);
	int SetPitch(unsigned char channel, unsigned char pitch);
	int SetExpression(unsigned char channel, unsigned char expression);
};

#endif
