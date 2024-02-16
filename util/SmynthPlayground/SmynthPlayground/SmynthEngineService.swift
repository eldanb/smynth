//
//  SmynthEngineService.swift
//  SmynthPlayground
//
//  Created by Eldan Ben-Haim on 29/07/2023.
//


import Foundation
import AVFAudio

class SmynthEngineService: ObservableObject {
    init() {
        audioEngine = AVAudioEngine();
        inputSourceNode = AVAudioSourceNode(
            format: AVAudioFormat(
                commonFormat: AVAudioCommonFormat.pcmFormatInt16,
                sampleRate: 44100,
                interleaved: true,
                channelLayout: AVAudioChannelLayout(layoutTag: kAudioChannelLayoutTag_Stereo)!),
                
            renderBlock: { silent, timestamp, frameCount, bufferList in
                    silent.pointee = false
                NSLog("ts=%f, frameCound=%d", timestamp.pointee.mSampleTime, frameCount);
                self.renderBlock(Int(frameCount), timestamp.pointee, UnsafeMutableAudioBufferListPointer(bufferList))
                    return 0;
                });

        audioEngine.attach(inputSourceNode!);
        audioEngine.connect(inputSourceNode!, to: audioEngine.outputNode, format: nil);
    }
    
    func renderBlock(_ frameCount: Int,
                     _ timeStamp: AudioTimeStamp,
                     _ bufferList: UnsafeMutableAudioBufferListPointer) {
            
        let sampleBuffer = bufferList[0].mData?.bindMemory(to: Int16.self, capacity: frameCount);
        
        
        
        fillSampleBuffer(sampleBuffer,
                             2,
                             Int32(frameCount));
        
        
        /*
        fillSampleBuffer(<#T##buffers: UnsafeMutablePointer<UnsafeMutablePointer<Int32>?>!##UnsafeMutablePointer<UnsafeMutablePointer<Int32>?>!#>, <#T##numBuffers: Int32##Int32#>, <#T##numFrames: Int32##Int32#>)
        for buffer in bufferList {
            let typedBuffer = buffer.mData!.bindMemory(to: Int32.self, capacity: frameCount);
            
            for sample in 0..<frameCount {
                let sampleValue: Int32 = Int32(cos(
                    (Double(samplesample+Int64(sample)) / 44100) * 6.28 * 220) * 32768*2000);
                
                typedBuffer[sample] = sampleValue;
                
            }
        }*/
        
        samplesample += Int64(frameCount);
    }
    
    func play() throws {
        try audioEngine.start();
    }
    
    func stop() {
        audioEngine.stop();
    }
    
    let audioEngine: AVAudioEngine
    var inputSourceNode: AVAudioSourceNode?;
}


var samplesample: Int64 = 0;
