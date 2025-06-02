import Foundation
import CoreData

public class CoreDataJournalRepository {
    private let context: NSManagedObjectContext
    private var entries: [JournalEntry] = []
    
    public init(context: NSManagedObjectContext) {
        self.context = context
    }
    
    public func create(_ entry: JournalEntry) throws -> JournalEntry {
        entries.append(entry)
        return entry
    }
    
    public func fetchAll() throws -> [JournalEntry] {
        return entries
    }
}