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
}