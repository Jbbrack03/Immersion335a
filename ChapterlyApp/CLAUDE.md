# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

Chapterly is a React Native 0.79.2 app for iOS/Android that connects to Audiobookshelf servers. It's a freemium app ($6.99 IAP) with EPUB3 audio sync support. The codebase is TypeScript-based with Redux Toolkit for state management.

## Key Commands

```bash
# Development
npm start                    # Start Metro bundler
npm run ios                 # Run on iOS simulator
npm run android             # Run on Android emulator

# Testing
npm test                    # Run all unit tests
npm test -- --watch         # Watch mode
npm test -- path/to/file    # Test specific file
npm run test:e2e:ios        # Run iOS E2E tests
npm run test:e2e:android    # Run Android E2E tests

# Code Quality
npm run lint                # ESLint check
npm run lint:fix           # Auto-fix linting issues
npm run type-check         # TypeScript validation

# Building
npm run bundle:ios         # Create iOS bundle
npm run bundle:android     # Create Android bundle
```

## Architecture

### State Management
- **Redux Toolkit** with typed hooks in `src/store/`
- Slices: `auth`, `collection`, `player`, `downloads`, `settings`
- Offline-first with sync queue in `src/utils/sync/SyncManager.ts`

### API Layer
- **BaseApiService** handles auth, retries, and token refresh
- **AudiobookshelfService** extends base for server communication
- **OpenLibraryService** for metadata enrichment
- Axios with Metro config workaround for RN 0.79

### Key Services
- **AudioPlayerService**: react-native-track-player wrapper
- **DownloadManager**: Queue-based with resume support
- **EPUB3Parser**: SMIL parsing and audio-text sync
- **EncryptionService**: Secure file storage

### Navigation
- React Navigation 7.x with 5 tabs: Home, Collection, Series, Library, Settings
- Empty states direct to Settings when no server connected

## Critical Implementation Notes

### React Native 0.79 + React 19 Compatibility
- Metro config requires Axios browser mode fix (see metro.config.js)
- Potential Detox E2E issues with Fabric - monitor for iOS crashes
- React Navigation native-stack may have Fabric issues on iOS

### Freemium Model
- Free: 10 books, 3 downloads, 1x speed, basic features
- Premium ($6.99): Unlimited everything + EPUB3 sync + variable speed
- Check `isPremium` in settings slice before enabling features

### Performance Targets
- App startup: <2s
- Screen navigation: <300ms
- Memory baseline: <200MB
- Test coverage: >70%

### Security
- Tokens in Keychain (react-native-keychain)
- Downloaded files encrypted with device-specific key
- No analytics/tracking (privacy-focused)

## Development Workflow

Follow the Implementation Roadmap phases:
1. Current phase tracked in README.md
2. Each phase has acceptance criteria
3. Test as you go - don't leave testing for Phase 14

### Error Handling
- Network timeout: Retry 3x with backoff
- 401: Refresh token once, then logout
- Sync conflicts: Server wins (last-write-wins)
- Downloads: Resume from last byte

### Testing Strategy
- Unit tests for services and utilities
- Component tests with React Native Testing Library
- Integration tests for navigation flows
- E2E only for critical paths (auth, playback, purchase)

## Key Files to Understand

- `src/api/services/BaseApiService.ts` - API foundation
- `src/store/index.ts` - State architecture
- `src/utils/epub3/EPUB3Parser.ts` - EPUB3 implementation
- `src/features/player/AudioPlayerService.ts` - Audio handling
- `Chapterly_Implementation_Roadmap.md` - Development phases

## Platform-Specific Notes

### iOS
- Background modes: audio, fetch, remote-notification
- Audio session: .playback with .spokenAudio
- Min version: iOS 15.1

### Android
- Foreground service for playback
- Scoped storage for downloads
- Min API: 24 (Android 7.0)

## Common Issues

- **Axios crypto error**: Check metro.config.js is properly configured
- **Navigation crashes**: Test thoroughly with New Architecture enabled
- **Download failures**: Ensure proper permissions and storage checks
- **Sync conflicts**: Always use server timestamp for resolution