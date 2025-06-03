import Foundation
import CoreData

public class CoreDataJournalRepository {
    private let context: NSManagedObjectContext
    
    public init(context: NSManagedObjectContext) {
        self.context = context
    }
    
    public func create(_ entry: JournalEntry) throws -> JournalEntry {
        // Create Core Data managed object
        let entityDescription = NSEntityDescription.entity(forEntityName: "JournalEntryEntity", in: context)!
        let managedObject = NSManagedObject(entity: entityDescription, insertInto: context)
        
        // Map domain model to Core Data
        managedObject.setValue(entry.id, forKey: "id")
        managedObject.setValue(entry.content, forKey: "content")
        managedObject.setValue(entry.title, forKey: "title")
        managedObject.setValue(entry.timestamp, forKey: "timestamp")
        
        // Save to Core Data
        try context.save()
        
        return entry
    }
    
    public func fetchAll() throws -> [JournalEntry] {
        let request = NSFetchRequest<NSManagedObject>(entityName: "JournalEntryEntity")
        let results = try context.fetch(request)
        
        return results.map { managedObject in
            JournalEntry(
                id: managedObject.value(forKey: "id") as! UUID,
                content: managedObject.value(forKey: "content") as! String,
                title: managedObject.value(forKey: "title") as? String,
                timestamp: managedObject.value(forKey: "timestamp") as! Date
            )
        }
    }
}