import XCTest
@testable import AuraMind

final class JournalEntryTests: XCTestCase {

    func testJournalEntryCreation() throws {
        let content = "Today was a great day!"
        let timestamp = Date()
        
        let entry = JournalEntry(
            content: content,
            timestamp: timestamp
        )
        
        XCTAssertEqual(entry.content, content)
        XCTAssertEqual(entry.timestamp, timestamp)
        XCTAssertNotNil(entry.id)
        XCTAssertFalse(entry.id.uuidString.isEmpty)
    }
}