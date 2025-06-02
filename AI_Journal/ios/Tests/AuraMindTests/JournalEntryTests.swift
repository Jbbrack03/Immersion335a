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
    
    func testJournalEntryWithThemes() throws {
        let content = "I'm grateful for my family and feeling blessed today."
        let timestamp = Date()
        let gratitudeTheme = Theme(name: "gratitude", confidence: 0.9)
        let familyTheme = Theme(name: "family", confidence: 0.8)
        let themes = [gratitudeTheme, familyTheme]
        
        let entry = JournalEntry(
            content: content,
            timestamp: timestamp,
            themes: themes
        )
        
        XCTAssertEqual(entry.content, content)
        XCTAssertEqual(entry.timestamp, timestamp)
        XCTAssertEqual(entry.themes?.count, 2)
        XCTAssertEqual(entry.themes?[0].name, "gratitude")
        XCTAssertEqual(entry.themes?[1].name, "family")
        XCTAssertNotNil(entry.id)
    }
}