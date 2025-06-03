# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

AuraMind is an AI-powered offline journal and mood analyzer application with absolute privacy as its core principle. All AI processing occurs on-device with no external data transmission.

### Key Architecture Principles
- **Privacy-First**: All data processing happens locally on-device
- **Offline-First**: Complete functionality without internet connectivity  
- **Native Development**: Separate iOS (Swift/SwiftUI) and Android (Kotlin/Compose) apps
- **Test-Driven Development**: Comprehensive test coverage is required
- **Security by Default**: End-to-end encryption for all user data

## Project Structure

This repository contains documentation, planning materials, and iOS implementation for the AuraMind application:

```
AI_Journal/
├── AI Journal App PRD.md          # Product Requirements Document
├── System_Architecture_Document.md # Technical architecture specifications
├── Technical_Requirements_Document.md # Detailed technical requirements
├── Implementation_Roadmap.md       # Development phases and milestones
├── ios/                           # iOS Swift Package implementation
│   ├── Package.swift              # Swift Package Manager configuration
│   ├── Sources/AuraMind/          # iOS source code
│   │   ├── Models/                # Data models (JournalEntry, Theme, Mood)
│   │   └── Services/              # Business logic services (JournalService)
│   └── Tests/AuraMindTests/       # XCTest test suite
└── CLAUDE.md                      # This file
```

### Test-Driven Development (MANDATORY - STRICTLY ENFORCED)

**ABSOLUTE RULES - NO EXCEPTIONS:**

## MANDATORY TDD CHECKPOINT QUESTIONS (Updated 5/28/2025)
Before writing ANY code (test or implementation), ask:

1. ❓ **"Have I reviewed existing code to understand patterns and conventions?"**
2. ❓ **"Have I researched unknowns and calculated test expectations correctly?"**
3. ❓ **"Have I written exactly ONE failing test?"**
4. ❓ **"Have I seen that ONE test fail?"**  
5. ❓ **"Am I about to implement ONLY what makes that ONE test pass?"**
6. ❓ **"Will I commit immediately after this test passes?"**

**If ANY answer is "no" - STOP and course-correct.**

## HANDLING SUSPECTED INCORRECT TESTS (Added 5/28/2025)
If you suspect a test expectation is incorrect:

1. **DO NOT immediately change the test** - TEST IMMUTABILITY is the default
2. **RESEARCH thoroughly** to validate your suspicion:
   - Check official documentation
   - Calculate expected values manually
   - Compare with similar existing tests for patterns
   - Write verification scripts if needed
3. **Document your findings** with clear evidence
4. **ONLY if research proves the test is wrong**, then:
   - Fix the test with a clear commit message explaining the error
   - Update implementation to match the corrected test
   - Add comments explaining the correct calculation
5. **If unsure**, respect TEST IMMUTABILITY and add TODO comments

**Example commit message for test correction:**
```
Fix incorrect B.LT test expectation

The test expected offset field 0xB (11) but should expect 0x3.
IR offset represents instruction count, not byte offset.
3 instructions * 4 bytes = 12 bytes = offset field 3.

Research: ARM64 encoding manual section X.Y.Z
```

## ENHANCED TDD WORKFLOW (Updated 6/2/2025)
**NEVER deviate from this sequence:**

1. **REVIEW existing codebase** - Study similar implementations and patterns
   - Look at how similar features are already implemented
   - Check documentation and architecture files
   - Understand project conventions and patterns
2. **RESEARCH unknowns** - Verify technical details BEFORE writing test
   - For iOS: Check Swift/SwiftUI documentation and patterns
   - For services: Review existing service patterns and error handling
   - Calculate expected values precisely
3. **Write ONE test** (and only one) with CORRECT expectations
4. **Run**: `swift test --filter "TestName"`
5. **Verify failure** (must see compilation error or test failure)
6. **Implement minimal code** (only what makes THIS test pass)
7. **Run same command**, verify pass
8. **Commit** with message: "Add [feature] support - RED/GREEN cycle complete"
9. **ONLY THEN** proceed to next test

**CRITICAL**: Each test is a complete story that deserves individual attention, not a step in a batch process.
**NEW EMPHASIS**: The REVIEW and RESEARCH steps prevent TEST IMMUTABILITY violations by ensuring test expectations are correct from the start.

