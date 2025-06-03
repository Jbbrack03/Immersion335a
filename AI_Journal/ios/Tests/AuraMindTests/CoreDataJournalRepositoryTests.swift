import XCTest
import CoreData
@testable import AuraMind

final class CoreDataJournalRepositoryTests: XCTestCase {

    func testCreateJournalEntry() throws {
        let repository = CoreDataJournalRepository(context: PersistenceController.shared.container.viewContext)
        let content = "Today was a great day for testing Core Data persistence."
        let title = "Core Data Test"
        let timestamp = Date()
        
        let entry = JournalEntry(content: content, title: title, timestamp: timestamp)
        let savedEntry = try repository.create(entry)
        
        XCTAssertEqual(savedEntry.content, content)
        XCTAssertEqual(savedEntry.title, title)
        XCTAssertEqual(savedEntry.id, entry.id)
        XCTAssertEqual(savedEntry.timestamp, timestamp)
    }
    
    func testCreateJournalEntryPersistsToCoreData() throws {
        let context = PersistenceController.shared.container.viewContext
        let repository = CoreDataJournalRepository(context: context)
        let content = "This should persist to Core Data"
        let title = "Core Data Persistence Test"
        let timestamp = Date()
        
        let entry = JournalEntry(content: content, title: title, timestamp: timestamp)
        let savedEntry = try repository.create(entry)
        
        // Force Core Data to verify actual persistence
        let request = NSFetchRequest<NSManagedObject>(entityName: "JournalEntryEntity")
        request.predicate = NSPredicate(format: "id == %@", savedEntry.id as CVarArg)
        
        let results = try context.fetch(request)
        XCTAssertEqual(results.count, 1, "Entry should be persisted to Core Data")
        
        let managedObject = results.first!
        XCTAssertEqual(managedObject.value(forKey: "content") as? String, content)
        XCTAssertEqual(managedObject.value(forKey: "title") as? String, title)
        XCTAssertEqual(managedObject.value(forKey: "id") as? UUID, savedEntry.id)
    }
}