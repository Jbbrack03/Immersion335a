import Foundation
import CoreData

public class CoreDataJournalRepository {
    private let context: NSManagedObjectContext
    
    public init(context: NSManagedObjectContext) {
        self.context = context
    }
    
    public func create(_ entry: JournalEntry) throws -> JournalEntry {
        return entry
    }
}