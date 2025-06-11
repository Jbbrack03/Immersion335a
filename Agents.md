# **Immersion335a – Agents.md**

**OpenAI Codex Context File**  
Reads fast (≤ 120 chars/line), lives in repo root, must be kept current.  
Every contributor reads this *first*.

---

## **1 Purpose**

Deliver controller‑friendly quest dialog in WoW 3.3.5a by making **Immersion‑wotLK** correctly consume  
**ConsolePortLK** input APIs. Target parity with retail Immersion \+ ConsolePort combo.

## **2 Background**

* Immersion‑wotLK [https://github.com/s0h2x/Immersion-WotLK](https://github.com/s0h2x/Immersion-WotLK)  
* ConsolePortLK [https://github.com/leoaviana/ConsolePortLK](https://github.com/leoaviana/ConsolePortLK)  
* Issue: Immersion fails to detect controller, hence no A/B/X/Y glyphs or navigation.

## **3 High‑Level Goals**

1. **Fix detection layer** – Immersion‑wotLK recognises ConsolePortLK hooks/bindings.  
2. **No regressions** when either addon loaded solo.  
3. **Back‑port parity** with Immersion v1.2.9 ✚ ConsolePort v2.7 (where API permits).  
4. **Code health** – tidy diffs, docs, tests; follow upstream style.

## **4 Out of Scope**

* Moving beyond 3.3.5a API.  
* Brand‑new Immersion UX unrelated to controller input.  
* Support for non‑ConsolePort controller addons.

## **5 Repo & Branch Policy**

| Role | Repo | Remote | Default Branch | Notes |
| ----- | ----- | ----- | ----- | ----- |
| **Active project** | Immersion335a | Jbbrack03 | `main` | Primary repo – all development & PRs |
| Upstream Immersion | immersion-wotlk | s0h2x | `main` | Read‑only; sync fixes via patches |
| Upstream ConsolePort | consoleportlk | leoaviana | `master` | Read‑only |

### **Parallel Task Workflow**

* **One task → one short‑lived branch** off `main` in *Immersion335a*, prefix with type (e.g. `fix/`, `feat/`, `test/`).  
* **PR title** \= Conventional Commit \+ short scope (`fix(controller): …`).  
* **README.md** & **CHANGELOG.md** are *protected*: update **only** in a dedicated `docs/` PR created *after* code PRs merge to avoid conflicts.  
* Use GitHub squash‑merge; PR description auto‑feeds changelog entry.  
* **One task → one short‑lived branch** off `feat/consoleport`, prefix with type (e.g. `fix/`, `feat/`, `test/`).  
* **PR title** \= Conventional Commit \+ short scope (`fix(controller): …`).  
* **README.md** & **CHANGELOG.md** are *protected*: update **only** in a dedicated `docs/` PR created *after* code PRs merge to avoid conflicts.  
* Use GitHub squash‑merge; PR description auto‑feeds changelog entry.

## **6 Environment & Tooling**

* **Lua 5.1** runtime – `lua5.1` binary available in container.  
* Codex **offline** WebUI sandbox:  
  * No outbound network (HTTP\[S\] blocked), no package managers (Yarn, npm, luarocks).  
  * Filesystem root \= `/workspace`; project root \= `/workspace/Immersion335a`.

**Directory layout** (keep consistent):  
Immersion335a/  
  Immersion/         \-- addon Lua \+ TOC shipped into WoW  
  scripts/  
    mock\_cp.lua      \-- ConsolePortLK API stubs  
    wow\_api\_stub.lua \-- minimal WoW API for unit tests  
    run\_tests.sh     \-- one‑click test wrapper  
  libs/              \-- vendored third‑party Lua (busted, luacheck)

*   
* **Vendoring** – copy external libs into `/libs`; add to `package.path` via `scripts/test_helper.lua`.  
* **Tools present** – `git`, `lua5.1`, `luac`, POSIX shell utils; **no** make/cmake.  
* **Manual WoW client** (3.3.5a) runs outside sandbox for integration testing.

## **7 Task Types & DoD Task Types & DoD**

| Type | Definition of Done |
| ----- | ----- |
| **Analyse** | Markdown note pin‑pointing root cause; stacktrace; link to lines. |
| **Patch** | Lua compiles error‑free; manual checklist passes; unit stubs pass. |
| **Refactor** | Behaviour unchanged; tests still green; complexity ↓. |
| **Docs** | README/CHANGELOG updated *only* via docs PR; no merge conflicts. |

## **8 Testing Strategy (Offline Sandbox‑First)**

* **Unit** – small pure‑Lua modules tested with [`busted`](https://olivinelabs.com/busted/). Bundle `busted.lua` in repo.  
* **Stub harness** (`scripts/mock_cp.lua`) mocks ConsolePortLK API for sandbox runs.  
* **Static** – run `luacheck` (local copy) with upstream configs.  
* **Manual** – checklist:  
  1. Launch WoW 3.3.5a with both addons.  
  2. Start quest dialog; see controller glyphs.  
  3. Navigate entire dialog using gamepad only.  
  4. Disable ConsolePortLK → Immersion reverts to keyboard cues.  
  5. `/console reloadui` shows zero Lua errors.

## **9 Coding Standards**

* 4‑space indent, `local` everything, PascalCase globals.  
* ≤ 400 LOC per file; one logical change per commit.  
* Follow **Conventional Commits**; scope \= Lua file or feature.

## **10 Documentation & Release**

* Keep a Changelog format; next tag in `## [Unreleased]` until docs PR.  
* README lists **Quick Install** \+ **Known Issues**.  
* Zip `/Immersion` folder on GitHub Release; include version bump in TOC.

## **11 Codex Interaction Rules**

1. Provide short, explicit console commands & patch snippets.  
2. Use *Generate‑Then‑Execute*: draft → run offline tests → iterate.  
3. Ask clarifying questions early if any ambiguity.  
4. Keep external web searches narrow; cite in PR, not in committed code.  
5. When multiple tasks open, Codex must label outputs with branch name & avoid touching docs unless on docs branch.

---

*Last updated: 2025‑05‑20*

