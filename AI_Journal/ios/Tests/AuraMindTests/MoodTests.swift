import XCTest
@testable import AuraMind

final class MoodTests: XCTestCase {

    func testMoodCreation() throws {
        let primaryMood = "happy"
        let intensity = 0.8
        let timestamp = Date()
        
        let mood = Mood(
            primaryMood: primaryMood,
            intensity: intensity,
            timestamp: timestamp
        )
        
        XCTAssertEqual(mood.primaryMood, primaryMood)
        XCTAssertEqual(mood.intensity, intensity)
        XCTAssertEqual(mood.timestamp, timestamp)
        XCTAssertNotNil(mood.id)
    }
    
    func testMoodWithSecondaryMoods() throws {
        let primaryMood = "excited"
        let secondaryMoods = ["grateful", "optimistic"]
        let intensity = 0.9
        let timestamp = Date()
        
        let mood = Mood(
            primaryMood: primaryMood,
            secondaryMoods: secondaryMoods,
            intensity: intensity,
            timestamp: timestamp
        )
        
        XCTAssertEqual(mood.primaryMood, primaryMood)
        XCTAssertEqual(mood.secondaryMoods?.count, 2)
        XCTAssertEqual(mood.secondaryMoods?[0], "grateful")
        XCTAssertEqual(mood.secondaryMoods?[1], "optimistic")
        XCTAssertEqual(mood.intensity, intensity)
        XCTAssertEqual(mood.timestamp, timestamp)
        XCTAssertNotNil(mood.id)
    }
}