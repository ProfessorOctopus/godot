//========= /*This file is part of : Godot Engine(see LICENSE.txt)*/ ============//
#import "midi_driver_coremidi.h"
#ifdef COREMIDI_ENABLED
#include "core/string/print_string.h"
#import <CoreAudio/HostTime.h>
#import <CoreServices/CoreServices.h>

Mutex MIDIDriverCoreMidi::mutex;
bool MIDIDriverCoreMidi::core_midi_closed = false;

MIDIDriverCoreMidi::InputConnection::InputConnection(int p_device_index, MIDIEndpointRef p_source) :
		parser(p_device_index), source(p_source) {}

void MIDIDriverCoreMidi::read(const MIDIPacketList *packet_list, void *read_proc_ref_con, void *src_conn_ref_con) {
	MutexLock lock(mutex);
	if (!core_midi_closed) {
		InputConnection *source = static_cast<InputConnection *>(src_conn_ref_con);
		const MIDIPacket *packet = packet_list->packet;
		for (UInt32 packet_index = 0; packet_index < packet_list->numPackets; packet_index++) {
			for (UInt16 data_index = 0; data_index < packet->length; data_index++) {
				source->parser.parse_fragment(packet->data[data_index]);
			}
			packet = MIDIPacketNext(packet);
		}
	}
}

Error MIDIDriverCoreMidi::open() {
	ERR_FAIL_COND_V_MSG(client || core_midi_closed, FAILED,
			"MIDIDriverCoreMidi cannot be reopened.");

	CFStringRef name = CFStringCreateWithCString(nullptr, "Godot", kCFStringEncodingASCII);
	OSStatus result = MIDIClientCreate(name, nullptr, nullptr, &client);
	CFRelease(name);
	if (result != noErr) {
		ERR_PRINT("MIDIClientCreate failed, code: " + itos(result));
		return ERR_CANT_OPEN;
	}

	result = MIDIInputPortCreate(client, CFSTR("Godot Input"), MIDIDriverCoreMidi::read, (void *)this, &port_in);
	if (result != noErr) {
		ERR_PRINT("MIDIInputPortCreate failed, code: " + itos(result));
		return ERR_CANT_OPEN;
	}

	int source_count = MIDIGetNumberOfSources();
	int connection_index = 0;
	for (int i = 0; i < source_count; i++) {
		MIDIEndpointRef source = MIDIGetSource(i);
		if (source) {
			InputConnection *conn = memnew(InputConnection(connection_index, source));
			const OSStatus res = MIDIPortConnectSource(port_in, source, static_cast<void *>(conn));
			if (res != noErr) {
				memdelete(conn);
			} else {
				connected_sources.push_back(conn);

				CFStringRef nameRef = nullptr;
				char name[256];
				MIDIObjectGetStringProperty(source, kMIDIPropertyDisplayName, &nameRef);
				CFStringGetCString(nameRef, name, sizeof(name), kCFStringEncodingUTF8);
				CFRelease(nameRef);
				connected_input_names.push_back(name);

				connection_index++; // Contiguous index for successfully connected inputs.
			}
		}
	}
	return OK;
}

void MIDIDriverCoreMidi::close() {
	mutex.lock();
	core_midi_closed = true;
	mutex.unlock();

	for (InputConnection *conn : connected_sources) {
		MIDIPortDisconnectSource(port_in, conn->source);
		memdelete(conn);
	}

	connected_sources.clear();
	connected_input_names.clear();

	if (port_in != 0) {
		MIDIPortDispose(port_in);
		port_in = 0;
	}

	if (client != 0) {
		MIDIClientDispose(client);
		client = 0;
	}
}

MIDIDriverCoreMidi::~MIDIDriverCoreMidi() {
	close();
}

#endif // COREMIDI_ENABLED
