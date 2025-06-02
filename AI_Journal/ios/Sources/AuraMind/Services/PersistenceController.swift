import CoreData
import Foundation

public class PersistenceController {
    public static let shared = PersistenceController()
    
    public let container: NSPersistentContainer
    
    init() {
        let model = NSManagedObjectModel()
        container = NSPersistentContainer(name: "AuraMind", managedObjectModel: model)
        
        configureStoreDescription()
        
        container.loadPersistentStores { _, error in
            if let error = error {
                fatalError("Failed to load Core Data store: \(error)")
            }
        }
    }
    
    private func configureStoreDescription() {
        guard let storeDescription = container.persistentStoreDescriptions.first else {
            fatalError("No persistent store description found")
        }
        
        #if os(iOS)
        storeDescription.setOption(FileProtectionType.complete as NSObject, 
                                  forKey: NSPersistentStoreFileProtectionKey)
        #endif
    }
}