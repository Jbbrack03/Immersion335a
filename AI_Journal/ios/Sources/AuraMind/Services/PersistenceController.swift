import CoreData
import Foundation

public class PersistenceController {
    public static let shared = PersistenceController()
    
    public let container: NSPersistentContainer
    
    init() {
        // Create model programmatically for now
        let model = NSManagedObjectModel()
        
        // Create JournalEntryEntity
        let journalEntity = NSEntityDescription()
        journalEntity.name = "JournalEntryEntity"
        journalEntity.managedObjectClassName = "JournalEntryEntity"
        
        // Add attributes
        let idAttribute = NSAttributeDescription()
        idAttribute.name = "id"
        idAttribute.attributeType = .UUIDAttributeType
        idAttribute.isOptional = false
        
        let contentAttribute = NSAttributeDescription()
        contentAttribute.name = "content"
        contentAttribute.attributeType = .stringAttributeType
        contentAttribute.isOptional = false
        
        let titleAttribute = NSAttributeDescription()
        titleAttribute.name = "title"
        titleAttribute.attributeType = .stringAttributeType
        titleAttribute.isOptional = true
        
        let timestampAttribute = NSAttributeDescription()
        timestampAttribute.name = "timestamp"
        timestampAttribute.attributeType = .dateAttributeType
        timestampAttribute.isOptional = false
        
        journalEntity.properties = [idAttribute, contentAttribute, titleAttribute, timestampAttribute]
        model.entities = [journalEntity]
        
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