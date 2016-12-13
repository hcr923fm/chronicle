#include "chronicle.h"

using namespace std;

int main(){
    RtAudio audio;

    // How many devices are available?
    unsigned int devices = audio.getDeviceCount();

    // Tell us about the devices available
	cout << devices << " devices available" << endl;
    for (unsigned int i=0; i<devices; i+=1){
		RtAudio::DeviceInfo deviceInfo = audio.getDeviceInfo(i);
        cout << "Device #" << i << ": " << deviceInfo.name << endl;
        cout << "    Input channels: " << deviceInfo.inputChannels << endl;
        cout << "    Output channels: " << deviceInfo.outputChannels << endl;
        cout << "    Max duplex channels: " << deviceInfo.duplexChannels << endl;
		cout << "    Sample rates: ";

		for (unsigned int j = 0; j < deviceInfo.sampleRates.size(); j++) {
			cout << deviceInfo.sampleRates[j] << "; ";
		}
		cout << endl;

        cout << "    Preferred sample rate: " << deviceInfo.preferredSampleRate << endl;
        cout << "    Data formats: ";
        
        RtAudioFormat deviceAudioFormats = deviceInfo.nativeFormats;
        if (deviceAudioFormats & RTAUDIO_SINT8){
            cout << "Signed 8-bit int; ";
        }
        if (deviceAudioFormats & RTAUDIO_SINT16){
            cout << "Signed 16-bit int; ";
        }
        if (deviceAudioFormats & RTAUDIO_SINT24){
            cout << "Signed 24-bit int; ";
        }
        if (deviceAudioFormats & RTAUDIO_SINT32){
            cout << "Signed 32-bit int; ";
        }
        if (deviceAudioFormats & RTAUDIO_FLOAT32){
            cout << "32-bit float; ";
        }
        if (deviceAudioFormats & RTAUDIO_FLOAT64){
            cout << "64-bit double; ";
        }

        cout << endl;
    }

}