//
//  ContentView.swift
//  SmynthPlayground
//
//  Created by Eldan Ben-Haim on 29/07/2023.
//

import SwiftUI

struct ContentView: View {
    @EnvironmentObject var smynthEngineService: SmynthEngineService;
    
    var body: some View {
        VStack {
            Button(action: {
                play();
            }) {
                Image(systemName: "play")
                    .imageScale(.large)
                    .foregroundColor(.accentColor)
                Text("Take it away boys!")
            }
            Button(action: {
                stop();
            }) {
                Image(systemName: "stop")
                    .imageScale(.large)
                    .foregroundColor(.accentColor)
                Text("That's enough!")
            }
        }
        .padding()
    }
    
    func play() {
        do {
            try smynthEngineService.play();
        } catch let error as NSException {
            NSLog("Error %@", error)
        }
        catch {
            NSLog("weird error")
        }
    }
    
    func stop() {
        smynthEngineService.stop();
    }
}

struct ContentView_Previews: PreviewProvider {
    static var previews: some View {
        ContentView()
    }
}
