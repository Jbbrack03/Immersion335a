import Foundation

public class AutosaveService {
    private let journalService: JournalService
    private var timer: Timer?
    private var draftContent: String?
    private var draftTitle: String?
    
    public init(journalService: JournalService) {
        self.journalService = journalService
    }
    
    public func startAutosave(interval: TimeInterval) {
        timer?.invalidate()
        timer = Timer.scheduledTimer(withTimeInterval: interval, repeats: true) { [weak self] _ in
            self?.performAutosave()
        }
    }
    
    public func updateDraft(content: String, title: String?) {
        self.draftContent = content
        self.draftTitle = title
    }
    
    private func performAutosave() {
        guard let content = draftContent, !content.isEmpty else { return }
        
        do {
            _ = try journalService.createEntry(content: content, title: draftTitle)
            // Clear draft after successful save
            draftContent = nil
            draftTitle = nil
        } catch {
            // Silent failure for autosave
        }
    }
    
    deinit {
        timer?.invalidate()
    }
}