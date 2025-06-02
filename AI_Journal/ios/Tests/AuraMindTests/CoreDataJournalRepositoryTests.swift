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
    
    func testFetchAllEntriesAfterCreate() throws {
        let repository = CoreDataJournalRepository(context: PersistenceController.shared.container.viewContext)
        let content = "Persistent entry content"
        let title = "Persistent Entry"
        let timestamp = Date()
        
        let entry = JournalEntry(content: content, title: title, timestamp: timestamp)
        try repository.create(entry)
        
        let fetchedEntries = try repository.fetchAll()
        XCTAssertEqual(fetchedEntries.count, 1)
        XCTAssertEqual(fetchedEntries.first?.content, content)
        XCTAssertEqual(fetchedEntries.first?.title, title)
        XCTAssertEqual(fetchedEntries.first?.id, entry.id)
    }
}