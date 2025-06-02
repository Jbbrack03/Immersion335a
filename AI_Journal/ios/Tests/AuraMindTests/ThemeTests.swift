import XCTest
@testable import AuraMind

final class ThemeTests: XCTestCase {

    func testThemeCreation() throws {
        let name = "gratitude"
        let confidence = 0.85
        
        let theme = Theme(
            name: name,
            confidence: confidence
        )
        
        XCTAssertEqual(theme.name, name)
        XCTAssertEqual(theme.confidence, confidence)
        XCTAssertNotNil(theme.id)
    }
}