## CRITICAL TDD LESSON (Added 6/2/2025)
**Tests MUST drive the correct architecture**, not just make tests pass:

❌ **WRONG**: Writing tests that can be satisfied with fake implementations
```swift
// Bad: This test can be satisfied without Core Data
func testFetchAll() {
    let entries = repository.fetchAll()
    XCTAssertEqual(entries.count, 1)
}
```

✅ **CORRECT**: Writing tests that force proper implementation
```swift
// Good: This test forces actual Core Data usage
func testPersistsToCoreData() {
    repository.create(entry)
    let request = NSFetchRequest<NSManagedObject>(entityName: "JournalEntry")
    let results = try context.fetch(request)
    XCTAssertEqual(results.count, 1)
}
```

## CORE TDD RULES

1. **RED-GREEN-REFACTOR ONLY**
   - RED: Write a failing test FIRST (test must fail)
   - GREEN: Write MINIMAL code to make test pass
   - REFACTOR: Clean up code while keeping tests green
   - **VIOLATION**: Writing ANY implementation code before a failing test

2. **TEST IMMUTABILITY**
   - Tests are CONTRACTS - once written, they CANNOT be changed
   - If a test seems wrong, the implementation is wrong, NOT the test
   - **VIOLATION**: Modifying any test to make implementation pass
   - **EXCEPTION**: Only if requirements genuinely changed (must document why)

3. **NO IMPLEMENTATION WITHOUT TESTS**
   - Every line of implementation code MUST have a corresponding test
   - No "temporary" code without tests
   - No "quick fixes" without tests
   - **VIOLATION**: Any implementation code not covered by tests

4. **SINGLE TEST DISCIPLINE**
   - Write exactly ONE test at a time
   - See it fail before writing ANY implementation
   - Implement ONLY what makes that ONE test pass
   - Commit before writing the next test
   - **VIOLATION**: Writing multiple tests before implementing any

## TDD VIOLATIONS TO AVOID
- ❌ Writing implementation then tests
- ❌ Modifying tests to match implementation
- ❌ Skipping tests for "simple" code
- ❌ **Writing multiple tests before implementing any**
- ❌ **Batching RED-GREEN cycles without commits**
- ❌ Commenting out failing tests
- ❌ Writing tests that always pass

## CORRECT TDD EXAMPLE
```cpp
// STEP 1: Write exactly ONE failing test
TEST(DecoderTest, DecodeADD) {
    uint8_t code[] = {0x01, 0xD8}; // add eax, ebx
    auto inst = decoder.decode(code, 2);
    EXPECT_EQ(inst.opcode, Opcode::ADD);  // This MUST fail first
}

// STEP 2: Run test, see failure
// STEP 3: Write minimal implementation  
// STEP 4: Test passes
// STEP 5: Commit immediately
// STEP 6: ONLY THEN write next test
```

## DISCIPLINE ENFORCEMENT
**Think of each test as a discipline exercise, not an efficiency optimization.**
- Each RED-GREEN cycle builds muscle memory
- Batching cycles breaks the discipline
- The goal is internalization, not speed


## Development Commands

### iOS Development (ACTIVE)
```bash
# Navigate to iOS project
cd /Users/jbbrack03/AI_Journal/ios

# Run all tests
swift test

# Run specific test
swift test --filter JournalEntryTests

# Build package
swift build

# Clean build
swift package clean
```

### Future iOS App Development
```bash
# Build iOS app (when Xcode project is created)
xcodebuild -scheme AuraMind -destination 'platform=iOS Simulator,name=iPhone 15'

# Run tests
xcodebuild test -scheme AuraMind -destination 'platform=iOS Simulator,name=iPhone 15'

# SwiftLint
swiftlint --strict
```

### Android Development  
```bash
# Build Android app
./gradlew clean assembleDebug

# Run tests
./gradlew test

# Lint
./gradlew ktlintCheck
```

## Core Technical Requirements

### Target Platforms
- **iOS**: Swift 5.9+, SwiftUI, Core ML, iOS 16+
- **Android**: Kotlin 1.9+, Jetpack Compose, TensorFlow Lite, API 26+

### Architecture Patterns
- **iOS**: MVVM-C (Model-View-ViewModel with Coordinators)
- **Android**: MVVM with Clean Architecture + Hilt DI

