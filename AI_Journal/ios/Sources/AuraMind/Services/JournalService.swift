import Foundation

public class JournalService {
    
    public init() {}
    
    public func createEntry(content: String, title: String?) throws -> JournalEntry {
        return JournalEntry(
            content: content,
            title: title,
            timestamp: Date()
        )
    }
}