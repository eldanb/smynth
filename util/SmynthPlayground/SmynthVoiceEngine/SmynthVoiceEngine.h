//
//  SmynthVoiceEngine.h
//  SmynthVoiceEngine
//
//  Created by Eldan Ben-Haim on 29/07/2023.
//

#import <Foundation/Foundation.h>


FOUNDATION_EXPORT double SmynthVoiceEngineVersionNumber;
FOUNDATION_EXPORT void restartNotes();
FOUNDATION_EXPORT void fillSampleBuffer(int16_t *buffer, int numChannels, int numFrames);
