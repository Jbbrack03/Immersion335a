import XCTest
@testable import AuraMind

final class AutosaveServiceTests: XCTestCase {
    func testAutosaveTimerCreatesEntries() throws {
        // Given
        let repository = MockJournalRepository()
        let journalService = JournalService(repository: repository)
        let autosaveService = AutosaveService(journalService: journalService)
        let expectation = XCTestExpectation(description: "Autosave should trigger")
        
        // When
        autosaveService.startAutosave(interval: 0.1) // 100ms for testing
        autosaveService.updateDraft(content: "Test autosave content", title: "Autosaved")
        
        // Wait for autosave to trigger
        DispatchQueue.main.asyncAfter(deadline: .now() + 0.2) {
            expectation.fulfill()
        }
        
        wait(for: [expectation], timeout: 1.0)
        
        // Then
        let entries = try journalService.fetchEntries(limit: nil)
        XCTAssertEqual(entries.count, 1)
        XCTAssertEqual(entries.first?.content, "Test autosave content")
        XCTAssertEqual(entries.first?.title, "Autosaved")
    }
}

// Mock repository for testing
private class MockJournalRepository: JournalRepository {
    private var entries: [UUID: JournalEntry] = [:]
    
    func create(_ entry: JournalEntry) throws -> JournalEntry {
        entries[entry.id] = entry
        return entry
    }
    
    func fetchAll() throws -> [JournalEntry] {
        return Array(entries.values)
    }
    
    func update(_ entry: JournalEntry) throws -> JournalEntry {
        entries[entry.id] = entry
        return entry
    }
    
    func delete(_ id: UUID) throws {
        entries.removeValue(forKey: id)
    }
    
    func search(_ query: String) throws -> [JournalEntry] {
        return []
    }
}