### AI/ML Requirements
- **On-device only**: No cloud-based AI services allowed
- **iOS**: Core ML, Natural Language Framework
- **Android**: TensorFlow Lite, ML Kit (selective usage)
- **Model constraints**: <25MB size, <100ms inference latency

### Data Layer
- **iOS**: Core Data with FileProtection.complete encryption
- **Android**: Room + SQLCipher with Android Keystore
- **Backup**: AES-256 encrypted user-initiated exports only

### Security Requirements
- Biometric/PIN authentication mandatory
- All user data encrypted at rest
- No sensitive data in logs or crash reports
- Privacy-preserving analytics only (if any)

## Key Components Architecture

### Core Services
1. **Journal Engine**: Entry CRUD, autosave, search functionality
2. **Mood Tracker**: Mood logging with multiple descriptors and trends
3. **AI Services**: Sentiment analysis, theme extraction, pattern recognition
4. **Prompt Generator**: Template-based personalized reflection prompts
5. **Analytics Engine**: Local trend analysis and visualizations

### Data Models (IMPLEMENTED)
- ✅ `JournalEntry`: Core entity with content, title, sentiment, themes, mood, timestamps
- ✅ `Mood`: Mood tracking with primary/secondary moods and intensity (0.0-1.0)
- ✅ `Theme`: Extracted themes with name and confidence scores
- ⏳ `SentimentAnalysis`: Score (-1.0 to 1.0), label, confidence (integrated into JournalEntry)

### Services Layer (IMPLEMENTED)
- ✅ `JournalService`: Complete CRUD operations with in-memory storage
  - ✅ Create entries with content and optional title
  - ✅ Update existing entries with ID preservation
  - ✅ Delete entries with proper error handling
  - ✅ Fetch entries with optional limit parameter
  - ✅ Search entries with case-insensitive content/title matching

## Development Guidelines

### Testing Requirements
- Minimum 80% code coverage for business logic
- 100% coverage for security/encryption modules  
- 90% coverage for AI/ML integration layers
- UI tests for critical user flows

### Performance Targets
- App launch: <2 seconds
- Entry save: <100ms
- AI analysis: <100ms (iOS), <200ms (Android)
- Memory usage: <150MB (iOS), <200MB (Android)

### Privacy Constraints
- No network calls for core functionality
- All AI processing must be verifiably on-device
- User data never leaves device unless explicitly exported by user
- Crash reports must be sanitized of personal information

## Current Status

**Project Phase**: Phase 2 Core Features - IN PROGRESS
- ✅ **Phase 1: Foundation - COMPLETED**
  - ✅ Product Requirements Document completed
  - ✅ System Architecture Document completed  
  - ✅ Technical Requirements Document completed
  - ✅ Implementation Roadmap completed
- ✅ **iOS Foundation Implementation completed (TDD)**
  - ✅ Core Data Models: JournalEntry, Theme, Mood with title support
  - ✅ Swift Package Manager setup
  - ✅ Strict TDD discipline maintained (12 RED-GREEN cycles total)
- ✅ **Phase 2: Week 5-6 Journal Engine - COMPLETED**
  - ✅ **Journal Service (CRUD operations) - COMPLETED**
  - ✅ Complete in-memory storage implementation
  - ✅ All 5 core operations: Create, Read, Update, Delete, Search
- 🚧 **Phase 2: Week 7-8 Data Persistence Layer - IN PROGRESS**
  - ✅ Core Data setup with FileProtection.complete encryption
  - ✅ PersistenceController with programmatic model creation
  - ✅ CoreDataJournalRepository with actual Core Data persistence
  - ✅ TDD violation corrected - tests now force proper architecture
  - ⏳ Repository update, delete, and search methods
  - ⏳ Integration with JournalService
  - ⏳ Data migration strategy
- ⏳ **Next: Phase 2 Remaining Features**
  - ⏳ Autosave functionality - Week 9-10
  - ⏳ Basic UI Implementation - Week 11-12
- ⏳ AI model training pipeline not yet established

### TDD Implementation History
**Commits demonstrating strict TDD discipline:**

**Phase 1 - Data Models (4 RED-GREEN cycles):**
1. `3614de2` - JournalEntry basic structure (RED/GREEN)
2. `120bffc` - Sentiment analysis support (RED/GREEN) 
3. `d21e17d` - Theme model and integration (RED/GREEN)
4. `068b457` - Mood model and integration (RED/GREEN)

