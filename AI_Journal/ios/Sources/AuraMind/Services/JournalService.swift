import Foundation

public class JournalService {
    private var entries: [UUID: JournalEntry] = [:]
    
    public init() {}
    
    public func createEntry(content: String, title: String?) throws -> JournalEntry {
        let entry = JournalEntry(
            content: content,
            title: title,
            timestamp: Date()
        )
        entries[entry.id] = entry
        return entry
    }
    
    public func updateEntry(id: UUID, content: String, title: String?) throws -> JournalEntry {
        guard entries[id] != nil else {
            throw JournalServiceError.entryNotFound
        }
        
        let updatedEntry = JournalEntry(
            id: id,
            content: content,
            title: title,
            timestamp: Date()
        )
        entries[id] = updatedEntry
        return updatedEntry
    }
    
    public func deleteEntry(id: UUID) throws {
        guard entries[id] != nil else {
            throw JournalServiceError.entryNotFound
        }
        
        entries.removeValue(forKey: id)
    }
    
    public func fetchEntries(limit: Int?) throws -> [JournalEntry] {
        let allEntries = Array(entries.values)
        
        if let limit = limit {
            return Array(allEntries.prefix(limit))
        }
        
        return allEntries
    }
    
    public func searchEntries(query: String) throws -> [JournalEntry] {
        let lowercaseQuery = query.lowercased()
        
        return entries.values.filter { entry in
            let contentMatch = entry.content.lowercased().contains(lowercaseQuery)
            let titleMatch = entry.title?.lowercased().contains(lowercaseQuery) ?? false
            return contentMatch || titleMatch
        }
    }
}

public enum JournalServiceError: Error, Equatable {
    case entryNotFound
}