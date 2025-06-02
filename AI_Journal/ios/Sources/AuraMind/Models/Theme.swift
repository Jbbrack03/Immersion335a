import Foundation

public struct Theme {
    public let id: UUID
    public let name: String
    public let confidence: Double
    
    public init(name: String, confidence: Double) {
        self.id = UUID()
        self.name = name
        self.confidence = confidence
    }
}