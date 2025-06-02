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
}