**Phase 2 - Journal Service (5 RED-GREEN cycles):**
5. `8d07523` - JournalService createEntry with title support (RED/GREEN)
6. `5507b23` - JournalService updateEntry with ID preservation (RED/GREEN)
7. `786fe3e` - JournalService deleteEntry with error handling (RED/GREEN)
8. `d370276` - JournalService fetchEntries and searchEntries (RED/GREEN)

**Phase 2 - Data Persistence Layer (3 RED-GREEN cycles):**
9. `cd39093` - PersistenceController with FileProtection.complete (RED/GREEN)
10. `1e142d7` - CoreDataJournalRepository create method (RED/GREEN)
11. `5dfbbd8` - CoreDataJournalRepository fetchAll method (RED/GREEN)
12. `0fbb4aa` - **TDD Correction**: Fix to use actual Core Data persistence (RED/GREEN)

**Test Coverage**: 15/15 tests passing (100% success rate)
- JournalEntryTests: 4 tests (creation, sentiment, themes, mood)
- ThemeTests: 1 test (creation)
- MoodTests: 2 tests (basic creation, secondary moods)
- JournalServiceTests: 5 tests (create, read, update, delete, search)
- PersistenceControllerTests: 1 test (initialization with encryption)
- **CoreDataJournalRepositoryTests: 2 tests (create with Core Data verification)**

## Important Considerations for AI Development

When working on this project:

1. **Never suggest cloud-based AI solutions** - All AI must run on-device
2. **Prioritize model optimization** - Models must be quantized and compressed
3. **Maintain privacy guarantees** - No data transmission for core features
4. **Follow platform conventions** - Use native ML frameworks (Core ML, TFLite)
5. **Test thoroughly** - AI accuracy and performance must meet documented targets

## Dataset and Model Locations

**Training Data**: `/Users/jbbrack03/AI_Journal/data/datasets/`
**Model Outputs**: `/Users/jbbrack03/AI_Journal/data/models/`

Note: Model training infrastructure is not yet implemented.

## Next Steps for Implementation

**IMMEDIATE - Phase 2: Core Features (Week 7-12 per roadmap)**

1. **Data Persistence Layer** (Week 7-8) - IN PROGRESS
   - ✅ Core data models completed
   - ✅ Journal Service CRUD operations completed  
   - ✅ Core Data setup with encryption (FileProtection.complete)
   - ✅ Repository pattern implementation (create, fetchAll)
   - ✅ Actual Core Data persistence (TDD compliance restored)
   - ⏳ Repository update method with Core Data
   - ⏳ Repository delete method with Core Data
   - ⏳ Repository search with NSPredicate
   - ⏳ Integration of CoreDataJournalRepository with JournalService
   - ⏳ Data migration strategy
   - ⏳ Repository isolation tests

2. **Autosave & Advanced Features** (Week 9-10)
   - ⏳ Autosave functionality with timer-based persistence
   - ⏳ Date range filtering for fetchEntries
   - ⏳ Entry sorting by timestamp
   - ⏳ Backup/restore functionality

3. **Basic UI Implementation** (Week 11-12)
   - ⏳ SwiftUI views following MVVM-C pattern
   - ⏳ Journal list view
   - ⏳ Journal entry editor
   - ⏳ Settings screen

**FUTURE - Phase 3: AI Integration (Week 13-18)**
4. Create AI model training pipeline
5. Integrate Core ML for on-device processing
6. Build sentiment analysis and theme extraction
7. Implement comprehensive UI testing strategy

## TDD Next Test Priorities

Following strict TDD principles, the next failing tests should be:

1. **testUpdateJournalEntryPersistsToCoreData** - Force Core Data update implementation
2. **testDeleteJournalEntryRemovesFromCoreData** - Force Core Data delete implementation  
3. **testSearchJournalEntriesWithPredicate** - Force NSPredicate-based search
4. **testRepositoryIsolation** - Ensure different contexts maintain separate data
5. **testJournalServiceWithCoreDataRepository** - Integration test

## Code Quality Standards

- Follow platform-specific style guides (Swift API Design Guidelines, Kotlin Coding Conventions)
- Use dependency injection (Swift property wrappers, Hilt)
- Comprehensive documentation for all public APIs
- Test-driven development approach
- Regular code reviews and static analysis