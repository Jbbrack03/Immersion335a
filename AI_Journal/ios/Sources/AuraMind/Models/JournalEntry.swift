import Foundation

public struct JournalEntry {
    public let id: UUID
    public let content: String
    public let title: String?
    public let timestamp: Date
    public let sentimentScore: Double?
    public let sentimentLabel: String?
    public let themes: [Theme]?
    public let mood: Mood?
    
    public init(content: String, title: String? = nil, timestamp: Date) {
        self.id = UUID()
        self.content = content
        self.title = title
        self.timestamp = timestamp
        self.sentimentScore = nil
        self.sentimentLabel = nil
        self.themes = nil
        self.mood = nil
    }
    
    public init(content: String, title: String? = nil, timestamp: Date, sentimentScore: Double, sentimentLabel: String) {
        self.id = UUID()
        self.content = content
        self.title = title
        self.timestamp = timestamp
        self.sentimentScore = sentimentScore
        self.sentimentLabel = sentimentLabel
        self.themes = nil
        self.mood = nil
    }
    
    public init(content: String, title: String? = nil, timestamp: Date, themes: [Theme]) {
        self.id = UUID()
        self.content = content
        self.title = title
        self.timestamp = timestamp
        self.sentimentScore = nil
        self.sentimentLabel = nil
        self.themes = themes
        self.mood = nil
    }
    
    public init(content: String, title: String? = nil, timestamp: Date, mood: Mood) {
        self.id = UUID()
        self.content = content
        self.title = title
        self.timestamp = timestamp
        self.sentimentScore = nil
        self.sentimentLabel = nil
        self.themes = nil
        self.mood = mood
    }
}