import Foundation

public struct JournalEntry {
    public let id: UUID
    public let content: String
    public let timestamp: Date
    public let sentimentScore: Double?
    public let sentimentLabel: String?
    public let themes: [Theme]?
    
    public init(content: String, timestamp: Date) {
        self.id = UUID()
        self.content = content
        self.timestamp = timestamp
        self.sentimentScore = nil
        self.sentimentLabel = nil
        self.themes = nil
    }
    
    public init(content: String, timestamp: Date, sentimentScore: Double, sentimentLabel: String) {
        self.id = UUID()
        self.content = content
        self.timestamp = timestamp
        self.sentimentScore = sentimentScore
        self.sentimentLabel = sentimentLabel
        self.themes = nil
    }
    
    public init(content: String, timestamp: Date, themes: [Theme]) {
        self.id = UUID()
        self.content = content
        self.timestamp = timestamp
        self.sentimentScore = nil
        self.sentimentLabel = nil
        self.themes = themes
    }
}