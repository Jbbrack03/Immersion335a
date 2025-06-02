import Foundation

public struct JournalEntry {
    public let id: UUID
    public let content: String
    public let timestamp: Date
    
    public init(content: String, timestamp: Date) {
        self.id = UUID()
        self.content = content
        self.timestamp = timestamp
    }
}