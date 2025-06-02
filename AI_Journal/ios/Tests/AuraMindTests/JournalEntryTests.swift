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
    
    func testJournalEntryWithSentimentAnalysis() throws {
        let content = "I feel really happy today!"
        let timestamp = Date()
        let sentimentScore = 0.8
        let sentimentLabel = "positive"
        
        let entry = JournalEntry(
            content: content,
            timestamp: timestamp,
            sentimentScore: sentimentScore,
            sentimentLabel: sentimentLabel
        )
        
        XCTAssertEqual(entry.content, content)
        XCTAssertEqual(entry.timestamp, timestamp)
        XCTAssertEqual(entry.sentimentScore, sentimentScore)
        XCTAssertEqual(entry.sentimentLabel, sentimentLabel)
        XCTAssertNotNil(entry.id)
    }
}