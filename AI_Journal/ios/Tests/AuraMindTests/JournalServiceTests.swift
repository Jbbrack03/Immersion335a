import XCTest
@testable import AuraMind

final class JournalServiceTests: XCTestCase {

    func testCreateEntry() throws {
        let service = JournalService()
        let content = "Today was a wonderful day exploring the city."
        let title = "City Adventure"
        
        let entry = try service.createEntry(content: content, title: title)
        
        XCTAssertEqual(entry.content, content)
        XCTAssertEqual(entry.title, title)
        XCTAssertNotNil(entry.id)
        XCTAssertFalse(entry.id.uuidString.isEmpty)
        XCTAssertNotNil(entry.timestamp)
    }
    
    func testUpdateEntry() throws {
        let service = JournalService()
        let originalContent = "First draft of my thoughts."
        let originalTitle = "Draft"
        
        let entry = try service.createEntry(content: originalContent, title: originalTitle)
        
        let updatedContent = "Updated and refined thoughts with more detail."
        let updatedTitle = "Final Version"
        
        let updatedEntry = try service.updateEntry(id: entry.id, content: updatedContent, title: updatedTitle)
        
        XCTAssertEqual(updatedEntry.id, entry.id)
        XCTAssertEqual(updatedEntry.content, updatedContent)
        XCTAssertEqual(updatedEntry.title, updatedTitle)
        XCTAssertNotNil(updatedEntry.timestamp)
    }
}