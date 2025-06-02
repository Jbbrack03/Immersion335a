import XCTest
import CoreData
@testable import AuraMind

final class PersistenceControllerTests: XCTestCase {

    func testPersistenceControllerInitialization() throws {
        let controller = PersistenceController.shared
        
        XCTAssertNotNil(controller.container)
        XCTAssertEqual(controller.container.name, "AuraMind")
        XCTAssertTrue(controller.container.persistentStoreDescriptions.count > 0)
        
        #if os(iOS)
        let storeDescription = controller.container.persistentStoreDescriptions.first!
        let fileProtection = storeDescription.options?[NSPersistentStoreFileProtectionKey] as? FileProtectionType
        XCTAssertEqual(fileProtection, FileProtectionType.complete)
        #endif
    }
}