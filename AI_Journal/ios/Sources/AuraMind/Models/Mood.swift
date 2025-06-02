import Foundation

public struct Mood {
    public let id: UUID
    public let primaryMood: String
    public let secondaryMoods: [String]?
    public let intensity: Double
    public let timestamp: Date
    
    public init(primaryMood: String, intensity: Double, timestamp: Date) {
        self.id = UUID()
        self.primaryMood = primaryMood
        self.secondaryMoods = nil
        self.intensity = intensity
        self.timestamp = timestamp
    }
    
    public init(primaryMood: String, secondaryMoods: [String], intensity: Double, timestamp: Date) {
        self.id = UUID()
        self.primaryMood = primaryMood
        self.secondaryMoods = secondaryMoods
        self.intensity = intensity
        self.timestamp = timestamp
    }
}