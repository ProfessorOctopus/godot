//========= /*This file is part of : Godot Engine(see LICENSE.txt)*/ ============//
#ifdef WINMIDI_ENABLED

#include "midi_driver_winmidi.h"
#include "core/string/print_string.h"

void MIDIDriverWinMidi::read(HMIDIIN hMidiIn, UINT wMsg, DWORD_PTR dwInstance, DWORD_PTR dwParam1, DWORD_PTR dwParam2) {
	if (wMsg == MIM_DATA) {
		// For MIM_DATA: dwParam1 = wMidiMessage, dwParam2 = dwTimestamp.
		// Windows implementation has already unpacked running status and dropped any SysEx,
		// so we can just forward straight to the event.
		const uint8_t *midi_msg = (uint8_t *)&dwParam1;
		send_event((int)dwInstance, midi_msg[0], &midi_msg[1], 2);
	}
}

Error MIDIDriverWinMidi::open() {
	int device_index = 0;
	for (UINT i = 0; i < midiInGetNumDevs(); i++) {
		HMIDIIN midi_in;
		MIDIINCAPS caps;

		MMRESULT open_res = midiInOpen(&midi_in, i, (DWORD_PTR)read,
				(DWORD_PTR)device_index, CALLBACK_FUNCTION);
		MMRESULT caps_res = midiInGetDevCaps(i, &caps, sizeof(MIDIINCAPS));

		if (open_res == MMSYSERR_NOERROR) {
			midiInStart(midi_in);
			connected_sources.push_back(midi_in);
			if (caps_res == MMSYSERR_NOERROR) {
				connected_input_names.push_back(caps.szPname);
			} else {
				// Should push something even if we don't get a name,
				// so that the IDs line up correctly on the script side.
				connected_input_names.push_back("ERROR");
			}
			// Only increment device index for successfully connected devices.
			device_index++;
		} else {
			char err[256];
			midiInGetErrorText(open_res, err, 256);
			ERR_PRINT("midiInOpen error: " + String(err));

			if (caps_res == MMSYSERR_NOERROR) {
				ERR_PRINT("Can't open MIDI device \"" + String(caps.szPname) + "\", is it being used by another application?");
			}
		}
	}
	return OK;
}

void MIDIDriverWinMidi::close() {
	for (int i = 0; i < connected_sources.size(); i++) {
		HMIDIIN midi_in = connected_sources[i];
		midiInStop(midi_in);
		midiInClose(midi_in);
	}
	connected_sources.clear();
	connected_input_names.clear();
}

MIDIDriverWinMidi::~MIDIDriverWinMidi() {
	close();
}

#endif // WINMIDI_ENABLED
