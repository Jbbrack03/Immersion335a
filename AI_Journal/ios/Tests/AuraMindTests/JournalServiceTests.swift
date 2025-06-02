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
    
    func testDeleteEntry() throws {
        let service = JournalService()
        let content = "This entry will be deleted."
        let title = "Temporary Entry"
        
        let entry = try service.createEntry(content: content, title: title)
        
        try service.deleteEntry(id: entry.id)
        
        XCTAssertThrowsError(try service.updateEntry(id: entry.id, content: "Updated", title: "Updated")) { error in
            XCTAssertTrue(error is JournalServiceError)
            XCTAssertEqual(error as? JournalServiceError, .entryNotFound)
        }
    }
    
    func testFetchEntries() throws {
        let service = JournalService()
        
        let entry1 = try service.createEntry(content: "First entry", title: "Entry 1")
        let entry2 = try service.createEntry(content: "Second entry", title: "Entry 2")
        let entry3 = try service.createEntry(content: "Third entry", title: "Entry 3")
        
        let entries = try service.fetchEntries(limit: nil)
        
        XCTAssertEqual(entries.count, 3)
        XCTAssertTrue(entries.contains { $0.id == entry1.id })
        XCTAssertTrue(entries.contains { $0.id == entry2.id })
        XCTAssertTrue(entries.contains { $0.id == entry3.id })
        
        let limitedEntries = try service.fetchEntries(limit: 2)
        XCTAssertEqual(limitedEntries.count, 2)
    }
    
    func testSearchEntries() throws {
        let service = JournalService()
        
        let entry1 = try service.createEntry(content: "Today I went hiking in the mountains", title: "Mountain Adventure")
        let entry2 = try service.createEntry(content: "Visited the local coffee shop", title: "Coffee Break")
        let entry3 = try service.createEntry(content: "Working on a challenging project", title: "Project Updates")
        
        let mountainResults = try service.searchEntries(query: "mountain")
        XCTAssertEqual(mountainResults.count, 1)
        XCTAssertEqual(mountainResults.first?.id, entry1.id)
        
        let coffeeResults = try service.searchEntries(query: "coffee")
        XCTAssertEqual(coffeeResults.count, 1)
        XCTAssertEqual(coffeeResults.first?.id, entry2.id)
        
        let noResults = try service.searchEntries(query: "unicorn")
        XCTAssertEqual(noResults.count, 0)
        
        let caseInsensitiveResults = try service.searchEntries(query: "COFFEE")
        XCTAssertEqual(caseInsensitiveResults.count, 1)
        XCTAssertEqual(caseInsensitiveResults.first?.id, entry2.id)
    }
}