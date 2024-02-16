//
//  SmynthPlaygroundApp.swift
//  SmynthPlayground
//
//  Created by Eldan Ben-Haim on 29/07/2023.
//

import SwiftUI

@main
struct SmynthPlaygroundApp: App {
    
    @StateObject
    var smynthEngineService = SmynthEngineService()
    
    var body: some Scene {
        WindowGroup {
            ContentView().environmentObject(smynthEngineService)
        }
        
    }

}
